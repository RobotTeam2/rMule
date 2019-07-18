const util = require('util');
const graphviz = require('graphviz');
const fs=require("fs");


module.exports = class NetJson2Dot {
  constructor(netJson) {
    this.netGraph_ = graphviz.graph(netJson.name);
    this.netJson_ = netJson;
    this.createLayer_();
    this.joinLayer_();
  }
  write() {
    const netDot = this.netGraph_.to_dot();
    //console.log('::netDot=<',netDot,'>');
    fs.writeFileSync('./dotnetwork_' + this.netJson_.name+ '.dot',netDot);
    this.netGraph_.setGraphVizPath( 'D:/program/graphbiz/bin/' );
    this.netGraph_.output( 'png', './dotnetwork_' + this.netJson_.name+ '.png' );
  }
  
  
  
  createLayer_() {
    for(let layerKey in this.netJson_.layers) {
      //console.log('::layerKey=<',layerKey,'>');
      const layerCluster = this.netGraph_.addCluster( layerKey);
      this.addLayerNodes_(layerKey,layerCluster);
    }    
  }
  joinLayer_() {
    for(let layerKey in this.netJson_.layers) {
      //console.log('::layerKey=<',layerKey,'>');
      this.connectLayerNodes_(layerKey);
    }    
  }
  
  addLayerNodes_(layerkey,graph){
    const nodeAttr = {
        label:'',
        color:'darkgreen',
        shape :'"circle"'
    };
    if(layerkey === 'input') {
      nodeAttr.color = 'blue';
    }
    if(layerkey === 'output') {
      nodeAttr.color = 'red';
    }
    let layerJson = this.netJson_.layers[layerkey];
    for(let i = 0;i < layerJson.width ;i++) {
      if(layerJson.label) {
        nodeAttr.label = layerJson.label + i;
      }
      graph.addNode( layerkey + '_' + i,nodeAttr);
    }
  }

  connectLayerNodes_(layerKey) {
    //console.log('connectLayer::layerKey=<',layerKey,'>');
    let layer = this.netJson_.layers[layerKey];
    //console.log('connectLayer::layer=<',layer,'>');
    if(layer.left) {
      //console.log('connectLayer::layer.left=<',layer.left,'>');
      let layerLeft = this.netJson_.layers[layer.left];
      //console.log('connectLayer::layerLeft=<',layerLeft,'>');
      let clusterMine = this.netGraph_.getCluster( layerKey);
      let clusterLeft = this.netGraph_.getCluster( layer.left);
      //console.log('connectLayer::clusterMine=<',clusterMine,'>');
      //console.log('connectLayer::clusterLeft=<',clusterLeft,'>');
      for(let nodeIdLeft in clusterLeft.nodes.items) {
        //console.log('connectLayer::nodeIdLeft=<',nodeIdLeft,'>');
        let nodeLeft = clusterLeft.getNode(nodeIdLeft);
        //console.log('connectLayer::nodeLeft=<',nodeLeft,'>');
        for(let nodeIdMine in clusterMine.nodes.items) {
          //console.log('connectLayer::nodeIdMine=<',nodeIdMine,'>');
          let nodeMine = clusterMine.getNode(nodeIdMine);
          //console.log('connectLayer::nodeMine=<',nodeMine,'>');
          this.netGraph_.addEdge(nodeLeft,nodeMine);
        }
      }
    }
  }

};

