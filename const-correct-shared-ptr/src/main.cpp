#include "shared_ptr_2.hpp"

#include <cstdio>
#include <memory>
#include <string>

static void mutate(const std::shared_ptr<std::string> &value) {
  *value = "Not what you think!";
}

static void mutate_fix(const SharedPtr<std::string> &value) {
  // No longer possible, compiler error
  // *value = "Not what you think!";
}

static void mutate2(const std::shared_ptr<std::string> &value) {
  std::shared_ptr<std::string> copy = value;

  *copy = "Still not what you think";
}

static void mutate2_fix(const SharedPtr<std::string> &value) {
  // Only works if we add const to the type
  SharedPtr<const std::string> copy = value;

  // No longer possible, compiler error
  // *copy = "Still not what you think";
}

int main(int, char **) {
  using std::string;

  auto std_value = std::make_shared<string>("Hello world!");
  SharedPtr value(new string("Hello world!"));

  printf("Initial values:\n");
  printf("  std:  '%s'\n  ours: '%s'\n\n", std_value->c_str(), value->c_str());

  mutate(std_value);
  mutate_fix(value);

  printf("After mutate:\n");
  printf("  std:  '%s'\n  ours: '%s'\n\n", std_value->c_str(), value->c_str());

  mutate2(std_value);
  mutate2_fix(value);

  printf("After mutate2:\n");
  printf("  std:  '%s'\n  ours: '%s'\n\n", std_value->c_str(), value->c_str());

  return 0;
}
