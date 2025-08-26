#!/bin/bash

echo "Compiling HLSL shaders..."

# Compile vertex shader
dxc -T vs_6_0 -E vsmain -Fo bin/vertex.cso vertex.hlsl

# Compile pixel shader
dxc -T ps_6_0 -E psmain -Fo bin/pixel.cso pixel.hlsl

echo "Shader compilation done."
