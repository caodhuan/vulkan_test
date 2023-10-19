#!/bin/bash

cmake . -BBuild && cd build && make &&  ./vulkan_test
