const fs = require('fs');

// play --type raw --rate 44100 --endian little --encoding signed-integer --bits 16 --channels 2 test.raw


const WASMLIB = '/build-wasm/bin/mp3dec_static.wasm';

const memory = new WebAssembly.Memory({
    initial: 256,
    maximum: 256
});

const heap = new Uint8Array(memory.buffer);

const importObject = {
    'global.Math': Math,
    global: {
        Infinity: Math.pow(10, 1000)
    },
    env: {
        'abortStackOverflow': _ => { throw new Error('overflow'); },
        '__memory_base': 1024,
        '__table_base': 0,
        'memory': memory,
        'table': new WebAssembly.Table({ initial: 176, maximum: 176, element: 'anyfunc' }),
        'STACKTOP': 0,
        'STACK_MAX': memory.buffer.byteLength,
        //abort: () => { },
        ___syscall146: () => { },
        ___setErrNo: () => { },
        _abort: () => { throw new Error('abort'); },
        __emval_take_value: () => { },
        __emval_incref: () => { },
        __emval_decref: () => { },
        ___syscall6: () => { },
        ___syscall140: () => { },
        abortOnCannotGrowMemory: err => { throw new Error(err); },
        //_emscripten_resize_heap: () => { },
        //_emscripten_memcpy_big: () => { },
        //_emscripten_get_heap_size: () => { },
        _llvm_trap: () => { console.log('llvm_trap'); },
        _llvm_exp2_f64: (val) => {
            //console.log('llvm_exp2_f64', val);
            return Math.pow(2, val);
            //const result = Math.pow(2, val);
            //console.log('llvm_exp2_f64', val, result);
            //return result;
        },
        ___cxa_pure_virtual: () => {
            console.log('___cxa_pure_virtual');
        },

        memory: memory,
        DYNAMICTOP_PTR: 8192,
        abort: function (err) {
            console.log(err);
            throw new Error('abort ', err);
        },
        abortOnCannotGrowMemory: function (err) {
            throw new Error('abortOnCannotGrowMemory ' + err);
        },
        ___cxa_throw: function (ptr, type, destructor) {
            console.error('cxa_throw: throwing an exception, ' + [ptr, type, destructor]);
        },
        ___cxa_allocate_exception: function (size) {
            console.error('cxa_allocate_exception' + size);
            return false; // always fail
        },
        ___cxa_uncaught_exception: function () {
            console.log('___cxa_uncaught_exception');
        },

        setTempRet0: function () {
            return 0;
        },

        /*
        ___setErrNo: function (err) {
            throw new Error('ErrNo ' + err);
        },
        */
        _emscripten_get_heap_size: function () {
            //console.log('_emscripten_get_heap_size');
            return heap.length;
        },
        _emscripten_resize_heap: function (size) {
            //console.log('_emscripten_resize_heap');
            return false; // always fail
        },
        _emscripten_memcpy_big: function (dest, src, count) {
            //console.log('_emscripten_memcpy_big', count);
            heap.set(heap.subarray(src, src + count), dest);
        },
        __table_base: 0,

        //DYNAMICTOP_PTR: 0,

        nullFunc_ii: () => console.log('nullFunc_ii'),
        nullFunc_iii: () => console.log('nullFunc_iii'),
        nullFunc_iiii: () => console.log('nullFunc_iiii'),
        nullFunc_iiiii: () => console.log('nullFunc_iiiii'),
        nullFunc_v: () => console.log('nullFunc_v'),
        nullFunc_vi: () => console.log('nullFunc_vi'),
        nullFunc_vii: () => console.log('nullFunc_vii'),
        nullFunc_viii: () => console.log('nullFunc_viii'),
        nullFunc_viiii: () => console.log('nullFunc_viiii'),
        nullFunc_viiiii: () => console.log('nullFunc_viiiii'),
        nullFunc_viiiiii: () => console.log('nullFunc_viiiiii'),
        ___cxa_begin_catch: () => console.log('___cxa_begin_catch'),
        ___lock: () => console.log('___lock'),
        ___unlock: () => console.log('___unlock'),
        ___syscall54: () => { },
        _llvm_cos_f64: () => 0,
        _llvm_sin_f64: () => 0,
        tempDoublePtr: 0,
    }
};

