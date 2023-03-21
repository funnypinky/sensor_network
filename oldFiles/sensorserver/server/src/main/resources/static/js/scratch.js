async function fetchJSON(url) {
  const response = await fetch(url);
  const data = await response.json();
  return data;
}
function drawChart() {
  var obj;
  fetchJSON('/api/data/sensors/' + real_data + '/lastDay')
    .then(data => obj = data)
    .then(() => console.log(obj))
    .then(() => createChart(obj))

}
function createChart(obj) {
  var xMeastime = [];
  var yTemperatur = [];
  var yHumidity = [];
  var yPressure = [];

  Object.entries(obj).forEach((entry) => {
    const [key, value] = entry;
    xMeastime.push(Date.parse(key));
    if (!isNaN(value.temperatur)) {
      yTemperatur.push(value.temperatur);
    }
    if (!isNaN(value.humidity)) {
      yHumidity.push(value.humidity);
    }
    if (!isNaN(value.pressure)) {
      yPressure.push(value.pressure / 100);
    }
  });
  // Create a new JavaScript Date object based on the timestamp
  // multiplied by 1000 so that the argument is in milliseconds, not seconds.
  var date = new Date(xMeastime[0]);
  var trace1 = {
    type: "scatter",
    mode: "lines",
    name: 'Temperatur °C',
    x: xMeastime,
    y: yTemperatur,
    line: { color: '#17BECF' }

  }
  var trace2 = {
    type: "scatter",
    mode: "lines",
    name: 'rel. Luftfeuchtigkeit %',
    x: xMeastime,
    y: yHumidity,
    yaxis: 'y2',
    line: { color: '#ff7f0e' }
  }
  var trace3 = {
    type: "scatter",
    mode: "lines",
    name: 'Luftdruck hPa',
    x: xMeastime,
    y: yPressure,
    yaxis: 'y3',
    line: { color: '#d62728' }
  }
  // Define Data
  var data = [trace1, trace2, trace3];

  // Define Layout
  var layout = {
    xaxis: {
      domain: [0.2, 0.9],
      autorange: true,
      type: 'date',
      tickformat: '%H:%M:%S'
    },
    yaxis: {
      title: "Temperatur °C",
      titlefont: {color: '#17BECF'},
      tickfont: {color: '#17BECF'},
      autorange: 'true',
      type: 'linear'
    },
    yaxis2: {
      title: 'rel. Luftfeuchtigkeit %',
      titlefont: { color: '#ff7f0e' },
      tickfont: { color: '#ff7f0e' },
      anchor: 'free',
      overlaying: 'y',
      side: 'left',
      position: 0.1,
      autorange: 'true',
      range: [0, 100],
      type: 'linear'
    },
    yaxis3: {
      title: 'Luftdruck hPa',
      titlefont: { color: '#d62728' },
      tickfont: { color: '#d62728' },
      anchor: 'x',
      overlaying: 'y',
      side: 'right',
      autorange: 'true',
      range: [900, 1200],
      type: 'linear'
    },
    title: "Verlauf für den "+date.toLocaleDateString(),
  };

  // Display using Plotly
  Plotly.newPlot("myPlot", data, layout);

}
