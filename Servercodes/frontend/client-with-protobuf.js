/*
 * JS -> C++, Frontend -> Backend
 * To run, do the following scripts:
 *
 * node general-client.js
 */

const protobuf = require('protobufjs');

const PORT = 6969;
const PACKET = {
  VehicleInertialState: {
    vehicleid: 6,
    latitude: 34.05589,
    longitude: -117.819964,
  },
};

const client = net.connect(PORT, HOST);

client.on('connect', () => {
  sendMessage();
});

client.on('close', () => console.log('Client disconnected'));

client.on('error', () => console.log('No server detected'));

function sendMessage() {
  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  rl.question('Type anything to send a packet (-1 to exit): ', msg => {
    rl.close();
    if (msg !== '-1') {
      sendPacket(PACKET);
      sendMessage();
    } else {
      client.end();
    }
  });
}

function sendPacket(packet) {
  protobuf.load('./protobuf/ProtoPackets.proto')
    .then(root => {
      const Message = root.lookupType('ProtoPackets.Packet');

      const e = Message.verify(packet);
      if (e) throw Error(e);

      const message = Message.create(packet);
      const buffer = Message.encodeDelimited(message).finish();
      client.write(buffer);
    })
    .catch(console.error);
}
