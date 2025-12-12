let port;
let reader;
let loudness = 0;

async function readSerial() {
  while (true) {
    const { value, done } = await reader.read();
    if (done) break;
    if (value) {
      for (let line of value.split("\n")) {
        let v = parseInt(line.trim());
        if (!isNaN(v)) {
          loudness = v;
          console.log("Loudness:", loudness);
        }
      }
    }
  }
}

async function connectSerial() {
  try {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 9600 });

    const decoder = new TextDecoderStream();
    port.readable.pipeTo(decoder.writable);
    reader = decoder.readable.getReader();

    readSerial();
  } catch (err) {
    console.error("Serial connection failed:", err);
  }
}
