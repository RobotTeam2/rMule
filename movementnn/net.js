const util = require('util');
const graphviz = require('graphviz');
const fs=require("fs");
const netJson = require('./eMule.json');
const NetJson2Dot = require('./net2dot.js');
let netOfDot = new NetJson2Dot(netJson);
netOfDot.write();

