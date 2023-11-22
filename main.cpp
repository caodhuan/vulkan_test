

#include <iostream>

#include "ApplicationBase.h"
// #include "HelloTriangleAppication.h"
int main() {
    // HelloTriangleApplication app;
    ApplicationBase base;
    try {
        if (base.InitApplication("shaders/colored_triangle.vert.spv",
                                 "shaders/colored_triangle.frag.spv",
                                 "base test", 600, 800)) {
            base.Run();
        }

        // app.run();

    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}