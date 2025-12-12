// let loudness = 0;
// let t = 0;

// async function simulateSerial() {
//   let raw = 0;
//   let targetLevel = 100; // Start quiet (scaled from ~25 → 1023 scale)
//   let burstCountdown = 0;

//   while (true) {
//     // Occasionally change the target level to simulate ambient changes
//     if (Math.random() < 0.02) {
//       targetLevel = Math.random() * 300 + 50;
//       // was ~13–89 → scaled to 0–1023 range
//     }

//     // Randomly trigger loud bursts (like talking, music, etc)
//     if (burstCountdown <= 0 && Math.random() < 0.05) {
//       burstCountdown = Math.floor(Math.random() * 20 + 10);

//       targetLevel = Math.random() * 750 + 300;
//       // was ~76–255 → scaled to 0–1023
//     }

//     if (burstCountdown > 0) {
//       burstCountdown--;

//       // When burst ends, drop back to quiet range again
//       if (burstCountdown === 0) {
//         targetLevel = Math.random() * 300 + 50;
//         // was ~13–64 → scaled
//       }
//     }

//     // Natural variation around the target
//     raw = targetLevel + (Math.random() - 0.5) * 200;
//     // was ±50 → scaled proportionally

//     // Clamp to 0–1023
//     raw = Math.max(0, Math.min(1023, raw));

//     // Faster smoothing for responsive behavior
//     loudness = loudness * 0.7 + raw * 0.3;

//     await new Promise((r) => setTimeout(r, 30));
//   }
// }

// function mockConnect() {
//   console.log("Simulator connected");
//   simulateSerial();
// }
