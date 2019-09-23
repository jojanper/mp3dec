const fs = require('fs');
const { parentPort, workerData } = require('worker_threads');

const { DraalDecoder, getImportObject, getMemory } = require('./decoder');

const { workerLib } = workerData;

// Initialize WASM module
const { memory, heap } = getMemory();
const wasmModule = new WebAssembly.Module(fs.readFileSync(workerLib));
const instance = new WebAssembly.Instance(wasmModule, getImportObject(memory, heap));

// Create decoder instance
const decoder = new DraalDecoder(instance.exports, memory);
decoder.open();

// Decoder is ready to receive data
parentPort.postMessage({ ready: true });

// Handle messages to/from worker
parentPort.on('message', (msg) => {
    if (msg.type === 'data') {
        // Input data for decoding received
        decoder.decode(msg.data, {
            write: (data) => {
                parentPort.postMessage({ decoded: data });
            }
        });
    } else if (msg.type === 'close') {
        // Close decoder instance
        decoder.close();
    } else if (msg.type === 'eos') {
        // End of stream signal received
        parentPort.postMessage({ eos: true });
    } else {
        throw new Error(`Unknown message type: ${msg.type}`);
    }
});
