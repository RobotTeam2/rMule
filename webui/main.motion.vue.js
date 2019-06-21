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
      if(jsonMsg.info) {
        onLegInfo(jsonMsg.info);
      } else {
        console.log('onmessage jsonMsg=<', jsonMsg,'>');
      }
    }
  } catch (e) {
    console.error('onmessage e=<', e,'>');
    console.error('onmessage received_msg=<', received_msg,'>');
  }
};


onWSReady = () => {
  readLegInfo();
}

readLegInfo = () => {
  let infoRead = {info:'request'};
  //console.log('onWSReady infoRead=<', infoRead,'>');
  ws.send(JSON.stringify(infoRead));
}


onLegInfo = (info) => {
  //console.log('onLegInfo info=<', info,'>');
  if(typeof onVueUILegInfoAdvance === 'function') {
    onVueUILegInfoAdvance(info);
  }
}


onVolumeWheelVol = (vol,index) => {
  //console.log('onVolumeWheelVol vol=<', vol,'>');
  $('#vue-ui-current-position-label-' + index).text(vol);
  $('#vue-ui-current-position-range-' + index).val(vol);
}



function onUIMoveForward(elem) {
  console.log('onUIMoveForward elem=<', elem,'>');
  let move = {
      moveTo:[
        {leg:'11',pos:150},
        {leg:'21',pos:150},
        {leg:'12',pos:150},
        {leg:'22',pos:150},
        {leg:'13',pos:150},
        {leg:'23',pos:150}
    ]};
  console.log('onUIMoveForward move=<', move,'>');
  ws.send(JSON.stringify(move));
}

function onUIMoveBackward(elem) {
  console.log('onUIMoveBackward elem=<', elem,'>');
  let move = {
      moveTo:[
        {leg:'11',pos:0},
        {leg:'21',pos:0},
        {leg:'12',pos:0},
        {leg:'22',pos:0},
        {leg:'13',pos:0},
        {leg:'23',pos:0}
    ]};
  console.log('onUIMoveBackward move=<', move,'>');
  ws.send(JSON.stringify(move));
}