const wasmModule = new WebAssembly.Module(fs.readFileSync(__dirname + WASMLIB));
const instance = new WebAssembly.Instance(wasmModule, importObject);

function testExec(instance) {
    const stream = fs.createReadStream(__dirname + '/Bryan_Adams_Xmas_Time.mp3');
    //const stream = fs.createReadStream(__dirname + '/ZZ_Top-Rough_Boy.mp3');
    //const stream = fs.createReadStream(__dirname + '/Toto-Africa.mp3');
    //const stream = fs.createReadStream(__dirname + '/Record.mp3');
    //const stream = fs.createReadStream(__dirname + '/Jon_Secada-Just_Another_Day.mp3');
    //const stream = fs.createReadStream(__dirname + '/Natalie_Cole_Miss_You_Like_Crazy.mp3');

    const outStream = fs.createWriteStream('test.raw');
    //const outStream = fd = fs.openSync('test.raw', 'w');

    const chunkSize = 32 * 1024;

    const { exports } = instance;
    console.log(exports);

    let initialized = false;
    const decoder = exports._openDecoder();

    let frames = 0;

    let jsInput;
    let wasmInputPtr;
    let wasmInput;

    stream.on('readable', () => {
        console.log('FOOFOO');

        let chunk;
        while ((chunk = stream.read(chunkSize))) {
            console.log(`First received ${chunk.length} bytes of data`);

            if (!initialized) {
                console.log('Initialize decoder');

                jsInput = new Uint8Array(chunk.length);
                wasmInputPtr = exports._createBuffer(jsInput.length);
                wasmInput = new Uint8Array(memory.buffer, wasmInputPtr, jsInput.length);

                wasmInput.set(chunk);

                /*
                console.log(chunk);
                for (let i = 0; i < 10; i++)
                    console.log(chunk[i]);
                    */

                const init = exports._initDecoder(decoder, wasmInputPtr, jsInput.length);

                console.log('Decoder init result is', init);

                initialized = true;

                while (1) {
                    const result = exports._decode(decoder);
                    if (result) {
                        const decPtr = exports._getAudio(decoder);
                        const nDecSamples = exports._getAudioSize(decoder);
                        console.log('Decoding result', result, frames, nDecSamples);

                        const jsData = new Uint8Array(memory.buffer, decPtr, nDecSamples);

                        //const buffer = Buffer.from(jsData.buffer, 0, jsData.length);
                        const slicedData = jsData.slice(0, nDecSamples);
                        //const buffer = Buffer.from(slicedData);
                        //const response = outStream.write(buffer);
                        //const response = fs.writeSync(outStream, slicedData, 0, nDecSamples);
                        outStream.write(Buffer.from(slicedData.buffer, 0, nDecSamples));
                        //console.log(jsData.length, buffer.length, response);
                        //as
                        //outStream.write(jsData.buffer.slice(0, nDecSamples));

                        /*
                        for (let i = 0; i < nDecSamples; i++)
                            console.log(jsData[i], slicedData[i], buffer[i]);
                        */

                        //for (let i = 0; i < 10; i++)
                        //console.log(chunk[i]);
                        //const d = buffer.map(item => item);

                        //const od = d.join('\n');
                        //console.log(od);
                        //outStream.write(od);
                        //console.log('done');

                        //const decOutput = new Int16Array(2304);
                        //const wasmDecOutputPtr = exports._create_buffer(decOutput.length);
                        //const wasmDecOutput = new Int16Array(memory.buffer, wasmDecOutputPtr, decOutput.length);
                    }

                    if (result === 0) {
                        //outStream.end();
                        //fs.close(outStream, () => { });
                        //as
                        //process.exit(1);
                        break;
                    }

                    frames++;
                }
            } else {
                wasmInput.set(chunk);

                const ret1 = exports._addInput(decoder, wasmInputPtr, jsInput.length);

                while (1) {
                    const result = exports._decode(decoder);
                    if (result) {
                        const decPtr = exports._getAudio(decoder);
                        const nDecSamples = exports._getAudioSize(decoder);

                        const jsData = new Uint8Array(memory.buffer, decPtr, nDecSamples);

                        const slicedData = jsData.slice(0, nDecSamples);

                        //const response = fs.writeSync(outStream, slicedData, 0, nDecSamples);

                        //const buffer = Buffer.from(slicedData);
                        outStream.write(Buffer.from(slicedData.buffer, 0, nDecSamples));
                        //console.log(jsData.length, buffer.length);
                        //const buffer = Buffer.from(jsData.buffer.slice(0, nDecSamples));
                        //outStream.write(buffer);

                        console.log('Decoding result', ret1, result, frames, nDecSamples);
                    }

                    if (result === 0) {
                        break;
                    }

                    frames++;
                }

                //const ret2 = exports._decode();

                //console.log(ret1, ret2);
            }

            //exports._destroy_buffer(wasmInputPtr);
        }

        while (null !== (chunk = stream.read())) {
            console.log(`Received ${chunk.length} bytes of data`);

            wasmInput.set(chunk);

            const ret1 = exports._addInput(decoder, wasmInputPtr, chunk.length);

            while (1) {
                const result = exports._decode(decoder);
                if (result) {
                    const decPtr = exports._getAudio(decoder);
                    const nDecSamples = exports._getAudioSize(decoder);

                    const jsData = new Uint8Array(memory.buffer, decPtr, nDecSamples);

                    const slicedData = jsData.slice(0, nDecSamples);

                    //const response = fs.writeSync(outStream, slicedData, 0, nDecSamples);

                    //const buffer = Buffer.from(slicedData);
                    outStream.write(Buffer.from(slicedData.buffer, 0, nDecSamples));
                    //console.log(jsData.length, buffer.length);
                    //const buffer = Buffer.from(jsData.buffer.slice(0, nDecSamples));
                    //outStream.write(buffer);

                    console.log('Decoding result', ret1, result, frames, nDecSamples);
                }

                if (result === 0) {
                    break;
                }

                frames++;
            }

            outStream.end();
            //fs.close(outStream, () => { });
            exports._destroyBuffer(wasmInputPtr);
            exports._closeDecoder(decoder);
        }
    });

    /*
    //outStream.end();

    // JavaScript sends data to WebAssembly, WebAssembly accesses the data via pointer
    const jsInput = new Uint8Array(3);
    const wasmInputPtr = exports._create_buffer(jsInput.length);
    const wasmInput = new Uint8Array(memory.buffer, wasmInputPtr, jsInput.length);

    // Copy data in to be used by WebAssembly
    wasmInput.set(jsInput);

    // Process
    exports._inc_array(wasmInputPtr, jsInput.length);

    // Copy data out to JavaScript
    jsInput.set(wasmInput);

    exports._destroy_buffer(wasmInputPtr);

    console.log(jsInput);

    // WebAssembly owns the data, JavaScript uses the data
    const staticDataPtr = exports._get_data();
    const jsData = new Uint8Array(memory.buffer, staticDataPtr, 3);

    console.log(jsData);

    // Copy WebAssembly owned data to JavaScript array
    const jsLocalData = new Uint8Array(3);
    jsLocalData.set(jsData);

    console.log(jsLocalData);

    let input = 21;

    // Call doubler API
    for (let i = 0; i < 10; i++) {
        const result = exports._doubler(input);
        console.log(input + ' doubled is ' + result);
        input = result;
    };

    //exports._closeDecoder();
    */
}

testExec(instance);

//console.log('HEP');
