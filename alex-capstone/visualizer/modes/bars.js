class BarsMode {
  constructor(numBars = 20, pixelSize = 30, historyLength = 200) {
    this.numBars = numBars;
    this.pixelSize = pixelSize;
    this.historyLength = historyLength;

    this.sampleHistory = [];

    this.barValues = Array(this.numBars).fill(0);
  }

  draw(micValue) {
    this.sampleHistory.push(micValue);
    if (this.sampleHistory.length > this.historyLength) {
      this.sampleHistory.shift();
    }

    let binSize = floor(this.historyLength / this.numBars);
    for (let i = 0; i < this.numBars; i++) {
      let start = i * binSize;
      let end = start + binSize;
      let sum = 0;
      for (let j = start; j < end; j++) {
        sum += this.sampleHistory[j] || 0;
      }
      let avg = sum / binSize;

      this.barValues[i] = lerp(this.barValues[i], avg, 0.2);
    }

    let barWidth = width / this.numBars;
    for (let i = 0; i < this.numBars; i++) {
      let barHeight = map(this.barValues[i], 200, 400, 0, height);
      let numPixels = floor(barHeight / this.pixelSize);

      for (let p = 0; p < numPixels; p++) {
        let y = height - (p + 1) * this.pixelSize;
        let localT = p / max(1, numPixels - 1);
        let barT = barHeight / height;
        let t = localT * barT;

        let hue;
        if (t < 0.5) {
          hue = lerp(0, 125, t * 2);
        } else {
          hue = lerp(125, 215, (t - 0.5) * 2);
        }

        fill(hue, 0, 255);
        rect(i * barWidth, y, barWidth - 2, this.pixelSize - 1);
      }
    }
  }
}
