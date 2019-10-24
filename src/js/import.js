/*
var Module = typeof Module !== "undefined" ? Module : {};

var moduleOverrides = {};

var key;

for (key in Module) {
    if (Module.hasOwnProperty(key)) {
        moduleOverrides[key] = Module[key];
    }
}

var arguments_ = [];

var thisProgram = "./this.program";

var quit_ = function (status, toThrow) {
    throw toThrow;
};

var ENVIRONMENT_IS_WEB = false;

var ENVIRONMENT_IS_WORKER = false;

var ENVIRONMENT_IS_NODE = false;

var ENVIRONMENT_HAS_NODE = false;

var ENVIRONMENT_IS_SHELL = false;

ENVIRONMENT_IS_WEB = typeof window === "object";

ENVIRONMENT_IS_WORKER = typeof importScripts === "function";

ENVIRONMENT_HAS_NODE = typeof process === "object" &&
typeof process.versions === "object" && typeof process.versions.node === "string";

ENVIRONMENT_IS_NODE = ENVIRONMENT_HAS_NODE && !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_WORKER;

ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;

var scriptDirectory = "";

function locateFile(path) {
    if (Module["locateFile"]) {
        return Module["locateFile"](path, scriptDirectory);
    }
    return scriptDirectory + path;
}

var read_, readAsync, readBinary, setWindowTitle;

module["exports"] = Module;

//if (ENVIRONMENT_IS_NODE) {
/*
scriptDirectory = __dirname + "/";
var nodeFS;
var nodePath;
read_ = function shell_read(filename, binary) {
    var ret;
    if (!nodeFS) nodeFS = require("fs");
    if (!nodePath) nodePath = require("path");
    filename = nodePath["normalize"](filename);
    ret = nodeFS["readFileSync"](filename);
    return binary ? ret : ret.toString();
};
readBinary = function readBinary(filename) {
    var ret = read_(filename, true);
    if (!ret.buffer) {
        ret = new Uint8Array(ret);
    }
    assert(ret.buffer);
    return ret;
};
if (process["argv"].length > 1) {
    thisProgram = process["argv"][1].replace(/\\/g, "/");
}
arguments_ = process["argv"].slice(2);
*/
//if (typeof module !== "undefined") {
//  module["exports"] = Module;
//}
/*
process["on"]("uncaughtException", function (ex) {
    if (!(ex instanceof ExitStatus)) {
        throw ex;
    }
});
process["on"]("unhandledRejection", abort);
quit_ = function (status) {
    process["exit"](status);
};
Module["inspect"] = function () {
    return "[Emscripten Module object]";
};
*/
/*}*/ /*else if (ENVIRONMENT_IS_SHELL) {
    if (typeof read != "undefined") {
        read_ = function shell_read(f) {
            return read(f);
        };
    }
    readBinary = function readBinary(f) {
        var data;
        if (typeof readbuffer === "function") {
            return new Uint8Array(readbuffer(f));
        }
        data = read(f, "binary");
        assert(typeof data === "object");
        return data;
    };
    if (typeof scriptArgs != "undefined") {
        arguments_ = scriptArgs;
    } else if (typeof arguments != "undefined") {
        arguments_ = arguments;
    }
    if (typeof quit === "function") {
        quit_ = function (status) {
            quit(status);
        };
    }
    if (typeof print !== "undefined") {
        if (typeof console === "undefined") console = {};
        console.log = print;
        console.warn = console.error = typeof printErr !== "undefined" ? printErr : print;
    }
} else if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
    if (ENVIRONMENT_IS_WORKER) {
        scriptDirectory = self.location.href;
    } else if (document.currentScript) {
        scriptDirectory = document.currentScript.src;
    }
    if (scriptDirectory.indexOf("blob:") !== 0) {
        scriptDirectory = scriptDirectory.substr(0, scriptDirectory.lastIndexOf("/") + 1);
    } else {
        scriptDirectory = "";
    }
    read_ = function shell_read(url) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", url, false);
        xhr.send(null);
        return xhr.responseText;
    };
    if (ENVIRONMENT_IS_WORKER) {
        readBinary = function readBinary(url) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", url, false);
            xhr.responseType = "arraybuffer";
            xhr.send(null);
            return new Uint8Array(xhr.response);
        };
    }
    readAsync = function readAsync(url, onload, onerror) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", url, true);
        xhr.responseType = "arraybuffer";
        xhr.onload = function xhr_onload() {
            if (xhr.status == 200 || xhr.status == 0 && xhr.response) {
                onload(xhr.response);
                return;
            }
            onerror();
        };
        xhr.onerror = onerror;
        xhr.send(null);
    };
    setWindowTitle = function (title) {
        document.title = title;
    };
} else { }
*/

var Module = {};
//module["exports"] = Module;

//var out = Module["print"] || console.log.bind(console);

//var err = Module["printErr"] || console.warn.bind(console);

