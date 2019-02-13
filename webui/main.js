let ws = new WebSocket('ws://127.0.0.1:18081');

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
    }
  } catch (e) {
    console.error('onmessage received_msg=<', received_msg,'>');
  }
};

onTofWheel = (tofDistance) => {
  console.log('onTofWheel tofDistance=<', tofDistance,'>');
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
  let run = { tofw: value};
  if(ws.readyState) {
    ws.send(JSON.stringify(run));
  }
}
