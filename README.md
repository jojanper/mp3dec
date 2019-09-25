# MP3 decoder and associated utilities
MP3 decoder for various build targets. Build targets include at the moment x64 Linux and WebAssembly.

## Quickstart

### Prerequisites

- [Node.js](https://nodejs.org/en/)
  - Consider installing via [nvm](https://github.com/nvm-sh/nvm)
- [CMake](https://cmake.org/)
- [GCC](https://gcc.gnu.org/)
  - For Linux building
- [Emscripten](https://emscripten.org/index.html)
  - For WebAssembly building, tested with version 1.38.45

### Install dependencies
```
pip install gcovr
npm i
```

### Decode MP3 file using Node + WebAssembly

Build WebAssembly target
```
npm run wasm-build
```

Execute in Node
```
node main.js --input=<input.mp3>  --output=<output.pcm>
```
