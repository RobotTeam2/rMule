const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
//const parser = new Readline();



const port = new SerialPort('COM8', {
  baudRate: 115200,
  autoOpen: false
});

const spacerCommand = '&$';
const spacerLength = spacerCommand.length;

port.open(function (err) {
  if (err) {
    return console.log('Error opening port: ', err.message);
  }
  //port.pipe(parser);
});

let dataBuffer = '';
port.on('data', function (response) {
  let resStr = response.toString('utf-8');
  //console.log('resStr=<',resStr ,'>');
  dataBuffer += resStr;
  let cmds = trySpliteResponse();
  //console.log('cmds=<',cmds ,'>');
  try {
    for(let i = 0;i < cmds.length;i++) {
      tryParseResponse(cmds[i]);     
    }
  } catch(e) {
    console.log('e=<',e ,'>');
  }
});

trySpliteResponse = () => {
  let cmds = [];
  let cmdRC = dataBuffer.split(spacerCommand);
  //console.log('trySpliteResponse cmdRC=<',cmdRC ,'>');
  dataBuffer = cmdRC[cmdRC.length -1];
  if(cmdRC.length > 1) {
    cmds = cmdRC.slice(0,cmdRC.length -1);
  }
  return cmds;
}

tryParseResponse = (resText) => {
  //console.log('tryParseResponse resText=<',resText ,'>');
  let param = resText.trim().split(':');
  //console.log('tryParseResponse param=<',param ,'>');
  let json = {};
  if(param.length > 1) {
    let top = param[0];
    json[top] = {};
    for(let i = 1;i < param.length;i++) {
      //console.log('tryParseResponse param[i]=<',param[i] ,'>');
      let param2 = param[i].trim().split(',');
      //console.log('tryParseResponse param2=<',param2 ,'>');
      if(param2.length > 1) {
        let key = param2[0];
        json[top][key] = param2[1];
      }
    }
    console.log('tryParseResponse json=<',json ,'>');
    if(json) {
      trans2ws(json);
    } else {
      console.log('tryParseResponse resText=<',resText ,'>');
    }
  } else {
    console.log('tryParseResponse resText=<',resText ,'>');
  }
}


const WebSocket = require('ws'); 
const wss = new WebSocket.Server({ host:'127.0.0.1',port: 18081 });

onWSSMsg = (msg) => {
  console.log('onWSSMsg msg=<', msg,'>');
  if(msg.startsWith('list_serial:')) {
    SerialPort.list((err, ports) => {
      //console.log('ports=<',ports ,'>');
      const serial = {serial:{ports}};
      trans2ws(serial);
      /*
      for(let port of ports) {
        console.log('port=<',port ,'>');
        trans2ws(ports)
      }
      */
    });    
  } else {
    trans2serial(msg);
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
  port.write(msg, function(err) {
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


