

#include <iostream>

#include "ApplicationBase.h"
// #include "HelloTriangleAppication.h"
int main() {
  // HelloTriangleApplication app;
  ApplicationBase base{"base test"};
  try {
    base.InitApplication("shaders/first_triangle.vert.spv",
                         "shaders/first_triangle.frag.spv");
    // app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}