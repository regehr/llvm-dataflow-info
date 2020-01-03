# llvm-transformation-pass
Template for an out-of-tree LLVM pass.

## Requirements
 * LLVM 3.8
 * cmake 3.7

## Build
```
mkdir build
cd build
cmake ..
```

## Run

On Linux:
```
opt -load build/hello/LLVMHelloWorld.so -helloworld test/hello.bc
```

On OS X:
```
opt -load build/hello/LLVMHelloWorld.dylib -helloworld test/hello.bc
```
