let ws = new WebSocket('ws://127.0.0.1:18081');

ws.onopen = (evt) => {
  setTimeout(onWSReady,1);
}

ws.onmessage = (evt) => { 
  let received_msg = evt.data;
  //console.log('onmessage received_msg=<', received_msg,'>');
  try {
    let jsonMsg = JSON.parse(received_msg);
    console.log('onmessage jsonMsg=<', jsonMsg,'>');
    if(jsonMsg) {
      if(jsonMsg.serial) {
        if(jsonMsg.open) {
          readLegInfo();
        } else {
          onSerialPort(jsonMsg.serial);
        }
      }
      if(jsonMsg.info) {
        onLegInfo(jsonMsg.info);
      }
      if(jsonMsg.wheel) {
        if(jsonMsg.wheel.vol0) {
          onVolumeWheelVol(jsonMsg.wheel.vol0,0);
        }
        if(jsonMsg.wheel.vol1) {
          onVolumeWheelVol(jsonMsg.wheel.vol1,1);
        }
      }
      //console.log('onmessage typeof jsonMsg.leg=<', typeof jsonMsg.leg,'>');
      if(typeof jsonMsg.leg === 'number') {
        onInfoLeg(jsonMsg.leg);
      }
      if(typeof jsonMsg.iEROMWheelLimitBack === 'number') {
        onInfoEROMWheelLimitBack(jsonMsg.iEROMWheelLimitBack);
      }
      if(typeof jsonMsg.iEROMWheelLimitFront === 'number') {
        onInfoEROMWheelLimitFront(jsonMsg.iEROMWheelLimitFront);
      }
    }
  } catch (e) {
    console.error('onmessage e=<', e,'>');
    console.error('onmessage received_msg=<', received_msg,'>');
  }
};


onWSReady = () => {
  let infoRead = 'serial:list,r\n';
  console.log('onWSReady infoRead=<', infoRead,'>');
  ws.send(infoRead);
}

readLegInfo = () => {
  let infoRead = 'info:r\n';
  console.log('onWSReady infoRead=<', infoRead,'>');
  ws.send(infoRead);
}


onLegInfo = (info) => {
  console.log('onLegInfo info=<', info,'>');
  if(info.ch >0) {
    let channelInfo = {};
    channelInfo.index = 0;
    channelInfo.id = info.id0;
    channelInfo.mb = info.mb0;
    channelInfo.mf = info.mf0;
    channelInfo.wp = info.wp0;
    //console.log('onLegInfo channelInfo=<', channelInfo,'>');
    if(typeof onVueUILegInfo === 'function') {
      onVueUILegInfo(channelInfo);
    }
  }
  if(info.ch >1) {
    let channelInfo = {};
    channelInfo.index = 1;
    channelInfo.id = info.id1;
    channelInfo.mb = info.mb1;
    channelInfo.mf = info.mf1;
    channelInfo.wp = info.wp1;
    //console.log('onLegInfo channelInfo=<', channelInfo,'>');
    if(typeof onVueUILegInfo === 'function') {
      onVueUILegInfo(channelInfo);
    }
  }
}

onSerialPort = (ports) => {
  console.log('onSerialPort ports=<', ports,'>');
  if(typeof onVueUILegInfo === 'function') {
    onVueUISerialPort(ports);
  }
}


onVolumeWheelVol = (vol,index) => {
  //console.log('onVolumeWheelVol vol=<', vol,'>');
  $('#vue-ui-current-position-label-' + index).text(vol);
  $('#vue-ui-current-position-range-' + index).val(vol);
}



$(document).ready(function(){
  $('#eMule-wheel-distance-slide-set').bind('change', function(){
    doSetWheelDistance(parseInt($('#eMule-wheel-distance-slide-set').val()));
  });
});

doSetWheelDistance = (value) => {
  console.log('doSetWheelDistance value=<', value,'>');
  $('#eMule-wheel-distance-text-set').text(value);
  let run = 'wheel:vol,'+ value + '\n';
  if(ws.readyState) {
    ws.send(run);
  }
}



function onUILoop3() {
  loopA();
  setTimeout(()=> {
    loopB();
  },1000)
  setTimeout(()=> {
    loopA();
  },2000)
  setTimeout(()=> {
    loopB();
  },3000)
  setTimeout(()=> {
    loopA();
  },4000)
  setTimeout(()=> {
    loopB();
  },5000)
  setTimeout(()=> {
    loopA();
  },6000)
}

function loopA() {
  let run1 = 'wheel:vol,560\n';
  if(ws.readyState) {
    ws.send(run1);
  }  
}

function loopB() {
  let run2 = 'wheel:vol,640\n';
  if(ws.readyState) {
    ws.send(run2);
  }  
}

function onUILinearUp() {
  let run2 = 'linear:ground,1:distance,1\n';
  if(ws.readyState) {
    ws.send(run2);
  }  
}

function onUILinearUp2s() {
  onUILinearUp()
  setTimeout(()=> {
    let run2 = 'linear:ground,1:distance,0\n';
    if(ws.readyState) {
      ws.send(run2);
    }      
  },2000);
}

function onUILinearDown() {
  let run2 = 'linear:ground,0:distance,1\n';
  if(ws.readyState) {
    ws.send(run2);
  }  
}

