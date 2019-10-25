let tempRet0 = 0;

function setTempRet0(value) {
    tempRet0 = value;
}

function getTempRet0() {
    return tempRet0;
}

const wasmTable = new WebAssembly.Table({
    initial: 400,
    maximum: 400,
    element: 'anyfunc'
});

let HEAP8;
let HEAPU8;
let HEAP32;

const DYNAMIC_BASE = 5337456;
const DYNAMICTOP_PTR = 94384;

function updateGlobalBufferAndViews(buf) {
    HEAP8 = new Int8Array(buf);
    HEAP32 = new Int32Array(buf);
    HEAPU8 = new Uint8Array(buf);
    HEAP32[DYNAMICTOP_PTR / 4] = DYNAMIC_BASE;
}

const wasmMemory = new WebAssembly.Memory({
    initial: 256,
    maximum: 256
});

updateGlobalBufferAndViews(wasmMemory.buffer);

function abort(what) {
    throw new Error(`abort(${what}). Build with -s ASSERTIONS=1 for more info`);
}

function createWasm(asmLibraryArg) {
    return {
        env: asmLibraryArg,
        wasi_unstable: asmLibraryArg,
        global: {
            NaN,
            Infinity
        },
        'global.Math': Math,
    };
}

const asmLibraryArg = {
    ___cxa_begin_catch: () => { },
    ___exception_addRef: () => { },
    ___exception_deAdjust: () => { },
    ___gxx_personality_v0: () => { },

    __memory_base: 1024,
    __table_base: 0,
    memory: wasmMemory,
    table: wasmTable,
    tempDoublePtr: 94560,

    _emscripten_get_heap_size: () => HEAP8.length,
    _emscripten_memcpy_big: (dest, src, num) => HEAPU8.set(HEAPU8.subarray(src, src + num), dest),
    _emscripten_resize_heap: () => abort('emscripten_resize_heap'),

    _fd_close: () => { },
    fd_close: () => { },
    _fd_seek: () => { },
    fd_seek: () => { },
    _fd_write: () => { },
    fd_write: () => { },

    ___wasi_fd_close: () => { },
    ___wasi_fd_seek: () => { },
    ___wasi_fd_write: () => { },

    _llvm_exp2_f32: val => 2 ** val,
    _llvm_exp2_f64: val => 2 ** val,
    _llvm_trap: () => abort('trap'),

    abort,
    _abort: abort,
    getTempRet0,
    setTempRet0
};

module.exports = createWasm(asmLibraryArg);
