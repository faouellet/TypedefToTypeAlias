Clang-Tools
===================

Introduction
-------------
clang-tools are mostly a fun and simple way of introducing me to writing Clang-based tools. Most of the implementation is based on [AST matchers and Clang refactoring tools](http://eli.thegreenplace.net/2014/07/29/ast-matchers-and-clang-refactoring-tools).

```
Compiling
-------------
Requirements:
- CMake
- Clang
- LLVM

To compile clang-tools, you first need to create a build directory.
```bash
mkdir ct_build
```
Then, you need to position yourself in the build directory.
```bash
cd ct_build
```
Afterwards, you need to invoke CMake so it can generate the Makefile.
```sh
cmake <path/to/clang-tools>
```
Finally, you can build clang-tools with a simple command.
```sh
make
```

Usage
-------------
To use one of the tools, you need to invoke it with at least a file you want to modify. 
```
./tool test.cpp --
```
Also, since they are Clang-based tools, you will need to specify a compilation database. For more information on this subject, check out [Compilation databases for Clang-based tools](http://eli.thegreenplace.net/2014/05/21/compilation-databases-for-clang-based-tools). In the example above, I used `--` to express that I'm using a "fixed" compilation database.

For more information on how to use one of the tools, you can invoke the help command
```sh
./tool -help
```
