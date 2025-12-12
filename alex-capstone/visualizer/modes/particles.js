class ParticlesMode {
  constructor() {
    this.particles = [];
  }

  draw(loudness) {
    let amount = map(loudness, 0, 1023, 1, 10);

    for (let i = 0; i < amount; i++) {
      this.particles.push({
        x: width / 2,
        y: height / 2,
        vx: random(-2, 2),
        vy: random(-2, 2),
        size: random(3, 8),
      });
    }

    for (let p of this.particles) {
      fill(255, random(150, 255), 0, 200);
      noStroke();
      circle(p.x, p.y, p.size);

      p.x += p.vx;
      p.y += p.vy;
      p.size *= 0.98;
    }

    this.particles = this.particles.filter((p) => p.size > 0.5);
  }
}
