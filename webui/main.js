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
      if(jsonMsg.info) {
        onLegInfo(jsonMsg.info);
      }
      if(jsonMsg.vol) {
        onVolumeWheel(jsonMsg.vol);
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
  let infoRead = 'info:r\n';
  console.log('onWSReady infoRead=<', infoRead,'>');
  ws.send(infoRead);
}

onLegInfo = (info) => {
  console.log('onLegInfo info=<', info,'>');
  $('#eMule-info-leg-id').text(info.id);
  $('#eMule-info-wheel-max-back').text(info.mb);
  $('#eMule-info-wheel-max-front').text(info.mf);
  
  $('#eMule-wheel-distance-text-show').text(info.wp);
  
  $('#eMule-wheel-distance-slide-show').attr('value',info.wp);
  $('#eMule-wheel-distance-slide-show').attr('min',info.mb);
  $('#eMule-wheel-distance-slide-show').attr('max',info.mf);

  $('#eMule-wheel-distance-slide-set').attr('min',info.mb);
  $('#eMule-wheel-distance-slide-set').attr('max',info.mf);
  
  console.log('onLegInfo eMule-wheel-distance-slide-show=<', $('#eMule-wheel-distance-slide-show'),'>');
  
}


onVolumeWheel = (tofDistance) => {
  //console.log('onVolumeWheel tofDistance=<', tofDistance,'>');
  $('#eMule-wheel-distance-text-show').text(tofDistance);
  $('#eMule-wheel-distance-slide-show').val(tofDistance);
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
  let run1 = 'wheel:vol,380\n';
  if(ws.readyState) {
    ws.send(run1);
  }  
}

function loopB() {
  let run2 = 'wheel:vol,320\n';
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
  let run1 = 'wheel:vol,385\n';
  if(ws.readyState) {
    ws.send(run1);
  }
}

function GotoWheelB() {
  let run2 = 'wheel:vol,315\n';
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


function onUIChangeLegID(elem) {
  console.log('onUIChangeLegID elem=<', elem,'>');
  console.log('onUIChangeLegID elem.value=<', elem.value,'>');
  let legId = 'setting:id,' + elem.value.trim() + '\n';
  console.log('onUIChangeLegID legId=<', legId,'>');
  ws.send(legId);
}

function onUIChangeWheelMaxFront(elem) {
  console.log('onUIChangeWheelMaxFront elem=<', elem,'>');
  console.log('onUIChangeWheelMaxFront elem.value=<', elem.value,'>');
  let mf = 'setting:mf,' + elem.value.trim() + '\n';
  console.log('onUIChangeWheelMaxFront mf=<', mf,'>');
  ws.send(mf);
}

function onUIChangeWheelMaxBack(elem) {
  console.log('onUIChangeWheelMaxBack elem=<', elem,'>');
  console.log('onUIChangeWheelMaxBack elem.value=<', elem.value,'>');
  let mb = 'setting:mb,' + elem.value.trim() + '\n';
  console.log('onUIChangeWheelMaxBack mb=<', mb,'>');
  ws.send(mb);
}

