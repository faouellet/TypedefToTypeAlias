TypedefToTypeAlias
===================

Introduction
-------------
TypedefToTypeAlias (TTTA) is a Clang-based tool that will transform `typedef`s into type aliases.

For example, the following piece of code:
```cpp
typedef int* MyIntPtr;
```

will be transformed into this:
```cpp
using MyIntPtr = int;
```

Also, it serves as a fun and simple introduction to writing Clang-based tools. Most of the implementation is based on [AST matchers and Clang refactoring tools](http://eli.thegreenplace.net/2014/07/29/ast-matchers-and-clang-refactoring-tools).

Compiling
-------------
Requirements:
- CMake
- Clang
- LLVM

To compile TTTA, you first need to create a build directory.
```bash
mkdir ttta_build
```
Then, you need to position yourself in the build directory.
```bash
cd ttta_build
```
Afterwards, you need to invoke CMake so it can generate the Makefile.
```sh
cmake <path/to/TTTA>
```
Finally, you can build TTTA with a simple command.
```sh
make
```

Usage
-------------
To use TTTA, you need to at least specify a file you want to modify. 
```
./ttta test.cpp --
```
Also, since it is a Clang-based tool, you will need to specify a compilation database. For more information on this subject, check out [Compilation databases for Clang-based tools](http://eli.thegreenplace.net/2014/05/21/compilation-databases-for-clang-based-tools). In the example above, I used `--` to express that I'm using a "fixed" compilation database.

For more information on how to use TTTA, you can invoke the help command of TTTA
```sh
./ttta -help
```
