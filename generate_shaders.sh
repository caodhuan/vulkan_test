#!/bin/bash
cd shaders
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv

cd ..

cp -r shaders Build/shaders
