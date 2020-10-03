/**
 * Provides a set of macros to hook into an application right before its
 * main function is called.
 */
#ifndef CNH_LIB_MAIN_H
#define CNH_LIB_MAIN_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <dlfcn.h>

#include "util/log.h"

#define LIB_MAIN_CONSTRUCTOR(func) __attribute__((constructor)) void lib_constructor() { func(); }

#define LIB_MAIN_DESTRUCTOR(func) __attribute__((destructor)) void lib_destructor() { func(); }

#define LIB_MAIN_TRAP_MAIN(func_before_main, func_after_main) \
typedef int (*func_libc_start_main_t)(int *(main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end)); \
typedef int* (*func_main_t)(int, char**, char**); \
typedef void (*func_init_t)(void); \
static func_main_t orig_main; \
static func_init_t orig_init; \
static bool trapped; \
\
int* trap_main(int argc, char** argv, char** envp) \
{ \
	log_debug("Trap before main (%s): argc %d", argv[0], argc); \
	func_before_main(argc, argv); \
    log_debug("Calling orig_init"); \
	orig_init(); \
	log_debug("Calling real main (%s): argc %d", argv[0], argc); \
	int* ret = orig_main(argc, argv, envp); \
	log_debug("Calling cleanup after main (%s): ret %d", argv[0], ret); \
	func_after_main(); \
	return ret; \
} \
\
void dummy_init(void) {} \
\
int __libc_start_main(int *(main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end)) \
{ \
	func_libc_start_main_t start = (func_libc_start_main_t) dlsym(RTLD_NEXT, "__libc_start_main"); \
	if (start == NULL) { \
		printf("ERROR finding orig func __libc_start_main\n"); \
	} \
	orig_main = main; \
	main = trap_main; \
	orig_init = init; \
	init = dummy_init; \
	trapped = true; \
	return (*start)(main, argc, ubp_av, init, fini, rtld_fini, stack_end); \
}

#endif