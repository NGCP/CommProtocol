#include <CommProto/comms.h>
#include <CommProto/architecture/macros.h>

#include <CommProto/network/udplink.h>
#include <CommProto/network/seriallink.h>
#include <CommProto/network/xbeelink.h>

#include <CommProto/debug/comms_debug.h>

#include <CommProto/callback.h>

using namespace comnet;

/***********************************************/
/******************* Private *******************/
/***********************************************/


/**
Helper function to convert between C++ and C function signatures
due to casting as a class member being incompatible with C style
thread creation APIs. Static linkage helps with that.
*/
void* Comms::commuincationHelperSend(void* context)
{
	return ((Comms*)context)->commuincationHandlerSend();
}

void* Comms::commuincationHelperRecv(void* context)
{
	return ((Comms*)context)->commuincationHandlerRecv();
}


/** function for communication thread */
void* Comms::commuincationHandlerSend()
{
	while (this->isRunning())
	{
		if (!sendQueue->isEmpty())
		{
			//send data here
			ObjectStream *temp = sendQueue->front();
			sendQueue->deQueue();
			connectionLayer->send(temp->headerPacket.destID, temp->getBuffer(), temp->getSize());
			free_pointer(temp);
		}
	}
	return 0;
}

/** function for communication thread */
void* Comms::commuincationHandlerRecv() {
  while (this->isRunning()) {
    AbstractPacket* packet = NULL;
    //send data here
	  uint8_t streamBuffer[MAX_BUFFER_SIZE];
    uint32_t recvLen = 0;
    connectionLayer->recv(streamBuffer, &recvLen);
    ObjectStream *temp = new ObjectStream();
    temp->setBuffer((char*)streamBuffer, recvLen);

    /*
      Algorithm should get the header, get the message id from header, then
      produce the packet from the header, finally get the callback.
     */
    if (temp->getSize() > 0) {
      header_t header = temp->deserializeHeader();
      // Create the packet.
      packet = this->packetManager.produceFromId(header.messageID);
    
      if (packet) {
        // Unpack the object stream.
        packet->unpack(*temp);
        Callback* callback = NULL;
        callback = this->packetManager.get(*packet);

        if (callback) {
          error_t error;
          /*
            TODO(Wallace): This might need to run on a separate thread, or 
            on a new thread, to prevent it from stopping the receive handler.
            User figures out what to do with the packet.
          */
          error = callback->callFunction(header, *packet);
          // Handle error.
          switch (error) {
            case CALLBACK_SUCCESS:
              cout << "Successful callback" << endl; break;
            default:  
              cout << "Nothing" << endl;
          };
        } else {
          // store the packet into the receive queue.
          recvQueue->enQueue(packet);
        }
      } else {
        COMMS_DEBUG("Unknown packet recieved.\n");
      }	
    }

    free_pointer(temp);				
  }

  return 0;
}

/***********************************************/
/******************* Public  *******************/
/***********************************************/
Comms::Comms(uint8_t platformID)
: CommNode(platformID)
{
	this->recvQueue = new AutoQueue <AbstractPacket*>;
	this->sendQueue = new AutoQueue <ObjectStream*>;
	mutex_init(&sendMutex);
	mutex_init(&recvMutex);
	connectionLayer = NULL;
}

Comms::~Comms()
{
	free_pointer(connectionLayer);
	mutex_destroy(&sendMutex);
	mutex_destroy(&recvMutex);
}

bool Comms::initConnection(transport_protocol_t connectionType, const char* port, const char* address, uint32_t baudrate)
{
	uint16_t length = 0;
	switch (connectionType)
	{
		case UDP_LINK:
		{			
			
			str_length(address, length);
			if (length < ADDRESS_LENGTH)
			{	
			  COMMS_DEBUG("UDP connection.\n");
				connectionLayer = new UDPLink();
				return connectionLayer->initConnection(port, address);
			}
			break;
		}
		case SERIAL_LINK:
		{
			
			str_length(address, length);
			if (length < ADDRESS_LENGTH)
			{
				connectionLayer = new SerialLink();
				return connectionLayer->initConnection(port, NULL, baudrate);
			}
			break;
		
		}
		case ZIGBEE_LINK:
		{
      str_length(address, length);
      if (length < ADDRESS_LENGTH) {
        connectionLayer = new experimental::XBeeLink();
        return connectionLayer->initConnection(port, NULL, baudrate);
      }
      break;
    }
		default:
		  COMMS_DEBUG("NO CONNECTION\n");
		{return false;}
	}
	return true;
}


bool Comms::addAddress(uint8_t destID, const char* address , uint16_t port)
{
	if (connectionLayer == NULL)return false;
	return connectionLayer->addAddress(destID, address, port);
}


bool Comms::removeAddress(uint8_t destID)
{
	if (connectionLayer == NULL)return false;
	return connectionLayer->removeAddress(destID);
}


bool Comms::send(AbstractPacket* packet, uint8_t destID) {
  if (connectionLayer == NULL) { 
    return false;
  }
  
  ObjectStream *stream = new ObjectStream();
  // Pack the stream with the packet.		
  packet->pack(*stream);		
  header_t header;

  header.destID = destID;
  header.sourceID = this->getNodeId();
  header.messageID = packet->getId();
  header.messageLength = stream->getSize();
  //
  //call encryption here
  //
  stream->serializeHeader(header);
  sendQueue->enQueue(stream);

  return true;
}


AbstractPacket* Comms::receive(uint8_t&  sourceID) {
  if (connectionLayer == NULL) return NULL;
  if (!recvQueue->isEmpty()) {
    cout << "Message recv in Comms" << endl;
    // This is a manual receive function. The user does not need to call this function,
    // however it SHOULD be used to manually grab a packet from the "orphanage" queue.
    recvQueue->deQueue();  
  }
	
  return NULL;
}


int32_t Comms::run()
{
	thread_create(&this->communicationThreadSend, commuincationHelperSend, this);
	thread_create(&this->communicationThreadRecv, commuincationHelperRecv, this);
	return CommNode::run();
}


int32_t Comms::stop()
{
	return CommNode::stop();
}


int32_t Comms::pause()
{
  return CommNode::pause();
}