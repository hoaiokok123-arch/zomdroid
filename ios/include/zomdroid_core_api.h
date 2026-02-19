#ifndef ZOMDROID_CORE_API_H
#define ZOMDROID_CORE_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ZOMDROID_EVENT_KEYBOARD = 0,
    ZOMDROID_EVENT_CURSOR_POS = 1,
    ZOMDROID_EVENT_MOUSE_BUTTON = 2,
    ZOMDROID_EVENT_JOYSTICK_CONNECTED = 3,
    ZOMDROID_EVENT_JOYSTICK_DISCONNECTED = 4,
    ZOMDROID_EVENT_JOYSTICK_AXIS = 5,
    ZOMDROID_EVENT_JOYSTICK_DPAD = 6,
    ZOMDROID_EVENT_JOYSTICK_BUTTON = 7
} zomdroid_event_type_t;

typedef enum {
    ZOMDROID_RENDERER_GL4ES = 0,
    ZOMDROID_RENDERER_ZINK_OSMESA = 1,
    ZOMDROID_RENDERER_ZINK_ZFA = 2,
    ZOMDROID_RENDERER_NG_GL4ES = 3
} zomdroid_renderer_t;

typedef struct {
    const char* game_dir_path;
    const char* library_dir_path;
    const char* main_class_name;
    const char* const* jvm_args;
    int jvm_argc;
    const char* const* game_args;
    int game_argc;
    zomdroid_renderer_t renderer;
    const char* renderer_name;
    const char* jvm_library_path;
    const char* linker_library_path;
    const char* const* env_vars;
    int env_varc;
} zomdroid_launch_config_t;

typedef struct {
    void* render_surface;
    int width;
    int height;
} zomdroid_surface_t;

int zomdroid_core_init(const zomdroid_launch_config_t* config);
int zomdroid_core_start_game(void);
void zomdroid_core_shutdown(void);

int zomdroid_core_set_surface(const zomdroid_surface_t* surface);
void zomdroid_core_clear_surface(void);

void zomdroid_core_send_keyboard(int key, bool is_pressed);
void zomdroid_core_send_cursor_pos(double x, double y);
void zomdroid_core_send_mouse_button(int button, bool is_pressed);
void zomdroid_core_send_joystick_axis(int axis, float state);
void zomdroid_core_send_joystick_dpad(int dpad, int state);
void zomdroid_core_send_joystick_button(int button, bool is_pressed);
void zomdroid_core_send_joystick_connected(void);
void zomdroid_core_send_joystick_disconnected(void);

#ifdef __cplusplus
}
#endif

#endif
