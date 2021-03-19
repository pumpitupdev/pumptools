#pragma once

/**
 * Initialize this module which adds code handling the custom kernel hack for
 * piuio devices lowering call overhead to the kernel. However, with newer
 * hardware the impact of this hack has become less and this introduces some
 * sort of compatibility issue if you just want to run the games without
 * having to depend on an additional kernel module handling this hack.
 */
void patch_piuio_khack_init();