/*
for (key in moduleOverrides) {
    if (moduleOverrides.hasOwnProperty(key)) {
        Module[key] = moduleOverrides[key];
    }
}

moduleOverrides = null;
*/

//if (Module["arguments"]) arguments_ = Module["arguments"];

//if (Module["thisProgram"]) thisProgram = Module["thisProgram"];

//if (Module["quit"]) quit_ = Module["quit"];

//var STACK_ALIGN = 16;

/*
function dynamicAlloc(size) {
    var ret = HEAP32[DYNAMICTOP_PTR >> 2];
    var end = ret + size + 15 & -16;
    if (end > _emscripten_get_heap_size()) {
        abort();
    }
    HEAP32[DYNAMICTOP_PTR >> 2] = end;
    return ret;
}
*/

/*
function getNativeTypeSize(type) {
    switch (type) {
        case "i1":
        case "i8":
            return 1;

        case "i16":
            return 2;

        case "i32":
            return 4;

        case "i64":
            return 8;

        case "float":
            return 4;

        case "double":
            return 8;

        default:
            {
                if (type[type.length - 1] === "*") {
                    return 4;
                } else if (type[0] === "i") {
                    var bits = parseInt(type.substr(1));
                    assert(bits % 8 === 0, "getNativeTypeSize invalid bits " + bits + ", type " + type);
                    return bits / 8;
                } else {
                    return 0;
                }
            }
    }
}
*/

/*
function warnOnce(text) {
    if (!warnOnce.shown) warnOnce.shown = {};
    if (!warnOnce.shown[text]) {
        warnOnce.shown[text] = 1;
        err(text);
    }
}
*/

/*
var asm2wasmImports = {
    "f64-rem": function (x, y) {
        return x % y;
    },
    "debugger": function () { }
};
*/

//var jsCallStartIndex = 1;

//var functionPointers = new Array(0);

/*
function convertJsFunctionToWasm(func, sig) {
    var typeSection = [1, 0, 1, 96];
    var sigRet = sig.slice(0, 1);
    var sigParam = sig.slice(1);
    var typeCodes = {
        "i": 127,
        "j": 126,
        "f": 125,
        "d": 124
    };
    typeSection.push(sigParam.length);
    for (var i = 0; i < sigParam.length; ++i) {
        typeSection.push(typeCodes[sigParam[i]]);
    }
    if (sigRet == "v") {
        typeSection.push(0);
    } else {
        typeSection = typeSection.concat([1, typeCodes[sigRet]]);
    }
    typeSection[1] = typeSection.length - 2;
    var bytes = new Uint8Array([0, 97, 115, 109, 1, 0, 0, 0].concat(typeSection, [2, 7, 1, 1, 101, 1, 102, 0, 0, 7, 5, 1, 1, 102, 0, 0]));
    var module = new WebAssembly.Module(bytes);
    var instance = new WebAssembly.Instance(module, {
        e: {
            f: func
        }
    });
    var wrappedFunc = instance.exports.f;
    return wrappedFunc;
}
*/

/*
var funcWrappers = {};

function dynCall(sig, ptr, args) {
    if (args && args.length) {
        return Module["dynCall_" + sig].apply(null, [ptr].concat(args));
    } else {
        return Module["dynCall_" + sig].call(null, ptr);
    }
}
*/

var tempRet0 = 0;

var setTempRet0 = function (value) {
    tempRet0 = value;
};

var getTempRet0 = function () {
    return tempRet0;
};

var wasmBinary;

if (Module["wasmBinary"]) wasmBinary = Module["wasmBinary"];

//var noExitRuntime;

//if (Module["noExitRuntime"]) noExitRuntime = Module["noExitRuntime"];

/*
if (typeof WebAssembly !== "object") {
    err("no native wasm support detected");
}

function setValue(ptr, value, type, noSafe) {
    type = type || "i8";
    if (type.charAt(type.length - 1) === "*") type = "i32";
    switch (type) {
        case "i1":
            HEAP8[ptr >> 0] = value;
            break;

        case "i8":
            HEAP8[ptr >> 0] = value;
            break;

        case "i16":
            HEAP16[ptr >> 1] = value;
            break;

        case "i32":
            HEAP32[ptr >> 2] = value;
            break;

        case "i64":
            tempI64 = [value >>> 0, (tempDouble = value, +Math_abs(tempDouble) >= 1 ? tempDouble > 0 ? (Math_min(+Math_floor(tempDouble / 4294967296), 4294967295) | 0) >>> 0 : ~~+Math_ceil((tempDouble - +(~~tempDouble >>> 0)) / 4294967296) >>> 0 : 0)],
                HEAP32[ptr >> 2] = tempI64[0], HEAP32[ptr + 4 >> 2] = tempI64[1];
            break;

        case "float":
            HEAPF32[ptr >> 2] = value;
            break;

        case "double":
            HEAPF64[ptr >> 3] = value;
            break;

        default:
            abort("invalid type for setValue: " + type);
    }
}
*/

