const SerialPort = require('serialport');
const serialOption = 
{
  baudRate: 115200,
  autoOpen: false
};

let dataBuffer = '';
const openSerial = (portName) => {
  let port = new SerialPort(portName, serialOption);
  port.open(function (err) {
    if (err) {
      return console.log('Error opening port: ', err.message);
    }
    setTimeout(()=> {
      trans2serial(port,'who:r\n');
      trans2serial(port,'info:r\n');
    },5000);
  });

  port.on('data', function (response) {
    let resStr = response.toString('utf-8');
    trans2motion(resStr);
    console.log('resStr=<',resStr ,'>');
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
}

const spacerCommand = '&$';
const spacerLength = spacerCommand.length;

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
      onJsonUart(json);
    } else {
      console.log('tryParseResponse resText=<',resText ,'>');
    }
  } else {
    console.log('tryParseResponse resText=<',resText ,'>');
  }
}

const onJsonUart = (json) => {
  console.log('onJsonUart json=<',json ,'>');
  if(json.info) {
    onInfoUart(json.info);
  }
};

const onInfoUart = (info) => {
  console.log('onInfoUart info=<',info ,'>');
  if(info.id0) {
    createRedisChannel(info.id0);
  }
  if(info.id1) {
    createRedisChannel(info.id1);
  }
};
console.log('::process.argv=<',process.argv,'>')
openSerial(process.argv[2]);



const redis = require('redis');
const redisOption = {
  host:'node2.ceph.wator.xyz',
  port:6379,
  family:'IPv6'
};
const redisNewsChannelDiscovery = 'redis.channel.news.discover';
const gPub = redis.createClient(redisOption);
let gPubChannel = false;
const createRedisChannel = (channel) => {
  console.log('createRedisChannel:channel=<',channel,'>')
  const fromUart = 'hexa-horse-uart-arduino-' + channel + '<-uart';
  gPubChannel = fromUart;
  
  const toUart = 'hexa-horse-uart-arduino-' + channel + '->uart';
  const sub = redis.createClient(redisOption);
  sub.subscribe(toUart);
  sub.on('message', (channel, message) => {
    onMessage(channel, message);
  });
}

const onMessage = (channel, message) => {
  console.log('onMessage:channel=<',channel,'>')
}




const trans2serial = (port,msg) => {
  //console.log('trans2ws msg=<', msg,'>');
  port.write(msg, function(err) {
    if (err) {
      return console.log('Error on write: ', err.message);
    }
    console.log('trans2serial msg=<', msg,'>');
  });
}

const trans2motion =(msg) => {
  if(gPubChannel) {
    gPub.publish(gPubChannel,msg);
  }
}