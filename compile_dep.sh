#!/usr/bin/bash
# Build GLM
echo "[GLM]"
cd ./vendor/glm
cmake \
    -DGLM_BUILD_TESTS=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -B build .
cmake --build build -j4 -- all
