/**
 * JavaScript interface for accessing MP3 decoder operating in WebAssembly context.
 * This interface wraps the underlying WebAssembly decoder API into easy-to-use JS class.
 */
class DraalDecoder {
    static create(api, memory) {
        return new DraalDecoder(api, memory).open();
    }

    /**
     * Constructor.
     *
     * @param {*} api Exported WebAssembly API for the MP3 decoder.
     * @param {*} memory Memory object to be used for data between JS and WebAssembly.
     */
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

    /**
     * Open decoder.
     */
    open() {
        console.log('open decoder');
        console.log(this.api);
        //this.api.__GLOBAL__sub_I_synfilt_cpp();

        console.log('HEP');
        console.log(this.api._sbrk());
        this.decoder = this.api._openDecoder();
        console.log(this.decoder);

        return this;
    }

    /**
     * Close decoder and related resource.
     */
    close() {
        this.api._destroyBuffer(this.wasmInputPtr);
        this.api._closeDecoder(this.decoder);

        this.decoder = null;
        this.wasmInputPtr = null;
        this.initialized = false;
    }

    /**
     * Decode input stream into audio samples.
     *
     * @param {*} dataChunk MP3 bitstream data.
     * @param {*} outStreamApi Callback for decoded output samples.
     */
    decode(dataChunk, outStreamApi) {
        if (!this.initialized) {
            this.jsInput = new Uint8Array(dataChunk.length);
            this.wasmInputPtr = this.api._createBuffer(this.jsInput.length);
            this.wasmInput = new Uint8Array(this.memory.buffer, this.wasmInputPtr, this.jsInput.length);

            this.wasmInput.set(dataChunk);

            if (!this.decoder) {
                console.log('open');
                this.open();
                console.log('done');
                return;
            }

            const init = this.api._initDecoder(this.decoder, this.wasmInputPtr, this.jsInput.length);
            if (!init) {
                return false;
            }

            this.initialized = true;
        } else {
            this.wasmInput.set(dataChunk);
            this.api._addInput(this.decoder, this.wasmInputPtr, dataChunk.length);
        }

        return this._decodeFrame(outStreamApi);
    }

    /**
     * Decode audio frames currently available for the decoder. This is internal
     * API and should not be called from outside.
     *
     * @param {*} outStreamApi Callback API for output samples.
     */
    _decodeFrame(outStreamApi) {
        let status = true;
        while (status) {
            const result = this.api._decode(this.decoder);
            if (result) {
                const decPtr = this.api._getAudio(this.decoder);
                const nDecSamples = this.api._getAudioSize(this.decoder);
                // console.log('Decoding result', result, this.frames, nDecSamples);

                const jsData = new Uint8Array(this.memory.buffer, decPtr, nDecSamples);
                const slicedData = jsData.slice(0, nDecSamples);
                outStreamApi.write(Buffer.from(slicedData.buffer, 0, nDecSamples));
            }

            if (result === 0) {
                status = false;
            }

            this.frames++;
        }

        return status;
    }
}

/**
 * Return memory and heap objects for WebAssembly instance.
 */
function getMemory() {
    /*
    const memory = new WebAssembly.Memory({
        initial: 256,
        maximum: 256
    });

    const heap = new Uint8Array(memory.buffer);
    */

    return {
        memory: null,
        heap: null
    };
}

/**
 * Return object to be imported for WebAssembly instance.
 *
 * @param {*} memory WebAssembly memory object.
 * @param {*} heap Heap object.
 */
