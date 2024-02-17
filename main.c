// Copyright 2024 copyright nitin
#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>

struct wl_compositor *wl_compositor; 
struct wl_surface* surface;
struct wl_buffer* buffer; 

struct xdg_wm_base* sh;  
struct xdg_toplevel* top; 

struct wl_shm* shm; 

uint8_t* pixel;

uint16_t height = 100; 
uint16_t width = 200; 

uint32_t alc_shm(uint64_t sz) { 
    int8_t name[8]; 
    name[0] = '/';
    for (int i = 0; i < 6; ++i) {
        name[i] = (rand() & 23) + 97; 
    }
    int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
    shm_unlink(name);
    ftruncate(fd, sz);
    return fd;
}

void resize() {
    int32_t fd = alc_shm(width*height*4);
    pixel = mmap(0, width*height*4, PROT_READ | PROT_WRITE, fd, MAP_SHARED, 0); 

    struct wl_shm_pool* pool = wl_shm_create_pool(shm, fd, width*height*4);

    buffer = wl_shm_pool_create_buffer(pool, 0, width, height, width*4, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool); 
    close(fd); 
}

void draw() {

}

void xsurface_conf(void* data, struct xdg_surface* xsurface, uint32_t ser) {
    xdg_surface_ack_configure(xsurface, ser);
    if (!pixel) {
        resize(); 
    }
    draw(); 
    wl_surface_attach(surface, buffer, 0 ,0);
    wl_surface_damage_buffer(surface, 0 , 0 ,width, height); 
    wl_surface_commit(surface); 
}

struct xdg_surface_listener xsurface_list = {
    .configure = xsurface_conf
};

void top_conf(void* data, struct xdg_toplevel* top, int32_t w, int32_t height, struct wl_array* array) {
    
}

void top_close(void* data, struct xdg_toplevel* top) {}


struct xdg_toplevel_listener top_list = {
    .configure = top_conf,
    .close = top_close
};

void sh_ping (void* data, struct xdg_wm_base* sh, uint32_t ser) {
    xdg_wm_base_pong(sh, ser); 
}

struct xdg_wm_base_listener sh_list = {
    .ping = sh_ping
};
 

void registry_global(void *data, struct wl_registry *wl_registry, uint32_t name, const char *interface, uint32_t version) {
    if (!strcmp(interface, wl_compositor_interface.name)) {
        wl_compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
    }
    else if (!strcmp(interface, wl_shm_interface.name)) {
        shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
    }
    else if (!strcmp(interface, xdg_wm_base_interface.name)) {
        shm = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(sh, &sh_list, 0);
        xdg_toplevel_set_title(top, "Wayland Client");
        wl_surface_commit(surface);
    }
}

void registry_global_remove(void* data, struct wl_registry *reg, uint32_t name) { 

}

struct wl_registry_listener wl_listener = { 
    .global = registry_global,
    .global_remove = registry_global_remove 
};

int main() {
    struct wl_display *display = wl_display_connect("wayland-1");
    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display\n");
        return EXIT_FAILURE;
    }
    else {
        fprintf(stdout, "Connected to display\n");
    }
    struct wl_registry *registry = wl_display_get_registry(display);
    if (!registry) {  
        fprintf(stdout, "Registry failed\n");
    }

    wl_registry_add_listener(registry, &wl_listener, 0); 

    if (!registry) {  
        fprintf(stdout, "Registry failed\n");
    }
    wl_display_roundtrip(display); // tells server we are looking for inputs
    fprintf(stdout, "Finished trip\n");

    surface = wl_compositor_create_surface(wl_compositor);

    struct xdg_surface* xsurface = xdg_wm_base_get_xdg_surface(sh, surface);

    xdg_surface_add_listener(xsurface , &xsurface_list, 0);

    top = xdg_surface_get_toplevel(xsurface); 
    xdg_toplevel_add_listener(top, &top_list, 0); 

    while (wl_display_dispatch(display)) ;

    if (buffer) {
        wl_buffer_destroy(buffer); 
    }

    wl_surface_destroy(surface);

    wl_display_disconnect(display);

    return EXIT_SUCCESS;
}
