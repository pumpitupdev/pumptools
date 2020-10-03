/**
 * Block the system call to amixer which messes with the volume levels
 * (relevant for Fiesta and up)
 */
#ifndef PATCH_AMIXER_BLOCK_H
#define PATCH_AMIXER_BLOCK_H

/**
 * Initialize patch module
 */
void patch_axmier_block_init(void);

#endif