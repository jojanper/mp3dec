const fs = require('fs');
const path = require('path');
const { Worker } = require('worker_threads');
const argv = require('minimist')(process.argv.slice(2));

const { input, output } = argv;
const WASMLIB = path.join(__dirname, 'build-wasm/bin/mp3dec_static.wasm');

/*
const {
    DraalDecoder, getImportObject, getMemory, eventHandler
} = require('./src/js/decoder');

const importObj = require('./src/js/import');
console.log(importObj);
//as
*/

function startDecoding(worker, stream, chunkSize) {
    stream.on('readable', () => {
        let chunk;

        // Read fixed size data and pass for decoding
        do {
            chunk = stream.read(chunkSize);
            worker.postMessage({ type: 'data', data: chunk });
        } while (chunk);

        // Read remaining data and pass for decoding + signal EOS
        do {
            chunk = stream.read();
            worker.postMessage({ type: 'data', data: chunk });
            if (chunk) {
                worker.postMessage({ type: 'eos' });
            }
        } while (chunk);
    });
}

if (!input) {
    throw new Error('No input file specified (use --input=<mp3-file>)');
}

if (!output) {
    throw new Error('No output file specified (use --output=<decoded-file>)');
}

const stream = fs.createReadStream(input);
const outStream = fs.createWriteStream(output);
const chunkSize = 32 * 1024;

// Initialize WASM module
//const { memory, heap } = getMemory();
//const wasmModule = new WebAssembly.Module(fs.readFileSync(WASMLIB));
//const instance = new WebAssembly.Instance(wasmModule, getImportObject(memory, heap));
//const instance = new WebAssembly.Instance(wasmModule, importObj.asm);

// Create decoder instance
//const decoder = DraalDecoder.create(instance.exports, importObj.asm.env.memory);

// Start Web Worker and pass the library name as input
const worker = new Worker('./src/js/worker.js', { workerData: { workerLib: WASMLIB } });

worker.on('error', err => { throw err; });

// Handle messages from worker
worker.on('message', message => {
    if (message.ready) {
        // Decoder is ready to receive data
        console.log('Start decoding');
        startDecoding(worker, stream, chunkSize);
    } else if (message.eos) {
        // Worker signalled that end-of-stream has been encountered
        console.log('Decoding finished');
        worker.postMessage({ type: 'close' });
        outStream.end();
        worker.unref();
    } else if (message.decoded) {
        // Decoded audio samples
        outStream.write(message.decoded);
    }
});
