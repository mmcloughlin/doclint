#include <iostream>
#include <string>
#include <vector>

class Greeter {
public:
  Greeter(std::string who) : who_(who) {}

  void Greet(std::ostream &out) const { out << greeting(); }

protected:
  int for_testing() { return 42; }

private:
  std::string greeting() const { return "Hello, " + who_ + "!"; }

  std::string who_;
};

template <typename T> T sum(std::vector<T> x) {
  T s{};
  for (const auto &e : x) {
    s += e;
  }
  return s;
}

namespace {
int mul(int a, int b) { return a * b; }
} // namespace

static int neg(int a) { return -a; }

/// add adds a and b.
int add(int a, int b) { return sum<int>({mul(a, 1), -neg(b)}); }

int main() { std::cout << add(1, 2) << std::endl; }