function getImportObject(/*memory, heap*/) {
    const memory = new WebAssembly.Memory({
        initial: 256,
        maximum: 256
    });

    const heap = new Uint8Array(memory.buffer);

    var tempRet0 = 0;

    var setTempRet0 = function (value) {
        tempRet0 = value;
        console.log('setTempRet0');
    };

    var getTempRet0 = function () {
        return tempRet0;
    };

    const heapLength = heap.length;

    const table = new WebAssembly.Table({
        initial: 400,
        maximum: 400,
        element: 'anyfunc'
    });

    const Math_cos = Math.cos;
    const Math_sin = Math.sin;

    const DYNAMICTOP_PTR = 1 * 8192;//94528;//96272;

    const env = {
        "___cxa_allocate_exception": () => console.log('___cxa_allocate_exception'),
        "___cxa_uncaught_exceptions": () => console.log('___cxa_uncaught_exceptions'),
        "___cxa_begin_catch": () => console.log('___cxa_begin_catch'),
        "___cxa_throw": () => console.log('___cxa_throw'),
        "___cxa_pure_virtual": () => console.log('___cxa_pure_virtual'),
        //"___exception_addRef": () => { },
        //"___exception_deAdjust": () => { },
        //"___gxx_personality_v0": () => { },

        "___syscall140": () => console.log('___syscall140'),
        "___syscall6": () => console.log('___syscall6'),

        "___lock": () => console.log('___lock'),
        "___setErrNo": () => console.log('___setErrNo'),
        "___unlock": () => console.log('___unlock'),
        "___wasi_fd_close": () => console.log('___wasi_fd_close'),
        "___wasi_fd_seek": () => console.log('___wasi_fd_seek'),
        "___wasi_fd_write": () => console.log('___wasi_fd_write'),
        "__memory_base": 1024,
        "__table_base": 0,
        "_abort": (err) => { throw new Error(err); },
        "_emscripten_get_heap_size": () => {
            console.log('HEAP size', heap.length);
            return heap.length;
        },
        "_emscripten_memcpy_big": (dest, src, count) => {
            console.log('emscripten_memcpy_big');
            heap.set(heap.subarray(src, src + count), dest);
        },
        //"_malloc": () => console.log('malloc called'),
        "_emscripten_resize_heap": () => { throw new Error('heap resize'); },
        //"_fd_close": () => { },
        //"_fd_seek": () => { },
        //"_fd_write": () => { },

        "_llvm_cos_f64": Math_cos,
        "_llvm_sin_f64": Math_sin,
        _llvm_exp2_f64: val => 2 ** val,

        "_llvm_trap": () => { throw new Error('trap'); },
        "abort": (err) => { throw new Error(err); },
        "abortOnCannotGrowMemory": () => { throw new Error('abortOnCannotGrowMemory'); },
        "abortStackOverflow": () => { throw new Error('abortStackOverflow'); },
        //"demangle": (func) => func,
        /*
        "demangleAll": (text) => {
            var regex =
                /\b__Z[\w\d_]+/g;
            return text.replace(regex,
                function (x) {
                    var y = x;
                    return x === y ? x : (y + ' [' + x + ']');
                });
        },
        */
        //"getTempRet0": getTempRet0,
        //"jsStackTrace": jsStackTrace,
        memory,
        "nullFunc_ii": () => console.log('nullFunc_ii'),
        "nullFunc_iidiiii": () => console.log('nullFunc_iidiiii'),
        "nullFunc_iii": () => console.log('nullFunc_iii'),
        "nullFunc_iiii": () => console.log('nullFunc_iiii'),
        "nullFunc_iiiii": () => console.log('nullFunc_iiiii'),
        "nullFunc_jiji": () => console.log('nullFunc_jiji'),
        "nullFunc_v": () => console.log('nullFunc_v'),
        "nullFunc_vi": () => console.log('nullFunc_vi'),
        "nullFunc_vii": () => console.log('nullFunc_vii'),
        "nullFunc_viii": () => console.log('nullFunc_viii'),
        "nullFunc_viiii": () => console.log('nullFunc_viiii'),
        "nullFunc_viiiii": () => console.log('nullFunc_viiiii'),
        "nullFunc_viiiiii": () => console.log('nullFunc_viiiii'),
        "nullFunc_viij": () => console.log('nullFunc_viij'),
        "setTempRet0": setTempRet0,
        //"stackTrace": stackTrace,
        //"table": new WebAssembly.Table({ initial: 5184, maximum: 5184, element: 'anyfunc' }),
        table,
        "tempDoublePtr": 94544,//96304,
        DYNAMICTOP_PTR
    };


    return {
        'global.Math': Math,
        global: {
            //Infinity: 10 ** 1000
            'NaN': NaN,
            'Infinity': Infinity
        },
        env
    };
}

/**
 * Handler events from/to decoding worker.
 *
 * @param {*} decoder WebAssembly decoder instance.
 * @param {*} callback Callback handler.
 */
function eventHandler(decoder, callback) {
    return function handler(msg) {
        if (msg.type === 'data') {
            // Input data for decoding received
            if (msg.data) {
                decoder.decode(msg.data, {
                    write: data => callback({ decoded: data })
                });
            }
        } else if (msg.type === 'close') {
            // Close decoder instance
            decoder.close();
        } else if (msg.type === 'eos') {
            // End of stream signal received
            callback({ eos: true });
        } else {
            throw new Error(`Unknown message type: ${msg.type}`);
        }
    };
}

module.exports = {
    DraalDecoder,
    getImportObject,
    getMemory,
    eventHandler
};
