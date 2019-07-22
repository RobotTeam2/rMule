const cv = require('opencv4nodejs');
//console.log(':: cv=<',cv,'>');

module.exports = class ImageLoaderCV {
  constructor(img,net) {
    this.net_ = net;
    this.inputLayer_ = this.net_.netJson_.layers.input;
    const matImg = cv.imread(img);
    //console.log('ImageLoaderCV::constructor matImg=<',matImg,'>');
    this.grayMat_ = matImg.bgrToGray();
    const [matB, matG, matR] = matImg.splitChannels();
    this.bMat_ = matB;
    this.gMat_ = matG;
    this.rMat_ = matR;
    //console.log('ImageLoaderCV::constructor this.grayMat_=<',this.grayMat_,'>');
    //cv.imwrite('./img_gray.png', this.grayMat_);
  }
  crashGray() {
    return this.crashChannel_(this.grayMat_);
  }
  crashBlue() {
    return this.crashChannel_(this.bMat_);
  }
  crashGreen() {
    return this.crashChannel_(this.gMat_);
  }
  crashRed() {
    return this.crashChannel_(this.rMat_);
  }
  pixelGray(){
    //console.log('ImageLoaderCV::crash this.grayMat_=<',this.grayMat_,'>');
    const pixels = [];
    for(let x = 0; x < this.grayMat_.cols;x++) {
      for(let y = 0; y < this.grayMat_.rows;y++) {
        pixels.push(this.grayMat_.at(y,x));
      }
    }
    return pixels;
  }
  
  dumpClips_(blockPixs,channelMat) {
    let counter = 0;
    cv.imwrite('./dumpout/img_channel-' + counter++ + '.png', channelMat);
    for(let block of blockPixs) {
      console.log('ImageLoaderCV::dumpClips_ block=<',block,'>');
      const whiteMat = new cv.Mat(Buffer.from(block),this.inputLayer_.y, this.inputLayer_.x, cv.CV_8UC1);
      cv.imwrite('./dumpout/img_grid-' + counter++ + '.png', whiteMat);
    }
  }
  crashChannel_(channelMat) {
    //console.log('ImageLoaderCV::crash this.net_=<',this.net_,'>');
    //console.log('ImageLoaderCV::crash this.inputLayer_=<',this.inputLayer_,'>');
    const crashX = this.inputLayer_.x;
    const crashY = this.inputLayer_.y;
    console.log('ImageLoaderCV::crash crashX=<',crashX,'>');
    console.log('ImageLoaderCV::crash crashY=<',crashY,'>');    
    console.log('ImageLoaderCV::crash channelMat=<',channelMat,'>');
    const loopX = Math.floor(channelMat.cols/crashX);
    const loopY = Math.floor(channelMat.rows/crashY);
    console.log('ImageLoaderCV::crash loopX=<',loopX,'>');
    console.log('ImageLoaderCV::crash loopY=<',loopY,'>');
    const blockPixs = [];
    for(let iBlock = 0;iBlock < loopX;iBlock++) {
      for(let jBlock = 0;jBlock < loopY;jBlock++) {
        const localPixs = new Array(crashX*crashY);
        for(let iLocal = 0;iLocal < crashX;iLocal++) {
          for(let jLocal = 0;jLocal < crashY;jLocal++) {
            const xGlobal = iBlock * crashX + iLocal;
            const yGlobal = jBlock * crashY + jLocal;
            const grayPixel = channelMat.at(yGlobal,xGlobal);
            const offset = crashX * jLocal + iLocal;
            localPixs[offset] = grayPixel;
          }
        }
        blockPixs.push(localPixs);
      }
    }
    //this.dumpClips_(blockPixs,channelMat);
    return blockPixs;
  }
  
  
};
