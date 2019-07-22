const fs=require("fs");
const fft = require('fft-js').fft;
const fftUtil = require('fft-js').util;
const ifft = require('fft-js').ifft;


const fConstMinValueDiff = 0.00001;
const fConstActiveThreshold = 1.0/9.0;
const fConstMinCenterHeight = 0.1;

module.exports = class ThresholdPreTrain {
  constructor(net) {
    //this.net_ = net;
    //this.inputLayer_ = this.net_.netJson_.layers.input;
    this.inputRange_ = 255;
    this.outNode_ = 4;
    this.centers_ = [];
    this.bigPeaks_ = [];
    /*
    this.centers_ = [];
    this.initWidth_ = this.inputRange_/ (this.outNode_ + 1);
    //console.log('ThresholdPreTrain::constructor this.initWidth_=<',this.initWidth_,'>');
    for(let i = 0;i < this.outNode_ ;i++) {
      this.centers_[i] = Math.floor(this.initWidth_ *(i+1));
    }
    console.log('ThresholdPreTrain::constructor this.centers_=<',this.centers_,'>');
    */
  }
  lStep(inputVectors) {
    for(let imgInput of inputVectors) {
      let outVectory = [];
      for(let center of this.centers_) {
        let diff =  Math.abs(center-imgInput);
        if(diff < fConstMinValueDiff) {
          diff = fConstMinValueDiff;
        }
        let out = 1.0/diff;
        if(out > fConstActiveThreshold) {
          outVectory = true;
        } else {
          outVectory = false;
        }
      }
      console.log('ThresholdPreTrain::lStep outVectory=<',outVectory,'>');
    }
    console.log('ThresholdPreTrain::step inputVectors.length=<',inputVectors.length,'>');
  }
  
  
  
  step(inputVectors) {
    //console.log('ThresholdPreTrain::step inputVectors.length=<',inputVectors.length,'>');
    const inputStats = new Array(255);
    inputStats.fill(0);
    for(let imgInput of inputVectors) {
      //console.log('ThresholdPreTrain::step imgInput=<',imgInput,'>');
      if(inputStats[imgInput]) {
        inputStats[imgInput]++;
      } else {
        inputStats[imgInput] = 1;;
      }
    }
    //console.log('ThresholdPreTrain::step inputStats=<',inputStats,'>');
    //this.fft_(inputStats);
    this.topPeaks_ =  this.peak_(inputStats);
    //console.log('ThresholdPreTrain::step this.topPeaks_=<',this.topPeaks_,'>');
    this.lear_(inputVectors);
  }
  
  
  lear_(inputVectors) {
    let hintAtCenter = this.caclHint_(inputVectors,this.topPeaks_);
    console.log('ThresholdPreTrain::step hintAtCenter=<',hintAtCenter,'>');
    const maxHint = Math.max(...hintAtCenter);
    console.log('ThresholdPreTrain::step maxHint=<',maxHint,'>');
    for(let index = 0;index < hintAtCenter.length;index++) {
      const heightHint = hintAtCenter[index]/maxHint;
      console.log('ThresholdPreTrain::step heightHint=<',heightHint,'>');
      if(heightHint > fConstMinCenterHeight) {
        this.bigPeaks_.push(this.topPeaks_[index]);
      }
    }
    console.log('ThresholdPreTrain::step this.bigPeaks_=<',this.bigPeaks_,'>');
    
    let hintBigPeak = this.caclHint_(inputVectors,this.bigPeaks_);
    console.log('ThresholdPreTrain::step hintBigPeak=<',hintBigPeak,'>');
    const maxHint2 = Math.max(...hintBigPeak);
    console.log('ThresholdPreTrain::step maxHint2=<',maxHint2,'>');
    for(let index = 0;index < hintBigPeak.length;index++) {
      const heightHint2 = hintBigPeak[index]/maxHint2;
      console.log('ThresholdPreTrain::step heightHint2=<',heightHint2,'>');
    }


  }
  showHintPeaks_() {
    
  }


  caclHint_(inputVectors,peaks) {
    console.log('ThresholdPreTrain::step peaks=<',peaks,'>');
    let hintCounter = 0;
    let hintAtCenter = new Array(peaks.length);
    hintAtCenter.fill(0);
    for(let imgInput of inputVectors) {
      let outVectory = [];
      let hint = 0;
      for(let index = 0;index < peaks.length;index++) {
        const center = peaks[index];
        let diff =  Math.abs(center-imgInput);
        if(diff < fConstMinValueDiff) {
          diff = fConstMinValueDiff;
        }
        let out = 1.0/diff;
        if(out > fConstActiveThreshold) {
          outVectory = true;
          hint++;
          hintAtCenter[index]++;
        } else {
          outVectory = false;
        }
      }
      //console.log('ThresholdPreTrain::lStep outVectory=<',outVectory,'>');
      if(hint > 0) {
        //console.log('ThresholdPreTrain::lStep outVectory=<',outVectory,'>');
        hintCounter++
      } else {
        
      }
    }    
    console.log('ThresholdPreTrain::step inputVectors.length=<',inputVectors.length,'>');
    console.log('ThresholdPreTrain::step hintCounter=<',hintCounter,'>');
    const hintRate = hintCounter/inputVectors.length;
    console.log('ThresholdPreTrain::step hintRate=<',hintRate,'>');
    console.log('ThresholdPreTrain::step hintAtCenter=<',hintAtCenter,'>');
    return hintAtCenter;
  }

  
  peak_(inputStats) {
    let topPeaks = [];
    let bottomPeaks = [];
    for(let i = 1;i < inputStats.length -1 ;i++) {
      let center = inputStats[i];
      let left = inputStats[i-1];
      let right = inputStats[i+1];
      if(center >= left && center >= right) {
        topPeaks.push(i);
      }
      if(center <= left && center <= right) {
        bottomPeaks.push(i);
      }
    }
    //console.log('ThresholdPreTrain::step bottomPeaks=<',bottomPeaks,'>');
    //console.log('ThresholdPreTrain::step topPeaks=<',topPeaks,'>');
    for( let bPeak of bottomPeaks) {
      //console.log('ThresholdPreTrain::step bPeak=<',bPeak,'>');
      let index = topPeaks.indexOf(bPeak);
      //console.log('ThresholdPreTrain::step index=<',index,'>');
      if(index !== -1 ) {
        topPeaks.splice(index,1);
      }
    }
    //console.log('ThresholdPreTrain::step topPeaks=<',topPeaks,'>');
    return topPeaks;
  }
  
  fft_(statsFFT) {
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
