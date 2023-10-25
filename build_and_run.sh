#!/bin/bash

cmake . -BBuild 

cp -r textures Build
cp -r models Build
cd Build && make &&  ./vulkan_test
