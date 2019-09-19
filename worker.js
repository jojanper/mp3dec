const fs = require('fs');

const { parentPort } = require('worker_threads');

const { DraalDecoder, getImportObject, getMemory } = require('./decoder');


const WASMLIB = '/build-wasm/bin/mp3dec_static.wasm';

const { memory, heap } = getMemory();

const wasmModule = new WebAssembly.Module(fs.readFileSync(__dirname + WASMLIB));
const instance = new WebAssembly.Instance(wasmModule, getImportObject(memory, heap));

//const { exports } = instance;
console.log(instance.exports);

const decoder = new DraalDecoder(instance.exports, memory);
decoder.open();

console.log('HEP');

parentPort.postMessage({ ready: true });

parentPort.on('message', (msg) => {
    let eos = false;

    if (msg.type === 'data') {
        //console.log(`data received: ${msg.data.length}`);

        const status = decoder.decode(msg.data, {
            write: (data) => {
                parentPort.postMessage({ decoded: data });
            }
        });

        //console.log('status', status);
        if (eos) {
            parentPort.postMessage({ eos: true });
        }
    } else if (msg.type === 'close') {
        console.log(`close received`);
        decoder.close();
    } else if (msg.type === 'eos') {
        console.log(`eos received`);
        //decoder.close();
        eos = true;
    } else {
        throw new Error(`Unknown message type: ${msg.type}`);
    }
});
