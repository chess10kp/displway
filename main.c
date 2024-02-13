// Copyright 2024 copyright nitin

#include <stdio.h>
#include <stdlib.h>
#include <wayland-client-core.h>
#include <wayland-client.h>

int main(int argc, char *argv[]) {
    // null because libwayland auto connects
    struct wl_display *display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Failed to connect to wayland display\n");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Connection established!\n");
    wl_display_disconnect(display);
    return EXIT_SUCCESS;
}
