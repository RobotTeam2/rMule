const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
//const parser = new Readline();

const port = new SerialPort('COM3', {
  baudRate: 115200,
  autoOpen: false
});


port.open(function (err) {
  if (err) {
    return console.log('Error opening port: ', err.message);
  }
  //port.pipe(parser);
  setTimeout(onTestGPIO,1000);
});

let dataBuffer = '';
port.on('data', function (response) {
  let resStr = response.toString('utf-8');
  console.log('resStr=<',resStr ,'>');
  try {
    tryParseResponse(resStr)
  } catch(e) {
    dataBuffer += resStr;
    tryParseMultiLine();
  }
});



tryParseMultiLine = () => {
  try {
    //console.log('dataBuffer=<',dataBuffer ,'>');
    let start = dataBuffer.indexOf('{');
    //console.log('start=<',start ,'>');
    let end = dataBuffer.indexOf('}') + 1;
    if(start > 0 && end > start) {
      let jsonLike = dataBuffer.substring(start,end);
      console.log('jsonLike=<',jsonLike ,'>');
      let good = tryParseResponse(jsonLike);
      if(good) {
        let remain = dataBuffer.substring(end);
        dataBuffer = remain;
        if(dataBuffer.length > 0) {
          tryParseMultiLine();
        }
      }
    }      
  } catch(e2) {
    console.log('e2=<',e2 ,'>');
    console.log('dataBuffer=<',dataBuffer ,'>');
  }  
}

tryParseResponse = (resStr) => {
  let jsonResp = JSON.parse(resStr);
  if(jsonResp) {
    trans2ws(jsonResp);
    return true;
  } else {
    console.log('tryParseResponse resStr=<',resStr ,'>');
  }
  return false;
}

HEX2DEC = (hex) => {
  let dec = parseInt('0x' + hex).toString();
  //console.log('HEX2DEC dec=<', dec,'>');
  return dec;
};

let data = HEX2DEC('A1');

const gpioObject = {
  G:{
/*
    HEX2DEC('A1'):0,
    HEX2DEC('A2'):0,
    HEX2DEC('A3'):0,
    HEX2DEC('A4'):0,
    HEX2DEC('A5'):0,
    HEX2DEC('A6'):0,
    HEX2DEC('A7'):0,
*/
  }
};

gpioObject.G[HEX2DEC('A1')] = 0;
/*
gpioObject.G[HEX2DEC('A2')] = 0;
gpioObject.G[HEX2DEC('A3')] = 0;
gpioObject.G[HEX2DEC('A4')] = 0;
gpioObject.G[HEX2DEC('A5')] = 0;
gpioObject.G[HEX2DEC('A6')] = 0;
gpioObject.G[HEX2DEC('A6')] = 0;
*/

const gpio1 = 'gpio:'+HEX2DEC('A1')+',0\n';
const gpio2 = 'gpio:'+HEX2DEC('A6')+',0\n';

onTestGPIO = () => {
  port.write(gpio1, function(err) {
    if (err) {
      console.log('Error on write: ', err.message);
    }
    console.log('onTestGPIO gpio=<', gpio1,'>');
  });
  setTimeout(onTestGPIO2,1);
}

onTestGPIO2 = () => {
  port.write(gpio2, function(err) {
    if (err) {
      console.log('Error on write: ', err.message);
    }
    console.log('onTestGPIO2 gpio=<', gpio2,'>');
  });  
};

// s:pwm f:direction
const wheelStop = 'wheel:s,0:f,0\n';
const wheelA = 'wheel:s,254:f,0\n';
const wheelB = 'wheel:s,254:f,1\n';

// d ->1 milsec g:directorn(ground)
const linearStop = 'linear:d,0:g,0\n';
const linearA = 'linear:d,1:g,0\n';
const linearB = 'linear:d,1:g,1\n';

