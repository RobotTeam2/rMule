onModelFileSelect = (evt)=> {
  //console.log('onModelFileSelect::evt=<',evt,'>');
  let files = evt.target.files;
  //console.log('onModelFileSelect::files=<',files,'>');
  if(files.length > 0) {
    let file = files[0];
    //console.log('onModelFileSelect::file=<',file,'>');
    readTextFile(file,(text)=> {
      //console.log('onModelFileSelect::text=<',text,'>');
      onModelContents(text);
    });
  }
}


readTextFile = (file,cb) => {
  let fileReader = new FileReader();
  fileReader.onload =  () => {
    //console.log('readTextFile::fileReader.result=<',fileReader.result,'>');
    cb(fileReader.result);
  }
  fileReader.readAsText(file);
}

let gPingData = [];

onModelContents = (text) => {
  //console.log('onModelContents::text=<',text,'>');
  let modelJson = JSON.parse(text);
  //console.log('onModelContents::modelJson=<',modelJson,'>');
  onDrawInputLayerStats(modelJson);
}


let options = {
  scales:{
    xAxes: [{
      display: true 
    }]
  }
}

const COLOR_MAP = [
 'red','blue','green','Cyan',
 'Sienna','Gold','HotPink','Magenta',
 'Maroon','Yellow','tan','orange',
 'olive','salmon','indigo','silver',
];

onDrawInputLayerStats = (model) => {
  console.log('onDrawInputLayerStats::model=<',model,'>');
  console.log('onDrawInputLayerStats::model.layerStats_.input=<',model.layerStats_.input,'>');
  let dataSets = [];
  let counter = 0;
  for(let dataStats of model.layerStats_.input) {
    console.log('onDrawInputLayerStats::dataStats=<',dataStats,'>');
    let gData = {
      label: 'node' + counter,
      borderColor:COLOR_MAP[counter],
      fill: false,
      borderWidth:1,
      pointBorderWidth:1,
      pointRadius:0,
      data:dataStats
    }
    dataSets.push(gData);
    counter++;
  }
  let labels = [];
  for(let i = 0;i < model.layerStats_.input[0].length;i++) {
    labels.push(i);
  }

  let ctx = document.getElementById('mmn-stats-canvas').getContext('2d');
  let graphOption = {
    type: 'line',
    data: {
      labels:labels,
      datasets: dataSets
    },
    options: options
  };
  let myChart = new Chart(ctx, graphOption);
}


document.getElementById('mmn-model-file').addEventListener('change', onModelFileSelect, false);


