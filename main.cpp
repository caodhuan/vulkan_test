

#include <iostream>

#include "ApplicationBase.h"
// #include "HelloTriangleAppication.h"
int main() {
  // HelloTriangleApplication app;
  ApplicationBase base{"base test"};
  try {
    base.InitApplication();
    // app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}