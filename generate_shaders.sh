#!/bin/bash

if [ ! -d 'Build' ]; then
    mkdir Build
fi
 
if [ ! -d 'Build/shaders' ]; then
    mkdir Build/shaders
fi

cd shaders
glslc shader.vert -o ../Build/shaders/vert.spv
glslc shader.frag -o ../Build/shaders/frag.spv

glslc first_triangle.vert -o ../Build/shaders/first_triangle.vert.spv
glslc first_triangle.frag -o ../Build/shaders/first_triangle.frag.spv
