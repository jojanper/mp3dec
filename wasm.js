const fs = require('fs');


const WASMLIB = '/build-wasm/bin/mp3dec_static.wasm';

const memory = new WebAssembly.Memory({
    initial: 256,
    maximum: 256
});

const heap = new Uint8Array(memory.buffer);

const importObject = {
    global: {
        Infinity: Math.pow(10, 1000),
    },
    env: {
        'abortStackOverflow': _ => { throw new Error('overflow'); },
        '__memory_base': 1024,
        '__table_base': 0,
        'memory': memory,
        'table': new WebAssembly.Table({ initial: 160, maximum: 160, element: 'anyfunc' }),
        'STACKTOP': 0,
        'STACK_MAX': memory.buffer.byteLength,
        abort: () => { },
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
        },
        ___cxa_pure_virtual: () => { },

        memory: memory,
        DYNAMICTOP_PTR: 4096,
        abort: function (err) {
            throw new Error('abort ' + err);
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
        /*
        ___setErrNo: function (err) {
            throw new Error('ErrNo ' + err);
        },
        */
        _emscripten_get_heap_size: function () {
            return heap.length;
        },
        _emscripten_resize_heap: function (size) {
            return false; // always fail
        },
        _emscripten_memcpy_big: function (dest, src, count) {
            heap.set(heap.subarray(src, src + count), dest);
        },
        __table_base: 0,

        //DYNAMICTOP_PTR: 0,
    }
};

const wasmModule = new WebAssembly.Module(fs.readFileSync(__dirname + WASMLIB));
const instance = new WebAssembly.Instance(wasmModule, importObject);

function testExec(instance) {
    const { exports } = instance;
    console.log(exports);

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
}

testExec(instance);
