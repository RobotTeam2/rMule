const fs=require("fs");
const fft = require('fft-js').fft;
const fftUtil = require('fft-js').util;
const ifft = require('fft-js').ifft;

module.exports = class PreTrain {
  constructor(net) {
    this.net_ = net;
    this.inputLayer_ = this.net_.netJson_.layers.input;
  }
  step(inputVectors) {
    //console.log('PreTrain::step this.net_=<',this.net_,'>');
    console.log('PreTrain::step this.inputLayer_=<',this.inputLayer_,'>');
    let inputStats = this.net_.layerStats_.input;
    console.log('PreTrain::step inputStats=<',inputStats,'>');
    console.log('PreTrain::step inputVectors.length=<',inputVectors.length,'>');
    for(let imgInput of inputVectors) {
      //console.log('PreTrain::step imgInput=<',imgInput,'>');
      for(let i = 0;i < imgInput.length;i++) {
        let pixel = imgInput[i];
        //console.log('PreTrain::step i=<',i,'>');
        //console.log('PreTrain::step pixel=<',pixel,'>');
        if(inputStats[i]) {
          if(inputStats[i][pixel]) {
            inputStats[i][pixel]++;
          } else {
            inputStats[i][pixel] = 1;
          }
        } else {
          inputStats[i] = new Array(255);
          inputStats[i].fill(0);
        }
        /*
        if(typeof inputStats[i][pixel] === 'undefined') {
          inputStats[i][pixel] = 1;
        } else {
          inputStats[i][pixel]++;
        }
        */
      } 
    }
    console.log('PreTrain::step this.net_=<',JSON.stringify(this.net_,undefined,'  '),'>');
    fs.writeFileSync('./modelnetwork_' + this.net_.netJson_.name+ '.model',JSON.stringify(this.net_,undefined,'  '));
    this.fft_();
  }
  fft_() {
    const statsFFT = this.net_.layerStats_.input[0];    
    //console.log('PreTrain::step statsFFT=<',statsFFT,'>');
    const phasors = fft(statsFFT);
    //console.log('PreTrain::step phasors=<',phasors,'>');
    const magnitudes = fftUtil.fftMag(phasors);
    //console.log('PreTrain::step magnitudes=<',JSON.stringify(magnitudes,undefined,'  '),'>');
    magnitudes.sort((a,b)=>{
      if( a < b ) return 1;
      if( a > b ) return -1;
      return 0;
    });
    console.log('PreTrain::step magnitudes=<',JSON.stringify(magnitudes,undefined,'  '),'>');
    console.log('PreTrain::step statsFFT=<',JSON.stringify(statsFFT,undefined,'  '),'>');
    const statsFFT_inverse =ifft(phasors);
    console.log('PreTrain::step statsFFT_inverse=<',JSON.stringify(statsFFT_inverse,undefined,'  '),'>');
    /*
    const frequencies = fftUtil.fftFreq(phasors, 8000);
    console.log('PreTrain::step frequencies=<',frequencies,'>');
    */
  }
};
