
let gTempChannelInfo = false;

Vue.component('ui-motor-channel', {
  data: function () {
    return {
      channel: gTempChannelInfo
    }
  },
  template: `
          <div class="card">
            <div class="card-body">
              <h5 class="card-title">Channel of {{ channel.index }} </h5>
              <div class="row justify-content-center">
                <div class="col">
                  <div class="input-group mb-3">
                    <div class="input-group-prepend">
                      <span class="input-group-text">{{ channel.id }}</span>
                      <button type="button" class="btn btn-primary btn-sm" onclick="onUIChangeLegID(this)">
                        <i class="fas fa-stamp"></i>
                        <i class="fas fa-check-circle"></i>
                      </button>
                    </div>
                    <input type="text" class="form-control" placeholder="leg id">
                  </div>
                </div>
                <div class="col">
                  <div class="input-group mb-3">
                    <div class="input-group-prepend">
                      <span class="input-group-text">{{ channel.mf }}</span>
                      <button type="button" class="btn btn-primary btn-sm" onclick="onUIChangeMaxFront(this)">
                        <i class="fas fa-hand-point-right"></i>
                        <i class="fas fa-grip-lines-vertical"></i>
                        <i class="fas fa-check-circle"></i>
                      </button>
                    </div>
                    <input type="text" class="form-control" placeholder="Wheel Max Front">
                  </div>
                </div>
                <div class="col">
                  <div class="input-group mb-3">
                    <div class="input-group-prepend">
                      <span class="input-group-text">{{ channel.mb }}</span>
                      <button type="button" class="btn btn-primary btn-sm" onclick="onUIChangeMaxBack(this)">
                        <i class="fas fa-grip-lines-vertical"></i>
                        <i class="fas fa-hand-point-left"></i>
                        <i class="fas fa-check-circle"></i>
                      </button>
                    </div>
                    <input type="text" class="form-control" placeholder="Wheel Max Back">
                  </div>
                </div>
              </div>
            </div>
          </div>
  `
})



onVueUILegInfo = (channelInfo) => {
  console.log('onVueUILegInfo channelInfo=<', channelInfo,'>');
  gTempChannelInfo = channelInfo;
  if(channelInfo.index === 0) {
    new Vue({ el: '#ui-vue-motor-channel-A'});
  }
  if(channelInfo.index === 1) {
    new Vue({ el: '#ui-vue-motor-channel-B'});
  }
}

onVueUISerialPort = (ports) => {
  console.log('onVueUISerialPort ports=<', ports,'>');
  new Vue({ el: '#ui-vue-serial-port-select' ,data: {ports:ports}});
}



