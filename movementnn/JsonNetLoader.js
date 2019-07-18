
module.exports = class JsonNetLoader {
  constructor(netJson) {
    this.netJson_ = netJson;
    this.layerStats_ = {};
    this.layerVertex_ = {};
    this.layerWeight_ = {};
    this.layerBias_ = {};
    this.createLayer_();
    this.joinLayer_();
  }
    
  createLayer_() {
    for(let layerKey in this.netJson_.layers) {
      //console.log('::layerKey=<',layerKey,'>');
      this.addLayerNodes_(layerKey);
    }    
  }
  joinLayer_() {
    for(let layerKey in this.netJson_.layers) {
      //console.log('::layerKey=<',layerKey,'>');
      this.connectLayerNodes_(layerKey);
    }    
  }
  
  addLayerNodes_(layerKey){
    let layerJson = this.netJson_.layers[layerKey];
    this.layerStats_[layerKey] = new Array(layerJson.width);
    /*
    for(let i = 0;i < layerJson.width ;i++) {
    }
    */
  }

  connectLayerNodes_(layerKey) {
    //console.log('connectLayer::layerKey=<',layerKey,'>');
    let layer = this.netJson_.layers[layerKey];
    //console.log('connectLayer::layer=<',layer,'>');
    if(layer.left) {
      //console.log('connectLayer::layer.left=<',layer.left,'>');
      let layerLeft = this.netJson_.layers[layer.left];
      //console.log('connectLayer::layerLeft=<',layerLeft,'>');
      let wMatrix = [];
      let bMatrix = [];
      let vMatrix = [];
      for(let i = 0;i < layer.width;i++) {
        let wVector = [];
        let bVector = [];
        let vVector = [];
        for(let j = 0;j < layerLeft.width;j++) {
          wVector.push(0.1);
          bVector.push(0.000001);
          vVector.push(Math.random());
        }
        wMatrix.push(wVector);
        bMatrix.push(bVector);
        vMatrix.push(vVector);
      }
      this.layerWeight_[layerKey] = wMatrix;
      this.layerBias_[layerKey] = bMatrix;
      this.layerVertex_[layerKey] = vMatrix;
    }
  }

};
