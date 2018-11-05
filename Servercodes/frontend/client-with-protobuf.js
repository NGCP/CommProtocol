/*
 * JS -> C++, Frontend -> Backend
 * To run, do the following scripts:
 *
 * node client-with-protobuf.js
 */

const net = require('net');
const readline = require('readline');
const protobuf = require('protobufjs');

const HOST = '127.0.0.1';
const PORT = 59000;

const PACKET = {
  VehicleInertialState: {
    vehicleid: 6,
    latitude: 34.05589,
    longitude: -117.819964,
  },
};

const Packet = protobuf.loadSync('./ProtoPackets.proto').lookup('ProtoPackets.Packet');

const client = net.connect(PORT, HOST);

let rl;

client.on('connect', sendMessage);

client.on('data', receiveMessage);

client.on('close', () => {
	if (rl) console.log();
	console.log('Client disconnected');
  if (rl) rl.close();
});

client.on('error', () => {
	if (rl) console.log();
	console.log('No server detected');
  if (rl) rl.close();
});

async function sendMessage() {
  rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  rl.question('Type anything to send a packet (-1 to exit): ', async msg => {
    rl.close();
    if (msg !== '-1') {
      await sendPacket(PACKET);
      sendMessage();
    } else {
      client.end();
    }
  });
}

/* Helper for sendMessage */
async function sendPacket(packet) {
  const e = Packet.verify(packet);
  if (e) throw Error(e);

  const message = Packet.create(packet);
  const buffer = Packet.encodeDelimited(message).finish();
  client.write(buffer);
}

function receiveMessage(message) {
  const packet = receivePacket(message);
  process.stdout.write('\nMessage Received: ');
  console.log(packet);
  process.stdout.write('Type anything to send a packet (-1 to exit): ');
}

/* Helper for receiveMessage */
function receivePacket(buffer) {
  const message = Packet.decodeDelimited(buffer);
  const data = Packet.toObject(message);

  for (const packetType in data) {
    return {
      type: packetType,
      data: data[packetType],
    };
  }
  return null;
}
