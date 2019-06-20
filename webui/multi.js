const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
//const parser = new Readline();

const serialOption = 
{
  baudRate: 115200,
  autoOpen: false
};

let gLegSerialPort = {};

let gLegSerialPortInternal = {};
let gLegSerialPortBuffer = {};

SerialPort.list((err, ports) => {
  //console.log('ports=<',ports ,'>');
  for(let port of ports) {
    //console.log('port=<',port ,'>');
    let portTry = new SerialPort(port.comName, serialOption);
    portTry.open((err) => {
      console.log('portTry=<',portTry.path ,'> is opened');
      if(err) {
        throw err;
      }
      gLegSerialPortBuffer[portTry.path] = '';
      gLegSerialPortInternal[portTry.path] = portTry;
      onInitAnySerial(portTry);
    })
  }
});

onInitAnySerial = (port) => {
  requestInfo(port);
  port.on('data',(data)=> {
    //console.log('onInitAnySerial port.path=<',port.path ,'>');
    //console.log('onInitAnySerial data=<',data ,'>');
    let dataStr = data.toString('utf-8');
    //console.log('onInitAnySerial dataStr=<',dataStr ,'>');
    gLegSerialPortBuffer[port.path] += dataStr;
    let cmds = trySpliteResponseMulti(port.path);
    //console.log('onInitAnySerial cmds=<',cmds ,'>');
    try {
      for(let i = 0;i < cmds.length;i++) {
        tryParseResponseMulti(cmds[i],port.path);     
      }
    } catch(e) {
      console.log('onInitAnySerial e=<',e ,'>');
    }
  });  
}

requestInfo = (port) => {
  let msg = 'info:\r\n';
  setTimeout(()=> {
    port.write(msg, (err) => {
      if (err) {
        console.log('requestInfo err=<', err,'>');
        throw err;
      }
      console.log('requestInfo msg=<',msg ,'>');
    });    
  },5000);
}


onJsonMsg = (msg,port) => {
  console.log('onJsonMsg msg=<',msg ,'>');
  console.log('onJsonMsg port=<',port ,'>');
  if(msg && msg.info) {
    onLegInfo(msg.info,port);
  }
}

onLegInfo = (info,portName) => {
  //console.log('onLegInfo info=<',info ,'>');
  //console.log('onLegInfo portName=<',portName ,'>');
  let leg1 = info.id0;
  let leg2 = info.id1;
  let port = gLegSerialPortInternal[portName];
  //console.log('onLegInfo port=<',port ,'>');
  gLegSerialPort[leg1] = port;
  gLegSerialPort[leg2] = port;
  //console.log('onLegInfo gLegSerialPort=<',gLegSerialPort ,'>');
}




const spacerCommand = '&$';
const spacerLength = spacerCommand.length;


trySpliteResponseMulti = (name) => {
  let cmds = [];
  let cmdRC = gLegSerialPortBuffer[name].split(spacerCommand);
  //console.log('trySpliteResponse cmdRC=<',cmdRC ,'>');
  gLegSerialPortBuffer[name] = cmdRC[cmdRC.length -1];
  if(cmdRC.length > 1) {
    cmds = cmdRC.slice(0,cmdRC.length -1);
  }
  return cmds;
}


tryParseResponseMulti = (resText,port) => {
  //console.log('tryParseResponseMulti resText=<',resText ,'>');
  let param = resText.trim().split(':');
  //console.log('tryParseResponseMulti param=<',param ,'>');
  let json = {};
  if(param.length > 1) {
    let top = param[0];
    json[top] = {};
    for(let i = 1;i < param.length;i++) {
      //console.log('tryParseResponseMulti param[i]=<',param[i] ,'>');
      let param2 = param[i].trim().split(',');
      //console.log('tryParseResponseMulti param2=<',param2 ,'>');
      if(param2.length > 1) {
        let key = param2[0];
        json[top][key] = param2[1];
      }
    }
    //console.log('tryParseResponseMulti json=<',json ,'>');
    if(json) {
      onJsonMsg(json,port);
    } else {
      console.log('tryParseResponseMulti resText=<',resText ,'>');
    }
  } else {
    console.log('tryParseResponseMulti resText=<',resText ,'>');
  }
}






const WebSocket = require('ws'); 
const wss = new WebSocket.Server({ host:'127.0.0.1',port: 18081 });

onWSSMsg = (msg) => {
  console.log('onWSSMsg msg=<', msg,'>');
  if(msg.startsWith('serial:list,')) {
    onListSerial();
  } else if(msg.startsWith('serial:open,')) {
    onOpenSerial(msg);
  } else {
    trans2serial(msg);
  }
};

onListSerial = () => {
  SerialPort.list((err, ports) => {
    console.log('ports=<',ports ,'>');
    const serial = {serial:ports};
    if(port) {
      serial.open = true;
    } else {
      serial.open = false;
    }
    trans2ws(serial);
  });  
}

onOpenSerial = (msg) => {
  console.log('onOpenSerial msg=<',msg ,'>');
  const portName = msg.replace('serial:open,','').trim();
  console.log('onOpenSerial portName=<',portName ,'>');
  openSerial(portName);
}


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