var wasmMemory;

var wasmTable = new WebAssembly.Table({
    "initial": 400,
    "maximum": 400,
    "element": "anyfunc"
});

//var ABORT = false;

//var EXITSTATUS = 0;

/*
function assert(condition, text) {
    if (!condition) {
        abort("Assertion failed: " + text);
    }
}

function getCFunc(ident) {
    var func = Module["_" + ident];
    assert(func, "Cannot call unknown function " + ident + ", make sure it is exported");
    return func;
}

function ccall(ident, returnType, argTypes, args, opts) {
    var toC = {
        "string": function (str) {
            var ret = 0;
            if (str !== null && str !== undefined && str !== 0) {
                var len = (str.length << 2) + 1;
                ret = stackAlloc(len);
                stringToUTF8(str, ret, len);
            }
            return ret;
        },
        "array": function (arr) {
            var ret = stackAlloc(arr.length);
            writeArrayToMemory(arr, ret);
            return ret;
        }
    };
    function convertReturnValue(ret) {
        if (returnType === "string") return UTF8ToString(ret);
        if (returnType === "boolean") return Boolean(ret);
        return ret;
    }
    var func = getCFunc(ident);
    var cArgs = [];
    var stack = 0;
    if (args) {
        for (var i = 0; i < args.length; i++) {
            var converter = toC[argTypes[i]];
            if (converter) {
                if (stack === 0) stack = stackSave();
                cArgs[i] = converter(args[i]);
            } else {
                cArgs[i] = args[i];
            }
        }
    }
    var ret = func.apply(null, cArgs);
    ret = convertReturnValue(ret);
    if (stack !== 0) stackRestore(stack);
    return ret;
}
*/

/*
var ALLOC_NONE = 3;

var UTF8Decoder = typeof TextDecoder !== "undefined" ? new TextDecoder("utf8") : undefined;

function UTF8ArrayToString(u8Array, idx, maxBytesToRead) {
    var endIdx = idx + maxBytesToRead;
    var endPtr = idx;
    while (u8Array[endPtr] && !(endPtr >= endIdx))++endPtr;
    if (endPtr - idx > 16 && u8Array.subarray && UTF8Decoder) {
        return UTF8Decoder.decode(u8Array.subarray(idx, endPtr));
    } else {
        var str = "";
        while (idx < endPtr) {
            var u0 = u8Array[idx++];
            if (!(u0 & 128)) {
                str += String.fromCharCode(u0);
                continue;
            }
            var u1 = u8Array[idx++] & 63;
            if ((u0 & 224) == 192) {
                str += String.fromCharCode((u0 & 31) << 6 | u1);
                continue;
            }
            var u2 = u8Array[idx++] & 63;
            if ((u0 & 240) == 224) {
                u0 = (u0 & 15) << 12 | u1 << 6 | u2;
            } else {
                u0 = (u0 & 7) << 18 | u1 << 12 | u2 << 6 | u8Array[idx++] & 63;
            }
            if (u0 < 65536) {
                str += String.fromCharCode(u0);
            } else {
                var ch = u0 - 65536;
                str += String.fromCharCode(55296 | ch >> 10, 56320 | ch & 1023);
            }
        }
    }
    return str;
}

function UTF8ToString(ptr, maxBytesToRead) {
    return ptr ? UTF8ArrayToString(HEAPU8, ptr, maxBytesToRead) : "";
}

function stringToUTF8Array(str, outU8Array, outIdx, maxBytesToWrite) {
    if (!(maxBytesToWrite > 0)) return 0;
    var startIdx = outIdx;
    var endIdx = outIdx + maxBytesToWrite - 1;
    for (var i = 0; i < str.length; ++i) {
        var u = str.charCodeAt(i);
        if (u >= 55296 && u <= 57343) {
            var u1 = str.charCodeAt(++i);
            u = 65536 + ((u & 1023) << 10) | u1 & 1023;
        }
        if (u <= 127) {
            if (outIdx >= endIdx) break;
            outU8Array[outIdx++] = u;
        } else if (u <= 2047) {
            if (outIdx + 1 >= endIdx) break;
            outU8Array[outIdx++] = 192 | u >> 6;
            outU8Array[outIdx++] = 128 | u & 63;
        } else if (u <= 65535) {
            if (outIdx + 2 >= endIdx) break;
            outU8Array[outIdx++] = 224 | u >> 12;
            outU8Array[outIdx++] = 128 | u >> 6 & 63;
            outU8Array[outIdx++] = 128 | u & 63;
        } else {
            if (outIdx + 3 >= endIdx) break;
            outU8Array[outIdx++] = 240 | u >> 18;
            outU8Array[outIdx++] = 128 | u >> 12 & 63;
            outU8Array[outIdx++] = 128 | u >> 6 & 63;
            outU8Array[outIdx++] = 128 | u & 63;
        }
    }
    outU8Array[outIdx] = 0;
    return outIdx - startIdx;
}
*/

