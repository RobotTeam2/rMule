const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
const parser = new Readline();

const port = new SerialPort('COM3', {
  baudRate: 115200,
  autoOpen: false
});



port.open(function (err) {
  if (err) {
    return console.log('Error opening port: ', err.message);
  }
  port.pipe(parser);
  setTimeout(onWriteTimerRight,1000);
});


port.on('data', function (response) {
  //console.log('response=<', response.toString('utf-8'),'>');
});


const wheelRight = {
  wheel:{
    f:0,
    s:130
  },
  L:{
    g:1,
    d:0
  }
};

const wheelLeft = {
  wheel:{
    f:1,
    s:130
  },
  L:{
    g:1,
    d:0
  }
};

const wheelStop = {
  wheel:{
    f:1,
    s:0
  },
  L:{
    g:1,
    d:0
  }
};


onWriteTimerRight = () => {
  port.write(JSON.stringify(wheelRight), function(err) {
    if (err) {
      console.log('Error on write: ', err.message);
    }
    console.log('write wheelRight=<', wheelRight,'>');
    setTimeout(onWriteTimerLeft,900);
  });
};

let counterLoop = 0;
onWriteTimerLeft = () => {
  port.write(JSON.stringify(wheelLeft), function(err) {
    if (err) {
       console.log('Error on write: ', err.message);
    }
    console.log('write wheelLeft=<', wheelLeft,'>');
    if(counterLoop++ > 1) {
      setTimeout(onWriteTimerStop,1);
    } else {
      setTimeout(onWriteTimerRight,900);      
    }
  });
}


onWriteTimerStop = () => {
  port.write(JSON.stringify(wheelStop), function(err) {
    if (err) {
      console.log('Error on write: ', err.message);
    }
    console.log('write wheelStop=<', wheelStop,'>');
  });
}

