class CircleMode {
  constructor() {
    this.noiseOffset = 0;
  }

  draw(loudness) {
    let cx = width / 2;
    let cy = height / 2;

    noStroke();
    fill(0, 25);
    rect(0, 0, width, height);

    let baseRadius = 120;
    if (loudness < 350) {
      let glowLayers = [
        { w: 12, a: 20 },
        { w: 8, a: 60 },
        { w: 3, a: 150 },
      ];
      for (let g of glowLayers) {
        stroke(0, 255, 180, g.a);
        strokeWeight(g.w);
        noFill();
        ellipse(cx, cy, baseRadius * 2);
      }
      return;
    }

    let numPoints = 200;
    let loudPush = map(loudness, LOW, HIGH, 0, 70);

    for (let t = 0; t < 5; t++) {
      let offset = this.noiseOffset - t * 0.05;
      let alpha = 200 - t * 40;

      stroke(0, 255, 180, alpha);
      strokeWeight(3);
      noFill();
      beginShape();

      for (let i = 0; i <= numPoints; i++) {
        let angle = map(i, 0, numPoints, 0, TWO_PI);

        let blob = map(noise(i * 0.015, offset * 0.8), 0, 1, -40, 40);
        let wiggle = map(noise(i * 0.09, offset * 1.6), 0, 1, -18, 18);
        let jitter = map(noise(i * 0.5, offset * 3.5), 0, 1, -5, 5);

        let radius = baseRadius + loudPush + blob + wiggle + jitter;

        let x = cx + radius * cos(angle);
        let y = cy + radius * sin(angle);
        vertex(x, y);
      }

      endShape(CLOSE);
    }

    this.noiseOffset += 0.015 + map(loudness, LOW, HIGH, 0, 0.04);
  }
}
