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
        this.decoder = this.api._openDecoder();
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
    const memory = new WebAssembly.Memory({
        initial: 256,
        maximum: 256
    });

    const heap = new Uint8Array(memory.buffer);

    return {
        memory,
        heap
    };
}

/**
 * Return object to be imported for WebAssembly instance.
 *
 * @param {*} memory WebAssembly memory object.
 * @param {*} heap Heap object.
 */
function getImportObject(memory, heap) {
    var tempRet0 = 0;

    var setTempRet0 = function(value) {
    tempRet0 = value;
    };

    var getTempRet0 = function() {
    return tempRet0;
    };

    const length = heap.length;

    return {
        'global.Math': Math,
        global: {
            Infinity: 10 ** 1000
        },
        env: {
            abortStackOverflow: () => { throw new Error('overflow'); },
            __memory_base: 1024,
            __table_base: 0,
            memory,
            table: new WebAssembly.Table({ initial: 400, maximum: 400, element: 'anyfunc' }),
            STACKTOP: 0, //96304,
            STACK_MAX: memory.buffer.byteLength,
            ___syscall146: () => { },
            ___setErrNo: () => { },
            _abort: () => { throw new Error('abort'); },
            __emval_take_value: () => { },
            __emval_incref: () => { },
            __emval_decref: () => { },
            ___syscall6: () => { },
            ___syscall140: () => { },
            abortOnCannotGrowMemory: err => { throw new Error(err); },
            _llvm_trap: () => { console.log('llvm_trap'); },
            _llvm_exp2_f64: val => 2 ** val,
            ___cxa_pure_virtual: () => {
                throw new Error('___cxa_pure_virtual');
            },

            DYNAMICTOP_PTR: 8192,
            //DYNAMICTOP_PTR: 96096, //8192,
            abort: err => {
                throw new Error('abort ', err);
            },
            ___cxa_throw: (/* ptr, type, destructor */) => {
                throw new Error('___cxa_throw');
            },
            ___cxa_allocate_exception: (/* size */) => false, // always fail
            ___cxa_uncaught_exception: () => {
                throw new Error('___cxa_uncaught_exception');
            },
            ___cxa_uncaught_exceptions: () => {
                throw new Error('___cxa_uncaught_exceptions');
            },

            ___wasi_fd_write: () => {
                throw new Error('___wasi_fd_write');
            },

            ___wasi_fd_close: () => {
                throw new Error('___wasi_fd_close');
            },

            ___wasi_fd_seek: () => {
                throw new Error('___wasi_fd_seek');
            },

            //setTempRet0: () => 0,
            setTempRet0,
            getTempRet0,

            _emscripten_get_heap_size: () => length,
            _emscripten_resize_heap: (/* size */) => false, // always fail
            _emscripten_memcpy_big: (dest, src, count) => {
                heap.set(heap.subarray(src, src + count), dest);
            },

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

            nullFunc_jiji: () => console.log('nullFunc_jiji'),
            nullFunc_viij: () => console.log('nullFunc_viij'),
            nullFunc_iidiiii: () => console.log('nullFunc_iidiiii'),

            segfault: () => console.log('segfault'),
            alignfault: () => console.log('alignfault'),

            ___cxa_begin_catch: () => console.log('___cxa_begin_catch'),
            ___lock: () => console.log('___lock'),
            ___unlock: () => console.log('___unlock'),
            ___syscall54: () => { },
            _llvm_cos_f64: () => 0,
            _llvm_sin_f64: () => 0,
            tempDoublePtr: 0,
            //tempDoublePtr: 96288,
        }
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
