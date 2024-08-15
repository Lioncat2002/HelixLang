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
## Todo:
- [x] Codegen using LLVM
- [ ] Operator support
- [ ] Variables support
