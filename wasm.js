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
        'table': new WebAssembly.Table({ initial: 400, maximum: 400, element: 'anyfunc' }),
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
        ___cxa_uncaught_exceptions: function () {
            console.log('___cxa_uncaught_exceptions');
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

class DraalDecoder {
    constructor(api, memory) {
        this.api = api;
        this.memory = memory;

        this.decoder = null;
        this.initialized = false;

        this.frames = 0;
        this.jsInput = null;
        this.wasmInputPtr = null;
        this.wasmInput = null;
    }

    open() {
        this.decoder = this.api._openDecoder();
    }

    close() {
        this.api._destroyBuffer(this.wasmInputPtr);
        this.api._closeDecoder(this.decoder);

        this.decoder = null;
        this.wasmInputPtr = null;
        this.initialized = false;
    }

    decode(dataChunk, outStream) {
        if (!this.initialized) {
            console.log('Initialize decoder');

            this.jsInput = new Uint8Array(dataChunk.length);
            this.wasmInputPtr = this.api._createBuffer(this.jsInput.length);
            this.wasmInput = new Uint8Array(this.memory.buffer, this.wasmInputPtr, this.jsInput.length);

            this.wasmInput.set(dataChunk);

            if (!this.decoder) {
                this.open();
            }

            const init = this.api._initDecoder(this.decoder, this.wasmInputPtr, this.jsInput.length);
            console.log('Decoder init result is', init);
            this.initialized = true;
        } else {
            this.wasmInput.set(dataChunk);
            this.api._addInput(this.decoder, this.wasmInputPtr, dataChunk.length);
        }

        return this._decodeFrame(outStream);
    }

    _decodeFrame(outStream) {
        while (1) {
            const result = this.api._decode(this.decoder);
            if (result) {
                const decPtr = this.api._getAudio(this.decoder);
                const nDecSamples = this.api._getAudioSize(this.decoder);
                console.log('Decoding result', result, this.frames, nDecSamples);

                const jsData = new Uint8Array(this.memory.buffer, decPtr, nDecSamples);
                const slicedData = jsData.slice(0, nDecSamples);
                outStream.write(Buffer.from(slicedData.buffer, 0, nDecSamples));
            }

            if (result === 0) {
                return false;
            }

            this.frames++;
        }

        return true;
    }
}

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

    const chunkSize = 32 * 1024;

    const { exports } = instance;
    console.log(exports);

    const decoder = new DraalDecoder(exports, memory);
    decoder.open();

    stream.on('readable', () => {
        let chunk;

        // Read fixed size data
        while ((chunk = stream.read(chunkSize))) {
            if (!decoder.decode(chunk, outStream)) {
                continue;
            }
        }

        // Read remaining data
        while (null !== (chunk = stream.read())) {
            decoder.decode(chunk, outStream);
            outStream.end();
            decoder.close();
        }
    });
}

testExec(instance);
