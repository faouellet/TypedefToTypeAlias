TypedefToTypeAlias
===================

TypedefToTypeAlias (TTTA) is a tool that will transform `typedef`s into type aliases.

For example, the following piece of code:
```cpp
typedef int* MyIntPtr;
```

will be transformed into this:
```cpp
using MyIntPtr = int;
```
