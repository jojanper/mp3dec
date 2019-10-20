const fs = require('fs');
const { parentPort, workerData } = require('worker_threads');

const {
    DraalDecoder, getImportObject, getMemory, eventHandler
} = require('./decoder');

const importObj = require('./import');
console.log(importObj);

const { workerLib } = workerData;

// Initialize WASM module
//const { memory, heap } = getMemory();
const wasmModule = new WebAssembly.Module(fs.readFileSync(workerLib));
//const instance = new WebAssembly.Instance(wasmModule, getImportObject(memory, heap));
const instance = new WebAssembly.Instance(wasmModule, importObj.asm);

// Create decoder instance
//const decoder = DraalDecoder.create(instance.exports, memory);
const decoder = DraalDecoder.create(instance.exports, importObj.asm.env.memory);

// Decoder is ready to receive data
parentPort.postMessage({ ready: true });

// Handle messages to/from worker
parentPort.on('message', eventHandler(decoder, data => parentPort.postMessage(data)));
