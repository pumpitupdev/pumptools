#ifndef HOOK_CORE_PIU_UTILS_H
#define HOOK_CORE_PIU_UTILS_H

/**
 * Initialize the logging system as early as possible to allow recording
 * everything to a log file as well.
 */
void hook_core_piu_log_init(int argc, char **argv);

/**
 * Log process and system relevant information.
 */
void hook_core_piu_log_info(int argc, char **argv);

/**
 * Check and verify the game data path argument passed to the game via argv
 *
 * Executes various checks and catches errors that might result in the game
 * simply crashing without any error indication.
 *
 * @param argc Argc from piu main
 * @param argv Argv from piu main
 * @return New string with game data path extracted from argv (caller has to
 * free)
 */
char *hook_core_piu_utils_get_and_verify_game_data_path(int argc, char **argv);

/**
 * Verify that the current process is running as root user because the game
 * needs access to various devices that require higher privileges.
 *
 * Does not return and errors if not running as root user.
 */
void hook_core_piu_utils_verify_root_user();

/**
 * Fix the locale configuration for Fiesta 2 and newer.
 */
void hook_core_piu_utils_fix_locale();

/**
 * If further child processes are started from the current process, and using
 * LD_PRELOAD on the current process, all children are also getting the
 * preloaded library. This causes multiple calls to the trap main function and
 * just crashes our application at some point. The executable file is required
 * to contain the "piu" substring to make this work properly.
 *
 * @param argc Argc from piu main
 * @param argv Argv from piu main
 * @return True if we have to block this call by another process,
 *         false if the process calling this is the piu process.
 */
bool hook_core_piu_block_non_piu_process_recursion(int argc, char **argv);

#endif
