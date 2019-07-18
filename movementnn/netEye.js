const util = require('util');
const graphviz = require('graphviz');
const fs=require("fs");
const netJson = require('./dEye.json');
const NetJson2Dot = require('./net2dot.js');
let netOfDot = new NetJson2Dot(netJson);
netOfDot.write();

const JsonNetLoader = require('./JsonNetLoader.js');
const net = new JsonNetLoader(netJson);
//console.log('::net=<',JSON.stringify(net,undefined,true),'>');

const ImageLoaderCV = require('./ImageLoaderCV.js');
const img = new ImageLoaderCV('./input/IMG_6043.JPG',net);
const inputVectors = img.crashRed();
//console.log('::inputVectors=<',inputVectors,'>');
const PreTrain = require('./PreTrain.js');
const pre = new PreTrain(net);
pre.step(inputVectors);