/*
function stringToUTF8(str, outPtr, maxBytesToWrite) {
    return stringToUTF8Array(str, HEAPU8, outPtr, maxBytesToWrite);
}

function lengthBytesUTF8(str) {
    var len = 0;
    for (var i = 0; i < str.length; ++i) {
        var u = str.charCodeAt(i);
        if (u >= 55296 && u <= 57343) u = 65536 + ((u & 1023) << 10) | str.charCodeAt(++i) & 1023;
        if (u <= 127)++len; else if (u <= 2047) len += 2; else if (u <= 65535) len += 3; else len += 4;
    }
    return len;
}
*/

/*
var UTF16Decoder = typeof TextDecoder !== "undefined" ? new TextDecoder("utf-16le") : undefined;

function writeArrayToMemory(array, buffer) {
    HEAP8.set(array, buffer);
}

function writeAsciiToMemory(str, buffer, dontAddNull) {
    for (var i = 0; i < str.length; ++i) {
        HEAP8[buffer++ >> 0] = str.charCodeAt(i);
    }
    if (!dontAddNull) HEAP8[buffer >> 0] = 0;
}
*/

//var WASM_PAGE_SIZE = 65536;

var buffer, HEAP8, HEAPU8, HEAP32; //, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;

// var STACK_BASE = 94576,

var DYNAMIC_BASE = 5337456, DYNAMICTOP_PTR = 94384;

function updateGlobalBufferAndViews(buf) {
    //buffer = buf;
    /*Module["HEAP8"] = */HEAP8 = new Int8Array(buf);
    //Module["HEAP16"] = HEAP16 = new Int16Array(buf);
    /*Module["HEAP32"] =*/ HEAP32 = new Int32Array(buf);
    /*Module["HEAPU8"] =*/ HEAPU8 = new Uint8Array(buf);
    //Module["HEAPU16"] = HEAPU16 = new Uint16Array(buf);
    //Module["HEAPU32"] = HEAPU32 = new Uint32Array(buf);
    //Module["HEAPF32"] = HEAPF32 = new Float32Array(buf);
    //Module["HEAPF64"] = HEAPF64 = new Float64Array(buf);
    HEAP32[DYNAMICTOP_PTR >> 2] = DYNAMIC_BASE;
}

//var INITIAL_TOTAL_MEMORY = Module["TOTAL_MEMORY"] || 16777216;

/*
if (Module["wasmMemory"]) {
    wasmMemory = Module["wasmMemory"];
} else {
    */
wasmMemory = new WebAssembly.Memory({
    "initial": 256, // INITIAL_TOTAL_MEMORY / WASM_PAGE_SIZE,
    "maximum": 256 // INITIAL_TOTAL_MEMORY / WASM_PAGE_SIZE
});
/*
}

if (wasmMemory) {
*/
buffer = wasmMemory.buffer;

/*
}
*/

//INITIAL_TOTAL_MEMORY = buffer.byteLength;

updateGlobalBufferAndViews(buffer);

//HEAP32[DYNAMICTOP_PTR >> 2] = DYNAMIC_BASE;

/*
function callRuntimeCallbacks(callbacks) {
    while (callbacks.length > 0) {
        var callback = callbacks.shift();
        if (typeof callback == "function") {
            callback();
            continue;
        }
        var func = callback.func;
        if (typeof func === "number") {
            if (callback.arg === undefined) {
                Module["dynCall_v"](func);
            } else {
                Module["dynCall_vi"](func, callback.arg);
            }
        } else {
            func(callback.arg === undefined ? null : callback.arg);
        }
    }
}
*/

/*
var __ATPRERUN__ = [];

var __ATINIT__ = [];

var __ATMAIN__ = [];

var __ATPOSTRUN__ = [];

var runtimeInitialized = false;

var runtimeExited = false;

function preRun() {
    if (Module["preRun"]) {
        if (typeof Module["preRun"] == "function") Module["preRun"] = [Module["preRun"]];
        while (Module["preRun"].length) {
            addOnPreRun(Module["preRun"].shift());
        }
    }
    callRuntimeCallbacks(__ATPRERUN__);
}

function initRuntime() {
    runtimeInitialized = true;
    callRuntimeCallbacks(__ATINIT__);
}

function preMain() {
    callRuntimeCallbacks(__ATMAIN__);
}

function exitRuntime() {
    runtimeExited = true;
}

function postRun() {
    if (Module["postRun"]) {
        if (typeof Module["postRun"] == "function") Module["postRun"] = [Module["postRun"]];
        while (Module["postRun"].length) {
            addOnPostRun(Module["postRun"].shift());
        }
    }
    callRuntimeCallbacks(__ATPOSTRUN__);
}

function addOnPreRun(cb) {
    __ATPRERUN__.unshift(cb);
}

function addOnPostRun(cb) {
    __ATPOSTRUN__.unshift(cb);
}
*/

