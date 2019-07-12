# MP3 decoder and associated utilities
MP3 decoder for various build targets. Build targets include at the moment x64 Linux and WebAssembly.

## Quickstart

### Prerequisites

- [CMake](https://cmake.org/)
- [Emscripten](https://emscripten.org/index.html)

### Install dependencies
```
pip install gcovr
npm install
```

### Decode MP3 file using Node + WebAssembly

Build WebAssembly target
```
npm run wasm-build
```

Execute in Node
```
node wasm
```
