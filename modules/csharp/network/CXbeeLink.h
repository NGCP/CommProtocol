/*
Copyright (C) 2016  Michael Wallace, Mario Garcia.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __CXBEELINK_H
#define __CXBEELINK_H

#include <CommProto/network/XBeeLink.h>
#include <network/CCommsLink.h>

namespace Comnet {
namespace Network {


public ref class CXBeeLink : public CCommsLink {
public:
  CXBeeLink();
  ~CXBeeLink();
  /**
  Initialize the connection.
  */
  Boolean InitConnection(String^ port, String^, uint32_t baudrate) override;
  /**
  Add an address to the link.
  */
  Boolean AddAddress(uint8_t destID, String^ address, uint16_t port) override;
  /**
  Remove an address from the link.
  */
  Boolean RemoveAddress(uint8_t destID) override;
  /**
  Send a message to the specified id of which an address is associated with.
  */
  Boolean Send(uint8_t destID, uint8_t* txData, uint32_t txLength) override;
  /**
  Receive a message from a protocol id.
  */
  Boolean Recv(uint8_t* rxData, UInt32% rxLength) override;

private:
  Experimental::XBeeLink* node;
};
} // Network
} // Comnet
#endif // __CXBEELINK_H