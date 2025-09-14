#!/bin/bash

echo "Compiling HLSL shaders..."

dxc -T vs_6_0 -E vsmain -Fo bin/vertex.cso vertex.hlsl

dxc -T ps_6_0 -E psmain -Fo bin/pixel.cso pixel.hlsl

echo "Shader compilation done."
