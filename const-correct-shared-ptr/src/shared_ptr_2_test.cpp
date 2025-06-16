#include "shared_ptr_2.hpp"

#include <string>

int main(int, char **) {
  using std::string;

  SharedPtr<string> test(new string("abc"));
  *test = "123";

  const SharedPtr<string> const_test = test;

  // The bellow doesn't work because of the const
  // *const_test = "456";

  SharedPtr<string> copy(test);

  // Doesn't work
  // SharedPtr<string> copy2(const_test);

  // Works but we lose the ability to modify the pointer since we got
  // it from a copy.
  SharedPtr<const string> copy3(const_test);
}
