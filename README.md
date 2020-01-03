# LLVM Dataflow Info Printer Pass

Tell us what some of LLVM's dataflow analyses think about the code
being compiled.

## Requirements
 * LLVM 9 or higher
 * cmake 3.7

## Build
```
mkdir build
cd build
cmake .. -G Ninja
ninja
```

## Run

On Linux, in the build directory:
```
opt -load ./LLVMDataflow.so -dataflow-info ../test/test1.ll
```

On OS X, in the build directory:
```
opt -load ./LLVMHelloWorld.dylib -dataflow-info ../test/test1.ll

```
