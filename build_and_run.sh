#!/bin/bash

cmake . -BBuild 

cp -r textures Build
cd Build && make &&  ./vulkan_test
