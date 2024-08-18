# HelixLang
a small language compiler written in C++
example program:
```rs
fn wrapper(n: number): number {
    return n;
}

fn main(): void {
    println(wrapper(12.34));
}
```

## Compile helix code:
- `helix <filename>.hlx`
- Help command at `helix -h`
## Todo:
- [x] Codegen using LLVM
- [x] Operator support
- [ ] Variables support

## Requirements:
- LLVM 14
- Clang
- C++ 17
- Cmake 3.22 or greater
