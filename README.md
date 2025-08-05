# LLVM Passes – Compiler Construction Project

## Overview

This project was developed as part of the *Compiler Construction* course and includes the implementation of two custom LLVM passes within the official LLVM framework:

- **Aggressive Dead Code Elimination (ADCE) Pass**
- **Instruction Combining (InstCombine) Pass**

Both passes are implemented using the new LLVM PassManager and integrated into the `llvm-project` source tree.

## Pass 1: Aggressive Dead Code Elimination (ADCE)

This pass performs an aggressive version of dead code elimination.

### Usage Example

```bash
opt -load-pass-plugin=path_to/LLVMkkADCE.so -passes=my-adce -S < path_to_test_file/test_file.ll
````

## Pass 2: Instruction Combining (InstCombine)

This pass simplifies and folds instructions in the LLVM IR, similar to LLVM's built-in `InstCombine`.

### Usage Example

```bash
opt -load-pass-plugin=/path_to/LLVMkkInstCombine.so -passes=my-instcombine -S < path_to_test_file.ll
```

## Project Structure

* The passes should be added under `llvm/lib/Transforms/`:

  * `kk_ADCEPass/`
  * `KK_InstCombine/`
* Registered in `llvm/lib/Transforms/CMakeLists.txt`
* Tests are located in:

  * `llvm/test/Transforms/ADCE/`
  * `llvm/test/Transforms/InstCombine/`
* If you want to test on already existing tests, you should replace (usually first or first two lines) RUN lines with:
  * For ADCE: (I prefer using absolute paths so /home/user...)
  ```
    ; RUN: /llvm-project/build/bin/opt -load-pass-plugin=LLVMkkADCE.so -passes=my-adce < %s
  ```

  * For InstCombine:
  ```
    ; RUN: /llvm-project/build/bin/opt -load-pass-plugin=LLVMkkInstCombine.so -passes=my-instcombine < %s
  ```
## Building the Passes

1. Clone llvm-project:

```bash
git clone https://github.com/llvm/llvm-project.git
```

2. Add your pass source code inside `llvm/lib/Transforms/` (In their own folders so for ADCE it would be llvm/lib/Transforms/kk_ADCEPass/ and then kk_ADCEPass.cpp and CMakeLists.txt files)

3. Register the pass in `llvm/lib/Transforms/CMakeLists.txt` (Add sub-directory)

4. Build with:

```bash
  cd Downloads/llvm-project
  ./make_llvm.sh
```

## Testing

* Each pass was tested using `.ll` IR test files with `llvm-lit`
* Test files include `RUN:` directives and `FileCheck` validations
* To run tests (To run custom passes, you must replace RUN directives with previously mentioned so that lit knows which passes to use):

```bash
./llvm-lit llvm/test/Transforms/ADCE/
./llvm-lit llvm/test/Transforms/InstCombine/
```

## Authors

Filip Antonijević mi19213 <br/>
Marko Petrović  mi20131 <br/>
Tamara Janićijević <br/>

University of Belgrade, Faculty of Mathematics: Compiler Construction Course