/*
var Math_abs = Math.abs;

var Math_ceil = Math.ceil;

var Math_floor = Math.floor;

var Math_min = Math.min;
*/

/*
var runDependencies = 0;

var runDependencyWatcher = null;

var dependenciesFulfilled = null;

function addRunDependency(id) {
    runDependencies++;
    if (Module["monitorRunDependencies"]) {
        Module["monitorRunDependencies"](runDependencies);
    }
}

function removeRunDependency(id) {
    runDependencies--;
    if (Module["monitorRunDependencies"]) {
        Module["monitorRunDependencies"](runDependencies);
    }
    if (runDependencies == 0) {
        if (runDependencyWatcher !== null) {
            clearInterval(runDependencyWatcher);
            runDependencyWatcher = null;
        }
        if (dependenciesFulfilled) {
            var callback = dependenciesFulfilled;
            dependenciesFulfilled = null;
            callback();
        }
    }
}

Module["preloadedImages"] = {};

Module["preloadedAudios"] = {};
*/

function abort(what) {
    /*
    if (Module["onAbort"]) {
        Module["onAbort"](what);
    }
    */
    //what += "";
    //out(what);
    //err(what);
    //ABORT = true;
    //EXITSTATUS = 1;
    throw new Error("abort(" + what + "). Build with -s ASSERTIONS=1 for more info");
}

/*
var dataURIPrefix = "data:application/octet-stream;base64,";

function isDataURI(filename) {
    return String.prototype.startsWith ? filename.startsWith(dataURIPrefix) : filename.indexOf(dataURIPrefix) === 0;
}
*/

/*
var wasmBinaryFile = "mp3dec_static.wasm";

if (!isDataURI(wasmBinaryFile)) {
    wasmBinaryFile = locateFile(wasmBinaryFile);
}
*/

/*
function getBinary() {
    try {
        if (wasmBinary) {
            return new Uint8Array(wasmBinary);
        }
        if (readBinary) {
            return readBinary(wasmBinaryFile);
        } else {
            throw "both async and sync fetching of the wasm failed";
        }
    } catch (err) {
        abort(err);
    }
}

function getBinaryPromise() {
    if (!wasmBinary && (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) && typeof fetch === "function") {
        return fetch(wasmBinaryFile, {
            credentials: "same-origin"
        }).then(function (response) {
            if (!response["ok"]) {
                throw "failed to load wasm binary file at '" + wasmBinaryFile + "'";
            }
            return response["arrayBuffer"]();
        }).catch(function () {
            return getBinary();
        });
    }
    return new Promise(function (resolve, reject) {
        resolve(getBinary());
    });
}
*/

function createWasm() {
    var info = {
        env: asmLibraryArg,
        wasi_unstable: asmLibraryArg,
        global: {
            NaN: NaN,
            Infinity: Infinity
        },
        'global.Math': Math,
        //"asm2wasm": asm2wasmImports
    };

    return info;
    /*
    function receiveInstance(instance, module) {
     var exports = instance.exports;
     Module["asm"] = exports;
     removeRunDependency("wasm-instantiate");
    }
    addRunDependency("wasm-instantiate");
    function receiveInstantiatedSource(output) {
     receiveInstance(output["instance"]);
    }
    function instantiateArrayBuffer(receiver) {
     return getBinaryPromise().then(function(binary) {
      return WebAssembly.instantiate(binary, info);
     }).then(receiver, function(reason) {
      err("failed to asynchronously prepare wasm: " + reason);
      abort(reason);
     });
    }
    function instantiateAsync() {
     if (!wasmBinary && typeof WebAssembly.instantiateStreaming === "function" && !isDataURI(wasmBinaryFile) && typeof fetch === "function") {
      fetch(wasmBinaryFile, {
       credentials: "same-origin"
      }).then(function(response) {
       var result = WebAssembly.instantiateStreaming(response, info);
       return result.then(receiveInstantiatedSource, function(reason) {
        err("wasm streaming compile failed: " + reason);
        err("falling back to ArrayBuffer instantiation");
        instantiateArrayBuffer(receiveInstantiatedSource);
       });
      });
     } else {
      return instantiateArrayBuffer(receiveInstantiatedSource);
     }
    }
    if (Module["instantiateWasm"]) {
     try {
      var exports = Module["instantiateWasm"](info, receiveInstance);
      return exports;
     } catch (e) {
      err("Module.instantiateWasm callback failed with error: " + e);
      return false;
     }
    }
    instantiateAsync();
    return {};
    */
}

Module["asm"] = createWasm;

//var tempDouble;

//var tempI64;

var tempDoublePtr = 94560;

