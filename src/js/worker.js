const fs = require('fs');
const { parentPort, workerData } = require('worker_threads');

const {
    DraalDecoder, getImportObject, getMemory, eventHandler
} = require('./decoder');

const { workerLib } = workerData;

// Initialize WASM module
const memory = getMemory();
const wasmModule = new WebAssembly.Module(fs.readFileSync(workerLib));
const instance = new WebAssembly.Instance(wasmModule, getImportObject(memory));

// Create decoder instance
const decoder = DraalDecoder.create(instance.exports, memory.memory);

// Decoder is ready to receive data
parentPort.postMessage({ ready: true });

// Handle messages to/from worker
parentPort.on('message', eventHandler(decoder, data => parentPort.postMessage(data)));
