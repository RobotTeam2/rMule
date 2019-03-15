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
      if(jsonMsg.tofw) {
        onTofWheel(jsonMsg.tofw);
      }
      if(jsonMsg.volume) {
        onTofWheel(jsonMsg.volume);
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
  $('#eMule-info-wheel-limit-back').text(info.lb);
  $('#eMule-info-wheel-limit-front').text(info.lf);
}


onTofWheel = (tofDistance) => {
  //console.log('onTofWheel tofDistance=<', tofDistance,'>');
  $('#eMule-wheel-distance-text-show').text(tofDistance);
  $('#eMule-wheel-distance-slide-show').val(tofDistance);
}

onInfoLeg = (leg) => {
  console.log('onInfoLeg leg=<', leg,'>');
}

onInfoEROMWheelLimitBack = (limit) => {
  console.log('onInfoEROMWheelLimitBack limit=<', limit,'>');
}

onInfoEROMWheelLimitFront = (limit) => {
  console.log('onInfoEROMWheelLimitFront limit=<', limit,'>');
}


$(document).ready(function(){
  $('#eMule-wheel-distance-slide-set').bind('change', function(){
    doSetWheelDistance(parseInt($('#eMule-wheel-distance-slide-set').val()));
  });
});

/* 
doSetWheelDistance = (value) => {
  console.log('doSetWheelDistance value=<', value,'>');
  $('#eMule-wheel-distance-text-set').text(value);
  let run = { tof: {wheel:value}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run));
  }
}
*/

doSetWheelDistance = (value) => {
  console.log('doSetWheelDistance value=<', value,'>');
  $('#eMule-wheel-distance-text-set').text(value);
  let run = { vol: {wheel:value}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run));
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
  let run1 = { vol: {wheel:380}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run1));
  }  
}

function loopB() {
  let run2 = { vol: {wheel:320}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run2));
  }  
}

function onUILinearUp() {
  let run2 = { linear: {g:1,d:1}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run2));
  }  
}

function onUILinearUp2s() {
  onUILinearUp()
  setTimeout(()=> {
    let run2 = { linear: {g:1,d:0}};
    if(ws.readyState) {
      ws.send(JSON.stringify(run2));
    }      
  },2000);
}

function onUILinearDown() {
  let run2 = { linear: {g:0,d:1}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run2));
  }  
}

function onUILinearDown2s() {
  onUILinearDown();
  setTimeout(()=> {
    let run2 = { linear: {g:0,d:0}};
    if(ws.readyState) {
      ws.send(JSON.stringify(run2));
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
  let run1 = { vol: {wheel:385}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run1));
  }  
}

function GotoWheelB() {
  let run2 = { vol: {wheel:315}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run2));
  }  
}

function GotoLinearA() {
  let run2 = { linear: {g:0,d:1}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run2));
  }  
  setTimeout(()=> {
    let run2 = { linear: {g:0,d:0}};
    if(ws.readyState) {
      ws.send(JSON.stringify(run2));
    }  
  },1000);
}

function GotoLinearB() {
  let run2 = { linear: {g:1,d:1}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run2));
  }  
  setTimeout(()=> {
    let run2 = { linear: {g:1,d:0}};
    if(ws.readyState) {
      ws.send(JSON.stringify(run2));
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

function onUIChangeWheelLimitFront(elem) {
  console.log('onUIChangeWheelLimitFront elem=<', elem,'>');
  console.log('onUIChangeWheelLimitFront elem.value=<', elem.value,'>');
  let lf = 'setting:lf,' + elem.value.trim() + '\n';
  console.log('onUIChangeWheelLimitFront lf=<', lf,'>');
  ws.send(lf);
}

function onUIChangeWheelLimitBack(elem) {
  console.log('onUIChangeWheelLimitBack elem=<', elem,'>');
  console.log('onUIChangeWheelLimitBack elem.value=<', elem.value,'>');
  let lb = 'setting:lb,' + elem.value.trim() + '\n';
  console.log('onUIChangeWheelLimitBack lb=<', lb,'>');
  ws.send(lb);
}

