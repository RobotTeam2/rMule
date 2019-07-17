const util = require('util');
const graphviz = require('graphviz');
const fs=require("fs");

const netJson = require('./eMule.json');
console.log('::netJson=<',netJson,'>');

console.log('::graphviz=<',graphviz,'>');
console.log('::graphviz.digraph=<',graphviz.digraph,'>');


const netGraph = graphviz.digraph(netJson.name);


const addLayerToDot = (layerkey,graph) => {
  for(let i = 0;i < netJson.layers[layerkey].width ;i++) {
    let node = graph.addNode( layerkey + '_' + i,{
      label:'',
      shape :'"circle"'
    });
  }
};


for(let layerKey in netJson.layers) {
  console.log('::layerKey=<',layerKey,'>');
  const layerCluster = netGraph.addCluster( layerKey);
  addLayerToDot(layerKey,layerCluster);
}

const connectLayer = (layerKey) => {
  console.log('connectLayer ::layerKey=<',layerKey,'>');
  let layer = netJson.layers[layerKey];
  console.log('connectLayer ::layer=<',layer,'>');
}

for(let layerKey in netJson.layers) {
  console.log('::layerKey=<',layerKey,'>');
  connectLayer(layerKey);
}

/*
const input = netGraph.addCluster( 'input');
const allLayers = {};
allLayers.input = netJson.input;
allLayers.output = netJson.output;
const allCluster = {};
allCluster['input'] = input;

const allNode = {};

const addLayToDot = (layerMine,layerkey,graph,layerLeft,layerLeftKey) => {
  for(let i = 0;i < layerMine.width ;i++) {
    let node = graph.addNode( layerkey + '_' + i,{
      label:'',
      shape :'"circle"'
    });
    if(layerLeft) {
      for(let j = 0;j < layerLeft.width ;j++) {
        let nodeLeftKey = layerLeftKey + '_' + j;
        let nodeLeft = graphviz.getNode(nodeLeftKey);
        console.log('addLayToDot::nodeLeft=<',nodeLeft,'>');
      }
    }
  }
};
addLayToDot(netJson.input,'input',input);

for(let layerHiddenKey in netJson.hidden) {
  console.log('::layerHiddenKey=<',layerHiddenKey,'>');
  const layerHidden = netJson.hidden[layerHiddenKey];
  console.log('::layerHidden=<',layerHidden,'>');
  const leftLayerKey = layerHidden.left;
  console.log('::leftLayerKey=<',leftLayerKey,'>');
  const layerCluster = netGraph.addCluster( layerHiddenKey);
  allCluster[leftLayerKey] = layerCluster;
  addLayToDot(layerHiddenKey,leftLayerKey);
}



const output = netGraph.addCluster( 'output');
allCluster['output'] = output;

const outputLeftKey = output.left;
const outputLeftLayer = netJson.hidden[outputLeftKey];
if(outputLeftLayer) {
  addLayToDot(netJson.output,'output',output,outputLeftLayer);
}
*/


const netDot = netGraph.to_dot();
console.log('::netDot=<',netDot,'>');
fs.writeFileSync('./network.dot',netDot);



//netJson.input
//for(let i = 0; i < netJson.)