/*
function demangle(func) {
    return func;
}

function demangleAll(text) {
    var regex = /\b__Z[\w\d_]+/g;
    return text.replace(regex, function (x) {
        var y = demangle(x);
        return x === y ? x : y + " [" + x + "]";
    });
}

function jsStackTrace() {
    var err = new Error();
    if (!err.stack) {
        try {
            throw new Error(0);
        } catch (e) {
            err = e;
        }
        if (!err.stack) {
            return "(no stack trace available)";
        }
    }
    return err.stack.toString();
}

function stackTrace() {
    var js = jsStackTrace();
    if (Module["extraStackTrace"]) js += "\n" + Module["extraStackTrace"]();
    return demangleAll(js);
}
*/

//var ___exception_infos = {};

//var ___exception_caught = [];

function ___exception_addRef(ptr) {
    /*
    if (!ptr) return;
    var info = ___exception_infos[ptr];
    info.refcount++;
    */
}

function ___exception_deAdjust(adjusted) {
    /*
    if (!adjusted || ___exception_infos[adjusted]) return adjusted;
    for (var key in ___exception_infos) {
        var ptr = +key;
        var adj = ___exception_infos[ptr].adjusted;
        var len = adj.length;
        for (var i = 0; i < len; i++) {
            if (adj[i] === adjusted) {
                return ptr;
            }
        }
    }
    */
    return adjusted;
}

function ___cxa_begin_catch(ptr) {
    /*
    var info = ___exception_infos[ptr];
    if (info && !info.caught) {
        info.caught = true;
        __ZSt18uncaught_exceptionv.uncaught_exceptions--;
    }
    if (info) info.rethrown = false;
    ___exception_caught.push(ptr);
    ___exception_addRef(___exception_deAdjust(ptr));
    */
    return ptr;
}

function ___gxx_personality_v0() { }

/*
var PATH = {
    splitPath: function (filename) {
        var splitPathRe = /^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/;
        return splitPathRe.exec(filename).slice(1);
    },
    normalizeArray: function (parts, allowAboveRoot) {
        var up = 0;
        for (var i = parts.length - 1; i >= 0; i--) {
            var last = parts[i];
            if (last === ".") {
                parts.splice(i, 1);
            } else if (last === "..") {
                parts.splice(i, 1);
                up++;
            } else if (up) {
                parts.splice(i, 1);
                up--;
            }
        }
        if (allowAboveRoot) {
            for (; up; up--) {
                parts.unshift("..");
            }
        }
        return parts;
    },
    normalize: function (path) {
        var isAbsolute = path.charAt(0) === "/", trailingSlash = path.substr(-1) === "/";
        path = PATH.normalizeArray(path.split("/").filter(function (p) {
            return !!p;
        }), !isAbsolute).join("/");
        if (!path && !isAbsolute) {
            path = ".";
        }
        if (path && trailingSlash) {
            path += "/";
        }
        return (isAbsolute ? "/" : "") + path;
    },
    dirname: function (path) {
        var result = PATH.splitPath(path), root = result[0], dir = result[1];
        if (!root && !dir) {
            return ".";
        }
        if (dir) {
            dir = dir.substr(0, dir.length - 1);
        }
        return root + dir;
    },
    basename: function (path) {
        if (path === "/") return "/";
        var lastSlash = path.lastIndexOf("/");
        if (lastSlash === -1) return path;
        return path.substr(lastSlash + 1);
    },
    extname: function (path) {
        return PATH.splitPath(path)[3];
    },
    join: function () {
        var paths = Array.prototype.slice.call(arguments, 0);
        return PATH.normalize(paths.join("/"));
    },
    join2: function (l, r) {
        return PATH.normalize(l + "/" + r);
    }
};
*/

/*
var SYSCALLS = {
    buffers: [null, [], []],
    printChar: function (stream, curr) {
        var buffer = SYSCALLS.buffers[stream];
        if (curr === 0 || curr === 10) {
            (stream === 1 ? out : err)(UTF8ArrayToString(buffer, 0));
            buffer.length = 0;
        } else {
            buffer.push(curr);
        }
    },
    varargs: 0,
    get: function (varargs) {
        SYSCALLS.varargs += 4;
        var ret = HEAP32[SYSCALLS.varargs - 4 >> 2];
        return ret;
    },
    getStr: function () {
        var ret = UTF8ToString(SYSCALLS.get());
        return ret;
    },
    get64: function () {
        var low = SYSCALLS.get(), high = SYSCALLS.get();
        return low;
    },
    getZero: function () {
        SYSCALLS.get();
    }
};
*/

function _fd_close(fd) {
    /*
    try {
        return 0;
    } catch (e) {
        if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
        return e.errno;
    }
    */
}

function ___wasi_fd_close() {
    //return _fd_close.apply(null, arguments);
}

function _fd_seek(fd, offset_low, offset_high, whence, newOffset) {
    /*
    try {
        return 0;
    } catch (e) {
        if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
        return e.errno;
    }
    */
}

function ___wasi_fd_seek() {
    //return _fd_seek.apply(null, arguments);
}

