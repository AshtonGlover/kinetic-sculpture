let mode = "bars";
let modes = {};
const LOW = 200;
const HIGH = 600;

async function setup() {
  createCanvas(windowWidth, windowHeight);

  let btn = createButton("Connect to Arduino");
  btn.position(10, 20);
  btn.style("padding", "12px 24px");
  btn.style("background", "rgba(0,255,180,0.1)");
  btn.style("color", "#00ffcc");
  btn.style("border", "2px solid #00ffcc");
  btn.style("border-radius", "10px");
  btn.style("font-size", "16px");
  btn.style("font-family", "Consolas, monospace");
  btn.style("cursor", "pointer");
  btn.style("text-shadow", "0 0 8px #00ffcc");
  btn.style("box-shadow", "0 0 12px #00ffcc55, inset 0 0 6px #00ffcc33");
  btn.mousePressed(connectSerial);

  modes.bars = new BarsMode();
  modes.circle = new CircleMode();
  modes.particles = new ParticlesMode();
}

function draw() {
  background(0);

  modes[mode].draw(loudness);

  drawModeLabel();
}

function keyPressed() {
  if (key === "1") mode = "bars";
  if (key === "2") mode = "circle";
  if (key === "3") mode = "particles";
}

function drawModeLabel() {
  fill(255);
  noStroke();
  textSize(16);
}
