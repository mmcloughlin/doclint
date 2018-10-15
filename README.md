# doclint

C++ Documentation Linter based on [Clang LibTooling](https://clang.llvm.org/docs/LibTooling.html).

This is purely _experimental_, used primarily as an example to help learn how to write a clang-based tool. The intention is to enforce documentation on all public exported functions in a C++ project. At the moment it seems to do something sensible on [basic example code](example.cc), but I assume the "should this declaration be commented" function would need a lot of tweaking to be useful in a large-scale codebase.

## Example

```
$ ./doclint example.cc
example.cc:7:3: error: missing comment
  Greeter(std::string who) : who_(who) {}
  ^
example.cc:9:8: error: missing comment
  void Greet(std::ostream &out) const { out << greeting(); }
       ^
example.cc:20:25: error: missing comment
template <typename T> T sum(std::vector<T> x) {
                        ^
example.cc:37:5: error: missing comment
int main() { std::cout << add(1, 2) << std::endl; }
    ^
4 errors generated.
Error while processing /Users/michaelmcloughlin/Development/misc/doclint/example.cc.
```