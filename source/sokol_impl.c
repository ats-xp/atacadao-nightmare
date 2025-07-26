#define SOKOL_GLCORE
#define SOKOL_IMPL

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_fetch.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_time.h"

#include "sokol_shape.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#define FONTSTASH_IMPLEMENTATION
#if defined(_MSC_VER )
#pragma warning(disable:4996)   // strncpy use in fontstash.h
#endif
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

#include "fontstash.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#define SOKOL_GL_IMPL
#include "sokol_gl.h"
#define SOKOL_FONTSTASH_IMPL
#include "sokol_fontstash.h"
