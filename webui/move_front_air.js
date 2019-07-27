const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
//const parser = new Readline();

const serialOption = 
{
  baudRate: 115200,
  autoOpen: false
};

const dataBufferList = {};
const portList = {};
const legList = {};

openSerial = (portName) => {
  let port = new SerialPort(portName, serialOption);
  dataBufferList[portName] = '';
  portList[portName] = port;
  port.open(function (err) {
    if (err) {
      return console.log('Error opening port: ', err.message);
    }
    //port.pipe(parser);
    console.log('portName=<',portName ,'> is opened');
    setTimeout(()=> {
      trans2serial('info:r\n',portName);
    },5000);
  });

  port.on('data', function (response) {
    let resStr = response.toString('utf-8');
    //console.log('resStr=<',resStr ,'>');
    dataBufferList[portName] += resStr;
    let cmds = trySpliteResponse(portName);
    //console.log('cmds=<',cmds ,'>');
    try {
      for(let i = 0;i < cmds.length;i++) {
        tryParseResponse(cmds[i],portName);     
      }
    } catch(e) {
      console.log('e=<',e ,'>');
    }
  });
}


const spacerCommand = '&$';
const spacerLength = spacerCommand.length;


trySpliteResponse = (portName) => {
  let cmds = [];
  let cmdRC = dataBufferList[portName].split(spacerCommand);
  //console.log('trySpliteResponse cmdRC=<',cmdRC ,'>');
  dataBufferList[portName] = cmdRC[cmdRC.length -1];
  if(cmdRC.length > 1) {
    cmds = cmdRC.slice(0,cmdRC.length -1);
  }
  return cmds;
}

tryParseResponse = (resText,portName) => {
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
      onJsonResponse(json,portName);
    } else {
      console.log('tryParseResponse resText=<',resText ,'>');
    }
  } else {
    console.log('tryParseResponse resText=<',resText ,'>');
  }
}

const onJsonResponse = (msg,portName) => {
  console.log('onJsonResponse msg=<',msg ,'>');
  console.log('onJsonResponse portName=<',portName ,'>');
  if(msg && msg.info) {
    onInfoMsg(msg.info,portName);
  }
}

const onRunFront = () => {
  for(let leg in legList) {
    let moveMsg = 'legM:id,' + leg + ':xmm,90:payload,0\r\n';
    console.log('onRunFront moveMsg=<',moveMsg ,'>');
    let port = legList[leg];
    port.write(moveMsg, (err) => {
      if (err) {
        return console.log('Error on write: ', err.message);
      }
      console.log('onRunFront moveMsg=<', moveMsg,'>');
    });
  }
}


const onInfoMsg = (info,portName) => {
  console.log('onInfoMsg info=<',info ,'>');
  const leg0 = info.id0;
  legList[leg0] = portList[portName];
  const leg1 = info.id1;
  legList[leg1] = portList[portName];
  //console.log('onInfoMsg legList=<',legList ,'>');
  setTimeout(onRunFront,2000);
}

trans2serial = (msg,portName) => {
  let port = portList[portName];
  //console.log('trans2serial msg=<', msg,'>');
  port.write(msg, function(err) {
    if (err) {
      return console.log('Error on write: ', err.message);
    }
    console.log('trans2serial msg=<', msg,'>');
  });
}



const onListSerial = () => {
  SerialPort.list((err, ports) => {
    console.log('onListSerial ports=<',ports ,'>');
    for(let port of ports) {
      openSerial(port.comName);
    }
  });
}
onListSerial();

