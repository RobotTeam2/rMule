const SerialPort = require('serialport');
const { fork  } = require('child_process')

SerialPort.list((err, ports) => {
  console.log('ports=<',ports ,'>');
  for(let port of ports) {
    if(port.comName) {
      child = fork('port.js',[port.comName]);
    }
  }
});  
