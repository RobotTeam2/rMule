const SerialPort = require('serialport');
const port = new SerialPort('COM4', {
  baudRate: 115200,
  autoOpen: false
});



port.open(function (err) {
  if (err) {
    return console.log('Error opening port: ', err.message);
  }
  setInterval(onWriteTimer,1000);
});


port.on('data', function (response) {
  console.log('response=<', response.toString('utf-8'),'>');
});


const wheelF = {
  wheel:{
    f:1,
    s:200
  }
};

const wheelB = {
  wheel:{
    f:0,
    s:200
  }
};

let counter = 0;
onWriteTimer = () => {
  let wheel = wheelF;
  if(counter++ %2 > 0) {
    wheel = wheelB;
  }
  port.write(JSON.stringify(wheel), function(err) {
    if (err) {
      return console.log('Error on write: ', err.message);
    }
    console.log('write wheel=<', wheel,'>');
  });  
};
