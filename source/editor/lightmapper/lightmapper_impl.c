#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "editor/glad/glad.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <GLFW/glfw3.h>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#define LIGHTMAPPER_IMPLEMENTATION
#include "lightmapper.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
