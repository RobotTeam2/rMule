const cv = require('opencv4nodejs');
//console.log(':: cv=<',cv,'>');

module.exports = class ImageLoaderCV {
  constructor(img,net) {
    this.net_ = net;
    this.inputLayer_ = this.net_.netJson_.layers.input;
    const matImg = cv.imread(img);
    //console.log('ImageLoaderCV::constructor matImg=<',matImg,'>');
    this.grayMat_ = matImg.bgrToGray();
    //console.log('ImageLoaderCV::constructor this.grayMat_=<',this.grayMat_,'>');
    //cv.imwrite('./img_gray.png', this.grayMat_);
  }
  crash() {
    //console.log('ImageLoaderCV::crash this.net_=<',this.net_,'>');
    //console.log('ImageLoaderCV::crash this.inputLayer_=<',this.inputLayer_,'>');
    const crashX = this.inputLayer_.x;
    const crashY = this.inputLayer_.y;
    console.log('ImageLoaderCV::crash crashX=<',crashX,'>');
    console.log('ImageLoaderCV::crash crashY=<',crashY,'>');    
    console.log('ImageLoaderCV::crash this.grayMat_=<',this.grayMat_,'>');
    const loopX = Math.floor(this.grayMat_.cols/crashX);
    const loopY = Math.floor(this.grayMat_.rows/crashY);
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
            const grayPixel = this.grayMat_.at(yGlobal,xGlobal);
            const offset = crashX * jLocal + iLocal;
            localPixs[offset] = grayPixel;
          }
        }
        blockPixs.push(localPixs);
      }
    }
    //this.dumpClips_(blockPixs);
    return blockPixs;
  }
  
  dumpClips_(blockPixs) {
    let counter = 0;
    cv.imwrite('./dumpout/img_grid-' + counter++ + '.png', this.grayMat_);
    for(let block of blockPixs) {
      console.log('ImageLoaderCV::dumpClips_ block=<',block,'>');
      const whiteMat = new cv.Mat(Buffer.from(block),this.inputLayer_.y, this.inputLayer_.x, cv.CV_8UC1);
      cv.imwrite('./dumpout/img_grid-' + counter++ + '.png', whiteMat);
    }
  }
};
