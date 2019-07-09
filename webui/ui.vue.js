
let gTempChannelInfo = false;

Vue.component('ui-motor-channel', {
  data: function () {
    return {
      channel: gTempChannelInfo
    }
  },
  template: `
          <div class="card text-white bg-dark">
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
                      <span class="input-group-text">{{ channel.pwmoffset }}</span>
                      <button type="button" class="btn btn-primary btn-sm" onclick="onUIChangePWMOffset(this)">
                        <i class="fas fa-wave-square"></i>
                        <i class="fas fa-check-circle"></i>
                      </button>
                    </div>
                    <input type="text" class="form-control" placeholder="pwm offset">
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

              <div class="row justify-content-start mt-2">
                <div class="col-3">
                  <button type="button" class="btn btn-primary" onclick="onUIChangeCWFlag(this,1)">
                    Set CW/CCW Flag 1
                    <i class="fas fa-check-circle"></i>
                  </button>
                </div>
                <div class="col-3">
                  <button type="button" class="btn btn-success" onclick="onUIChangeCWFlag(this,0)">
                    Set CW/CCW Flag 0
                    <i class="fas fa-check-circle"></i>
                  </button>
                </div>
              </div>
              
              <div class="row justify-content-center">
                <div class="col">
                  <div class="form-group text-center">
                    <label>Curent Distance <span class="badge badge-primary" v-bind:id="channel.elemIDCurrentPositionLabel">{{ channel.wp }}</span>unit</label>
                    <input type="range" disabled class="form-control-range" v-bind:id="channel.elemIDCurrentPositionRange" v-bind:value="channel.wp" v-bind:min="channel.mf" v-bind:max="channel.mb">
                  </div>
                </div>
              </div>
              
              <div class="row justify-content-center">
                <div class="col text-center">
                    <label>Target Distance <span class="badge badge-primary" v-bind:id="channel.elemIDTargetPositionLabel">{{ channel.mAva }}</span>unit</label>
                  <div class="input-group ">
                    <div class="input-group-prepend">
                      <span class="input-group-text">{{ channel.mf }}</span>
                    </div>
                    <input type="range" class="form-control form-control-range" v-bind:id="channel.elemIDTargetPositionRange" v-bind:value="channel.mAva" v-bind:min="channel.mf" v-bind:max="channel.mb" onchange="onUIChangeTargetPosition(this)">
                    <div class="input-group-append">
                      <span class="input-group-text">{{ channel.mb }}</span>
                    </div>
                  </div>
                </div>
              </div>
              
            </div>
          </div>
  `
})



onVueUILegInfo = (channelInfo) => {
  console.log('onVueUILegInfo channelInfo=<', channelInfo,'>');
  channelInfo.elemIDCurrentPositionLabel = 'vue-ui-current-position-label-' + channelInfo.index;
  channelInfo.elemIDCurrentPositionRange = 'vue-ui-current-position-range-' + channelInfo.index;
  channelInfo.elemIDTargetPositionLabel = 'vue-ui-target-position-label-' + channelInfo.index;
  channelInfo.elemIDTargetPositionRange = 'vue-ui-target-position-range-' + channelInfo.index;
  channelInfo.mf = parseInt(channelInfo.mf);
  channelInfo.mb = parseInt(channelInfo.mb);
  channelInfo.wp = parseInt(channelInfo.wp);
  channelInfo.mAva = (channelInfo.mf + channelInfo.mb)/2;
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






