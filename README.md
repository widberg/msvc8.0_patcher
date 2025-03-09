# msvc8.0_patcher

A conglomeration of magical artifacts that patch MSVC 8.0 binaries to add pseudo-`__usercall` syntax from IDA Pro in support of the [FUELDecompilation project](https://github.com/widberg/FUELDecompilation).

## Syntax

### Example 1

Pass `x` in `edx` and `y` on the stack. Callee cleans up the stack. Only ecx needs to be caller preserved across the call, if it is live; eax and edx do not need to be preserved even though they are traditionally call spoiled registers.

```cpp
int __usercall __edx __spoils __ecx func(int x, int y)
{
  return x + y;
}
```

### Example 2

Pass `this` in `esi`, `x` in `ebx`, and `y` on the stack. Caller cleans up the stack. Default spoils behavior.

```cpp
int __userpurge __esi __ebx class::func(int x, int y)
{
  return x + y + this->z;
}
```

## Usage

Clone the repository and submodules. Use a modern Visual Studio and Ninja to build the project. Make sure to define `-DMSVC_ORIGINAL_BIN=<path to original msvc8.0 bin directory> -DMSVC_PATCHED_BIN=<path to patched msvc8.0 bin directory>`. A compatible MSVC 8.0 can be found in the [widberg/msvc8.0 repository](https://github.com/widberg/msvc8.0).

## TODO

Ordered by priority.

* For some reason reg args are treated as "used" aka overwritten even if they aren't
  - Only the regs not in the clobber list
  - This is really annoying because it makes it impossible to match functions using regs other than eax, ecx, and edx for args
  - Actual flex params are not treated as used, try to copy what those do if I can figure that out
* figure out if we need spoils/preseved regs
  - Probably need preserved
  - The compiler will save off eax before a call to a function that doesn't modify it but in the game it doesn't
  - likely the ltcg optimizer can be more restrictive with the spoils list
  - this might solve the ebp problem too?
* Figure out if we need a way to force using base pointer as reg
  - Sometimes the compiler uses another reg when it should be using ebp
  - Even with /Oy- it still uses another reg
  - This could be due to not matching instructions elsewhere in the functions due to the previous todo
* Store the regs data out of band
  - We're stepping on memory used by the compiler putting the pointer where we are
* force inline/no inline per call
  - Likely unnecessary
  - Not sure if LTCG would cause anything to be inlined that wouldn't be otherwise if available
  - If we do need this then probably just allow forceinline and noinline keywords after a function call and tag the call

## Notes

* Which regs are supported for parameters
  - IDA thinks the compiler passes floats in st regs for user code but I think its confused
  - I don't see any evidence that the compiler is capable of using st regs for flex params. Only the 8 gprs and 8 xmm regs are used
  - The compiler may emit calls to compiler runtime functions such as __ftol2_sse which does pass the argument in st0 but these are a special case an user code will never do this
  - FUEL doesn't use xmm regs for params outside of library code so I won't bother supporting it
  - FUEL also never passes arguments in EBP or ESP
