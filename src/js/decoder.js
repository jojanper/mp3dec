/**
 * JavaScript interface for accessing audio (e.g., MP3) decoder operating in WebAssembly context.
 * This interface wraps the underlying WebAssembly decoder API into easy-to-use JS class.
 */
class DraalDecoder {
    static create(api, memory) {
        return new DraalDecoder(api, memory).open();
    }

    /**
     * Constructor.
     *
     * @param {*} api Exported WebAssembly API for the audio decoder.
     * @param {*} memory Memory object to be used for data between JS and WebAssembly.
     */
    constructor(api, memory) {
        this.api = api;
        this.memory = api.memory;

        const heap = api.memory.buffer;
        this.HEAP32 = new Int32Array(heap);
        this.HEAPU8 = new Uint8Array(heap);

        memory.setHeap(this.HEAPU8);

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
        this.decoder = this.api.openDecoder();
        return this;
    }

    /**
     * Close decoder and related resource.
     */
    close() {
        this.api.destroyBuffer(this.wasmInputPtr);
        this.api.closeDecoder(this.decoder);

        this.decoder = null;
        this.wasmInputPtr = null;
        this.initialized = false;
    }

    /**
     * Decode input stream into audio samples.
     *
     * @param {*} dataChunk Audio bitstream data.
     * @param {*} outStreamApi Callback for decoded output samples.
     */
    decode(dataChunk, outStreamApi) {
        if (!this.initialized) {
            this.jsInput = new Uint8Array(dataChunk.length);
            this.wasmInputPtr = this.api.createBuffer(this.jsInput.length);
            this.wasmInput = new Uint8Array(this.memory.buffer, this.wasmInputPtr, this.jsInput.length);

            this.wasmInput.set(dataChunk);

            if (!this.decoder) {
                this.open();
            }

            const init = this.api.initDecoder(this.decoder, this.wasmInputPtr, this.jsInput.length);
            if (!init) {
                return false;
            }

            this.initialized = true;
        } else {
            this.wasmInput.set(dataChunk);
            this.api.addInput(this.decoder, this.wasmInputPtr, dataChunk.length);
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
            const result = this.api.decode(this.decoder);
            if (result) {
                const decPtr = this.api.getAudio(this.decoder);
                const nDecSamples = this.api.getAudioSize(this.decoder);
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
 * Memory object mapper for WebAssembly instance.
 */
function getMemory() {
    class MemObject {
        constructor() {
            this.HEAPU8 = null;
        }

        setHeap(heap) {
            this.HEAPU8 = heap;
        }
    }

    return new MemObject();
}

function abort(what) {
    throw new Error(`abort(${what}). Build with -s ASSERTIONS=1 for more info`);
}

/**
 * Return minimal object data to be imported for WebAssembly instance.
 *
 * @param {*} memObject WebAssembly memory object.
 */
function getImportObject(memObject) {
    const env = {
        emscripten_memcpy_big: (dest, src, num) => {
            memObject.HEAPU8.copyWithin(dest, src, src + num);
        },
        emscripten_resize_heap: () => abort('emscripten_resize_heap'),
        abort
    };

    return {
        env,
        wasi_snapshot_preview1: env
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
