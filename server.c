// copyright 2024

#include <stdio.h>
#include <stdlib.h>
#include <wayland-server-core.h>
#include <wayland-server.h>

int main(int argc, char *argv[]) {
    struct wl_display *display = wl_display_create();
    if (!display) {
        fprintf(stderr, "Failed to create display\n");
        return EXIT_FAILURE;
    }
    const char *socket = wl_display_add_socket_auto(display);
    if (!socket) {
        fprintf(stderr, "Unable to add socket to wayland display\n");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Running wayland display on %s\n", socket);
    wl_display_run(display);
    wl_display_destroy(display);
    return EXIT_SUCCESS;
}