/*
function flush_NO_FILESYSTEM() {
    var fflush = Module["_fflush"];
    if (fflush) fflush(0);
    var buffers = SYSCALLS.buffers;
    if (buffers[1].length) SYSCALLS.printChar(1, 10);
    if (buffers[2].length) SYSCALLS.printChar(2, 10);
}
*/

function _fd_write(fd, iov, iovcnt, pnum) {
    /*
    try {
        var num = 0;
        for (var i = 0; i < iovcnt; i++) {
            var ptr = HEAP32[iov + i * 8 >> 2];
            var len = HEAP32[iov + (i * 8 + 4) >> 2];
            for (var j = 0; j < len; j++) {
                SYSCALLS.printChar(fd, HEAPU8[ptr + j]);
            }
            num += len;
        }
        HEAP32[pnum >> 2] = num;
        return 0;
    } catch (e) {
        if (typeof FS === "undefined" || !(e instanceof FS.ErrnoError)) abort(e);
        return e.errno;
    }
    */
}

function ___wasi_fd_write() {
    //return _fd_write.apply(null, arguments);
}

function _abort() {
    abort();
}

function _emscripten_get_heap_size() {
    return HEAP8.length;
}

function abortOnCannotGrowMemory(requestedSize) {
    abort("OOM");
}

function _emscripten_resize_heap(requestedSize) {
    abortOnCannotGrowMemory(requestedSize);
}

function _llvm_exp2_f32(x) {
    return Math.pow(2, x);
}

function _llvm_exp2_f64(a0) {
    return _llvm_exp2_f32(a0);
}

function _llvm_trap() {
    abort("trap!");
}

function _emscripten_memcpy_big(dest, src, num) {
    HEAPU8.set(HEAPU8.subarray(src, src + num), dest);
}

//var ASSERTIONS = false;

var asmGlobalArg = {};

var asmLibraryArg = {
    __cxa_pure_virtual: () => {},
    __cxa_allocate_exception: () => {},
    __cxa_throw: () => {},
    __cxa_uncaught_exceptions: () => {},
    sbrk: () => {},
    ___cxa_begin_catch,
    ___exception_addRef,
    ___exception_deAdjust,
    ___gxx_personality_v0,
    ___wasi_fd_close,
    ___wasi_fd_seek,
    ___wasi_fd_write,
    __memory_base: 1024,
    __table_base: 0,
    _abort,
    _emscripten_get_heap_size,
    _emscripten_memcpy_big,
    emscripten_memcpy_big: _emscripten_memcpy_big,
    emscripten_resize_heap: _emscripten_resize_heap,
    _fd_close,
    fd_close: _fd_close,
    _fd_seek,
    fd_seek: _fd_seek,
    _fd_write,
    fd_write: _fd_write,
    _llvm_exp2_f32,
    _llvm_exp2_f64,
    _llvm_trap,
    abort,
    abortOnCannotGrowMemory,
    getTempRet0,
    memory: wasmMemory,
    setTempRet0,
    table: wasmTable,
    tempDoublePtr
};

//const asm = Module["asm"](asmGlobalArg, asmLibraryArg, buffer);
//module.export = asm;

Module["asm"] = Module["asm"](asmGlobalArg, asmLibraryArg, buffer);;
module.exports = Module["asm"];

