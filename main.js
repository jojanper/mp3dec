const fs = require('fs');
const { Worker } = require('worker_threads');
const argv = require('minimist')(process.argv.slice(2));

const { input, output } = argv;
const WASMLIB = '/build-wasm/bin/mp3dec_static.wasm';

function startDecoding(worker, stream, chunkSize) {
    stream.on('readable', () => {
        let chunk;

        // Read fixed size data and pass for decoding
        while ((chunk = stream.read(chunkSize))) {
            worker.postMessage({ type: 'data', data: chunk });
        }

        // Read remaining data and pass for decoding + signal EOS
        while (null !== (chunk = stream.read())) {
            worker.postMessage({ type: 'data', data: chunk });
            worker.postMessage({ type: 'eos' });
        }
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

const worker = new Worker('./worker.js', { workerData: { workerLib: __dirname + WASMLIB } });

worker.on('error', err => { throw err; });
worker.on('message', (message) => {
    if (message.ready) {
        startDecoding(worker, stream, chunkSize);
    } else if (message.eos) {
        worker.postMessage({ type: 'close' });
        outStream.end();
        worker.unref();
    } else if (message.decoded) {
        outStream.write(message.decoded);
    }
});
