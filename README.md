# MP3 decoder and associated utilities
MP3 decoder for various build targets. Build targets include at the moment Linux/x64 and WebAssembly.

## Quickstart

### Prerequisites

- [Node.js](https://nodejs.org/en/)
  - Consider installing via [nvm](https://github.com/nvm-sh/nvm)
- [CMake](https://cmake.org/)
- [GCC](https://gcc.gnu.org/)
  - For Linux building
- [Emscripten](https://emscripten.org/index.html)
  - For WebAssembly building, tested with version 1.39.1

### Install dependencies
```
pip install gcovr
npm i
```

-----

### Decode MP3 file using Node + WebAssembly

Build WebAssembly target
```
npm run wasm-build
```

Execute in Node
```
node main.js --input=<input.mp3>  --output=<output.pcm>
```

-----

### Decode MP3 file using Linux/x64

Build target
```
npm run x64-build
```

Decode using command line binary
```
build/bin/mp3streamapp -stream <input.mp3> -out <output.wav> -wave-out
```

Run unit tests with memory checking enabled and generate code coverage report
```
npm run x64-build -- --type=Debug --folder=build-debug
npm run x64-tests -- --folder=build-debug --memcheck
npm run x64-coverage -- --folder=build-debug
```

-----

### Troubleshooting tips

- Run in Debug mode
- Add following line (example only) to `build-wasm/bin/mp3dec_static.html`
    - ```Module.onRuntimeInitialized=function() {console.log(Module); console.log(Module._openDecoder());}```
- Serve content using `npm run serve-build`
