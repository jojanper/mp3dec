const fs = require('fs');
const { Worker } = require('worker_threads');

const stream = fs.createReadStream(__dirname + '/Bryan_Adams_Xmas_Time.mp3');
const outStream = fs.createWriteStream('test.raw');
const chunkSize = 32 * 1024;

const worker = new Worker('./worker.js');

worker.on('error', err => { throw err; });
worker.on('message', (message) => {
    //console.log('message received from worker');
    //console.log(message);

    if (message.ready) {
        startDecoding();
    } else if (message.eos) {
        console.log('CLOSING');
        worker.postMessage({ type: 'close' });
        outStream.end();
        worker.unref();
        //decoder.close();
        console.log('end');
    } else if (message.decoded) {
        console.log(`Received decoded data: ${message.decoded.length}`);
        outStream.write(message.decoded);
    }
});

console.log(worker);

function startDecoding() {
    stream.on('readable', () => {
        let chunk;

        //console.log('HEPO');

        // Read fixed size data
        while ((chunk = stream.read(chunkSize))) {
            //console.log(chunk.length);
            worker.postMessage({ type: 'data', data: chunk });
            //if (!decoder.decode(chunk, outStream)) {
            //    continue;
            //}
        }

        // Read remaining data
        while (null !== (chunk = stream.read())) {
            //console.log(chunk.length);
            //decoder.decode(chunk, outStream);
            worker.postMessage({ type: 'data', data: chunk });
            worker.postMessage({ type: 'eos' });
            //outStream.end();
            //worker.unref();
            //decoder.close();
            //console.log('end');
        }

        //console.log('done');
    });
}
