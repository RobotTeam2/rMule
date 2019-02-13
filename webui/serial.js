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
  //setTimeout(onWriteTimerRight,1000);
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


const WebSocket = require('ws'); 
const wss = new WebSocket.Server({ host:'127.0.0.1',port: 18081 });

onWSSMsg = (msg) => {
  console.log('onWSSMsg msg=<', msg,'>');
  let jsonMsg = JSON.parse(msg);
  if(jsonMsg) {
    trans2serial(jsonMsg);
  }
};

onWSSConnected = (ws) => {
  //console.log('onWSSConnected ws=<', ws,'>');
  ws.on('message',onWSSMsg);  
}

wss.on('connection', onWSSConnected);

trans2ws = (msg) => {
  //console.log('trans2ws msg=<', msg,'>');
  wss.clients.forEach(function each(client) {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify(msg));
    }
  });
}

trans2serial = (msg) => {
  //console.log('trans2ws msg=<', msg,'>');
  port.write(JSON.stringify(msg), function(err) {
    if (err) {
      return console.log('Error on write: ', err.message);
    }
    console.log('trans2serial msg=<', msg,'>');
  });
}

/*
test.run.
*/
const wheelRight = {
  wheel:{
    f:0,
    s:125
  },
  L:{
    g:1,
    d:0
  }
};

const wheelLeft = {
  wheel:{
    f:0,
    s:125
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
    setTimeout(onWriteTimerLeft,1500);
  });
};

let counterLoop = 0;
onWriteTimerLeft = () => {
  port.write(JSON.stringify(wheelLeft), function(err) {
    if (err) {
       console.log('Error on write: ', err.message);
    }
    console.log('write wheelLeft=<', wheelLeft,'>');
    if(counterLoop++ > 10) {
      setTimeout(onWriteTimerStop,1);
    } else {
      setTimeout(onWriteTimerRight,1500);      
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


