/**
 * Module to fix weird OpenGL error that only appeared on Exceed 2 thus far.
 *
 * If OpenGL is not forced into single threaded mode, the backend will run in
 * multi-threaded mode for some reason and crash in __glDispatchCheckMultithreaded
 * when the game calls glXChooseVisual in its GFX init function.
 *
 * The fix is quite simple (luckily) as this seems that Exceed 2 is not the only
 * game that requires the environment variable __GL_SINGLETHREADED to be set.
 */

#include <stdio.h>
#include <string.h>

#include "capnhook/hook/lib.h"

typedef char* (*getenv_t) (const char* name);

static getenv_t _x2hook_ogl_fix_real_getenv;

char* getenv(const char* name)
{
    if (!_x2hook_ogl_fix_real_getenv) {
        _x2hook_ogl_fix_real_getenv = (getenv_t) cnh_lib_get_func_addr("getenv");
    }

    char* ret =  _x2hook_ogl_fix_real_getenv(name);

    if (!strcmp(name, "__GL_SINGLETHREADED")) {
        if (ret == NULL || strcmp(name, "1") != 0) {
            printf("!!! x2hook: __GL_SINGLETHREADED not set or enabled, force patching OpenGL to single threaded to "
                "avoid crash !!!\n");
            return "1";
        }
    }

    return ret;
}