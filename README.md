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
  - For WebAssembly building, tested with version 2.0.26

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
npm run decode -- --input=<input.mp3>  --output=<output.pcm>
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
- Goto http://localhost:4200/mp3dec_static.html on browser

- npm run js-beautify -- build-wasm/bin/mp3dec_static.js -r (unminify js file)
- node main.js --input=<file>.mp3 --output=test.raw (decode)
- ffplay -f s16le -ar 44k -ac 2 test.raw (play with ffplay)
