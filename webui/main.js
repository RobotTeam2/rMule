let ws = new WebSocket('ws://127.0.0.1:18081');

ws.onopen = (evt) => {
  setTimeout(onWSReady,1);
}

ws.onmessage = (evt) => { 
  let received_msg = evt.data;
  //console.log('onmessage received_msg=<', received_msg,'>');
  try {
    let jsonMsg = JSON.parse(received_msg);
    //console.log('onmessage jsonMsg=<', jsonMsg,'>');
    if(jsonMsg) {
      if(jsonMsg.tofw) {
        onTofWheel(jsonMsg.tofw);
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
  let infoRead = { info: {}};
  console.log('onWSReady infoRead=<', infoRead,'>');
  ws.send(JSON.stringify(infoRead));
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
 
doSetWheelDistance = (value) => {
  console.log('doSetWheelDistance value=<', value,'>');
  $('#eMule-wheel-distance-text-set').text(value);
  let run = { tof: {wheel:value}};
  if(ws.readyState) {
    ws.send(JSON.stringify(run));
  }
}