/*
var __ZSt18uncaught_exceptionv = Module["__ZSt18uncaught_exceptionv"] = function () {
    return Module["asm"]["__ZSt18uncaught_exceptionv"].apply(null, arguments);
};

var ___cxa_can_catch = Module["___cxa_can_catch"] = function () {
    return Module["asm"]["___cxa_can_catch"].apply(null, arguments);
};

var ___cxa_is_pointer_type = Module["___cxa_is_pointer_type"] = function () {
    return Module["asm"]["___cxa_is_pointer_type"].apply(null, arguments);
};

var ___errno_location = Module["___errno_location"] = function () {
    return Module["asm"]["___errno_location"].apply(null, arguments);
};

var _addInput = Module["_addInput"] = function () {
    return Module["asm"]["_addInput"].apply(null, arguments);
};

var _closeDecoder = Module["_closeDecoder"] = function () {
    return Module["asm"]["_closeDecoder"].apply(null, arguments);
};

var _createBuffer = Module["_createBuffer"] = function () {
    return Module["asm"]["_createBuffer"].apply(null, arguments);
};

var _decode = Module["_decode"] = function () {
    return Module["asm"]["_decode"].apply(null, arguments);
};

var _destroyBuffer = Module["_destroyBuffer"] = function () {
    return Module["asm"]["_destroyBuffer"].apply(null, arguments);
};

var _emscripten_get_sbrk_ptr = Module["_emscripten_get_sbrk_ptr"] = function () {
    return Module["asm"]["_emscripten_get_sbrk_ptr"].apply(null, arguments);
};

var _free = Module["_free"] = function () {
    return Module["asm"]["_free"].apply(null, arguments);
};

var _getAudio = Module["_getAudio"] = function () {
    return Module["asm"]["_getAudio"].apply(null, arguments);
};

var _getAudioSize = Module["_getAudioSize"] = function () {
    return Module["asm"]["_getAudioSize"].apply(null, arguments);
};

var _initDecoder = Module["_initDecoder"] = function () {
    return Module["asm"]["_initDecoder"].apply(null, arguments);
};

var _malloc = Module["_malloc"] = function () {
    return Module["asm"]["_malloc"].apply(null, arguments);
};

var _memcpy = Module["_memcpy"] = function () {
    return Module["asm"]["_memcpy"].apply(null, arguments);
};

var _memmove = Module["_memmove"] = function () {
    return Module["asm"]["_memmove"].apply(null, arguments);
};

var _memset = Module["_memset"] = function () {
    return Module["asm"]["_memset"].apply(null, arguments);
};

var _openDecoder = Module["_openDecoder"] = function () {
    return Module["asm"]["_openDecoder"].apply(null, arguments);
};

var establishStackSpace = Module["establishStackSpace"] = function () {
    return Module["asm"]["establishStackSpace"].apply(null, arguments);
};

var stackAlloc = Module["stackAlloc"] = function () {
    return Module["asm"]["stackAlloc"].apply(null, arguments);
};

var stackRestore = Module["stackRestore"] = function () {
    return Module["asm"]["stackRestore"].apply(null, arguments);
};

var stackSave = Module["stackSave"] = function () {
    return Module["asm"]["stackSave"].apply(null, arguments);
};

var dynCall_ii = Module["dynCall_ii"] = function () {
    return Module["asm"]["dynCall_ii"].apply(null, arguments);
};

var dynCall_iidiiii = Module["dynCall_iidiiii"] = function () {
    return Module["asm"]["dynCall_iidiiii"].apply(null, arguments);
};

var dynCall_iii = Module["dynCall_iii"] = function () {
    return Module["asm"]["dynCall_iii"].apply(null, arguments);
};

var dynCall_iiii = Module["dynCall_iiii"] = function () {
    return Module["asm"]["dynCall_iiii"].apply(null, arguments);
};

var dynCall_iiiii = Module["dynCall_iiiii"] = function () {
    return Module["asm"]["dynCall_iiiii"].apply(null, arguments);
};

var dynCall_jiji = Module["dynCall_jiji"] = function () {
    return Module["asm"]["dynCall_jiji"].apply(null, arguments);
};

var dynCall_v = Module["dynCall_v"] = function () {
    return Module["asm"]["dynCall_v"].apply(null, arguments);
};

var dynCall_vi = Module["dynCall_vi"] = function () {
    return Module["asm"]["dynCall_vi"].apply(null, arguments);
};

var dynCall_vii = Module["dynCall_vii"] = function () {
    return Module["asm"]["dynCall_vii"].apply(null, arguments);
};

var dynCall_viii = Module["dynCall_viii"] = function () {
    return Module["asm"]["dynCall_viii"].apply(null, arguments);
};

var dynCall_viiii = Module["dynCall_viiii"] = function () {
    return Module["asm"]["dynCall_viiii"].apply(null, arguments);
};

var dynCall_viiiii = Module["dynCall_viiiii"] = function () {
    return Module["asm"]["dynCall_viiiii"].apply(null, arguments);
};

var dynCall_viiiiii = Module["dynCall_viiiiii"] = function () {
    return Module["asm"]["dynCall_viiiiii"].apply(null, arguments);
};

var dynCall_viij = Module["dynCall_viij"] = function () {
    return Module["asm"]["dynCall_viij"].apply(null, arguments);
};

Module["asm"] = asm;
*/

/*
var calledRun;

function ExitStatus(status) {
    this.name = "ExitStatus";
    this.message = "Program terminated with exit(" + status + ")";
    this.status = status;
}

dependenciesFulfilled = function runCaller() {
    if (!calledRun) run();
    if (!calledRun) dependenciesFulfilled = runCaller;
};

/*
function run(args) {
    args = args || arguments_;
    if (runDependencies > 0) {
        return;
    }
    preRun();
    if (runDependencies > 0) return;
    function doRun() {
        if (calledRun) return;
        calledRun = true;
        if (ABORT) return;
        initRuntime();
        preMain();
        if (Module["onRuntimeInitialized"]) Module["onRuntimeInitialized"]();
        postRun();
    }
    if (Module["setStatus"]) {
        Module["setStatus"]("Running...");
        setTimeout(function () {
            setTimeout(function () {
                Module["setStatus"]("");
            }, 1);
            doRun();
        }, 1);
    } else {
        doRun();
    }
}

Module["run"] = run;

if (Module["preInit"]) {
    if (typeof Module["preInit"] == "function") Module["preInit"] = [Module["preInit"]];
    while (Module["preInit"].length > 0) {
        Module["preInit"].pop()();
    }
}

noExitRuntime = true;
*/

//run();
//module.export = Module["asm"];
