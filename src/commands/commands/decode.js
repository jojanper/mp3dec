const fs = require('fs');
const path = require('path');
const { Worker } = require('worker_threads');

const WASMLIB = path.join(__dirname, '..', '..', '..', 'build-wasm/bin/mp3dec_static.wasm');

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

async function decode({ input, output}) {
    return new Promise(resolve => {
        const stream = fs.createReadStream(input);
        const outStream = fs.createWriteStream(output);
        const chunkSize = 32 * 1024;

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
                resolve();
            } else if (message.decoded) {
                // Decoded audio samples
                outStream.write(message.decoded);
            }
        });
    });
}

module.exports = program => {
    program
        .command('decode')
        .description('Decode audio file')
        .requiredOption('--input <file>', 'Audio input (mp3)')
        .requiredOption('--output <file>', 'Audio output (PCM)')
        .action(options => decode(options).catch(err => {
            console.log(err); process.exit(1);
        }));
};
