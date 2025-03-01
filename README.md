# msvc8.0_patcher

A conglomeration of magical artifacts that patch MSVC 8.0 binaries to add pseudo-`__usercall` syntax from IDA Pro in support of the [FUELDecompilation project](https://github.com/widberg/FUELDecompilation).

## Syntax

### Example 1

Pass `x` in `edx` and `y` on the stack. Callee cleans up the stack.

```cpp
int __usercall __edx func(int x, int y)
{
  return x + y;
}
```

### Example 2

Pass `this` in `esi`, `x` in `ebx`, and `y` on the stack. Caller cleans up the stack.

```cpp
int __userpurge __esi __ebx class::func(int x, int y)
{
  return x + y + this->z;
}
```

## Usage

Clone the repository and submodules. Use a modern Visual Studio and Ninja to build the project. Make sure to define `-DMSVC_ORIGINAL_BIN=<path to original msvc8.0 bin directory> -DMSVC_PATCHED_BIN=<path to patched msvc8.0 bin directory>`. A compatible MSVC 8.0 can be found in the [widberg/msvc8.0 repository](https://github.com/widberg/msvc8.0).

## TODO

* For some reason reg args are treated as "used" aka overwritten even if they aren't
* figure out if the compiler ever passes floats in st regs
  - IDA thinks so but I think its confused
* figure out if we need spoils?
  - Doesn't seem like it
* force storing base pointer
  - Sometimes the compiler doesn't store the base pointer so it doesn't match
* force inline/no inline per call
  - Likely unnecessary