function onUILinearDown2s() {
  onUILinearDown();
  setTimeout(()=> {
    let run2 = 'linear:ground,0:distance,0\n';
    if(ws.readyState) {
      ws.send(run2);
    }  
  },2000);
}

function onUIActionGroud() {
  GotoWheelB();
}

function onUIActionAir() {
  GotoLinearB();
  setTimeout(()=>{
    GotoWheelA();
  },2000);
  setTimeout(()=>{
    GotoLinearA();
  },4000);
}


function GotoWheelA() {
  let run1 = 'wheel:vol,560\n';
  if(ws.readyState) {
    ws.send(run1);
  }
}

function GotoWheelB() {
  let run2 = 'wheel:vol,640\n';
  if(ws.readyState) {
    ws.send(run2);
  }
}

function GotoLinearA() {
  let run2 = 'linear:ground,0:distance,1\n';
  if(ws.readyState) {
    ws.send(run2);
  }
  setTimeout(()=> {
    let run2 = 'linear:ground,0:distance,0\n';
    if(ws.readyState) {
      ws.send(run2);
    }
  },1000);
}

function GotoLinearB() {
  let run2 = 'linear:ground,1:distance,1\n';
  if(ws.readyState) {
    ws.send(run2);
  }
  setTimeout(()=> {
    let run2 = 'linear:ground,1:distance,0\n';
    if(ws.readyState) {
      ws.send(run2);
    }
  },1000);
}


function onUIClickSerialConnect(elem) {
  //console.log('onUIClickSerialConnect elem=<', elem,'>');
  const portName = elem.textContent.replace('Connect To ','').trim();
  console.log('onUIClickSerialConnect portName=<', portName,'>');
  ws.send('serial:open,' + portName);
}


function getInputUITool(elem) {
  let inputElem = elem.parentElement.parentElement.getElementsByTagName('input')[0];
  console.log('getInputUITool inputElem=<', inputElem,'>');
  return parseInt(inputElem.value.trim());
}

function getChannelUITool(elem) {
  let channelElem = elem.parentElement.parentElement.parentElement.parentElement.parentElement.getElementsByTagName('h5')[0];
  console.log('getChannelUITool channelElem=<', channelElem,'>');
  return parseInt(channelElem.textContent.replace('Channel of ','').trim());
}

function onUIChangeLegID(elem) {
  console.log('onUIChangeLegID elem=<', elem,'>');
  let legId = getInputUITool(elem);
  console.log('onUIChangeLegID legId=<', legId,'>');
  let channel = getChannelUITool(elem);
  console.log('onUIChangeLegID channel=<', channel,'>');
  if(!isNaN(channel) && !isNaN(legId)) {
    let legIdMsg = 'setting:id' + channel + ',' + legId + '\n';
    console.log('onUIChangeLegID legIdMsg=<', legIdMsg,'>');
    ws.send(legIdMsg);
  }
}

function onUIChangeMaxFront(elem) {
  console.log('onUIChangeMaxFront elem=<', elem,'>');
  let limit = getInputUITool(elem);
  console.log('onUIChangeMaxFront limit=<', limit,'>');
  let channel = getChannelUITool(elem);
  console.log('onUIChangeMaxFront channel=<', channel,'>');
  if(!isNaN(channel) && !isNaN(limit)) {
    let mf = 'setting:mf' + channel + ',' + limit + '\n';
    console.log('onUIChangeMaxFront mf=<', mf,'>');
    ws.send(mf);
  }
}

function onUIChangeMaxBack(elem) {
  console.log('onUIChangeMaxBack elem=<', elem,'>');
  let limit = getInputUITool(elem);
  console.log('onUIChangeMaxBack limit=<', limit,'>');
  let channel = getChannelUITool(elem);
  console.log('onUIChangeMaxBack channel=<', channel,'>');
  if(!isNaN(channel) && !isNaN(limit)) {
    let mb = 'setting:mb' + channel + ',' + limit + '\n';
    console.log('onUIChangeMaxBack mb=<', mb,'>');
    ws.send(mb);
  }
}

function onUIChangeTargetPosition(elem) {
  console.log('onUIChangeTargetPosition elem=<', elem,'>');
  let targetPostion = parseInt(elem.value);
  console.log('onUIChangeTargetPosition targetPostion=<', targetPostion,'>');
  let channel = getChannelUITool(elem);
  console.log('onUIChangeTargetPosition channel=<', channel,'>');
  if(!isNaN(channel) && !isNaN(targetPostion)) {
    $('#vue-ui-target-position-label-' + channel).text(targetPostion);
    let target = 'wheel:vol' + channel + ',' + targetPostion + '\n';
    console.log('onUIChangeTargetPosition target=<', target,'>');
    ws.send(target);
  }
}

function onUIChangeCWFlag(elem,flag) {
  console.log('onUIChangeCWFlag elem=<', elem,'>');
  console.log('onUIChangeCWFlag flag=<', flag,'>');
  let channel = getChannelUITool(elem);
  console.log('onUIChangeCWFlag channel=<', channel,'>');
  if(!isNaN(channel)) {
    let cw = 'setting:cw' + channel + ',' + flag + '\n';
    console.log('onUIChangeCWFlag cw=<', cw,'>');
    ws.send(cw);
  }
}
