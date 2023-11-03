#pragma once
#include "ApplicationBase.h"

class ColoredApplication : public ApplicationBase {
 public:
  ColoredApplication(const std::string &title, int width = 600,
                     int height = 800);
  ~ColoredApplication();
};