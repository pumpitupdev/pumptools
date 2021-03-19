#pragma once

void propatch_usb_fix_init(
    const char *device_nodes, const char *p1_bus_port, const char *p2_bus_port);

void propatch_usb_fix_shutdown();