let gTempLegInfo = false;
Vue.component('ui-motion-leg', {
  data: function () {
    return {
      leg: gTempLegInfo
    }
  },
  template: `
          <div class="card text-white bg-dark">
            <div class="card-body">
              <h5 class="card-title">Channel of {{ leg.leg }} </h5>
          
              
              <div class="row justify-content-center">
                <div class="col">
                  <div class="form-group text-center">
                    <label>Curent Distance <span class="badge badge-primary" v-bind:id="leg.elemIDCurrentPositionLabel">{{ leg.wp }}</span>unit</label>
                    <input type="range" disabled class="form-control-range" v-bind:id="leg.elemIDCurrentPositionRange" v-bind:value="leg.wp" v-bind:min="leg.mf" v-bind:max="leg.mb">
                  </div>
                </div>
              </div>
              
            </div>
          </div>
  `
})


onVueUILegInfoAdvance = (leginfo) => {
  console.log('onVueUILegInfoAdvance leginfo=<', leginfo,'>');
  let elID = '#ui-vue-leg-' + leginfo.leg;
  gTempLegInfo = leginfo;
  gTempLegInfo.elemIDCurrentPositionLabel = 'vue-ui-current-position-label-' + leginfo.leg;
  gTempLegInfo.elemIDCurrentPositionRange = 'vue-ui-current-position-range-' + leginfo.leg;
  new Vue({ el: elID});
}


