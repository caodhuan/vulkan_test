

#include <iostream>

#include "ApplicationBase.h"
// #include "HelloTriangleAppication.h"
int main() {
  // HelloTriangleApplication app;
  ApplicationBase base{"base test", 800, 800};
  try {
    if (base.InitApplication("shaders/colored_triangle.vert.spv",
                             "shaders/colored_triangle.frag.spv")) {
      base.Run();
    }

    // app.run();

  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}