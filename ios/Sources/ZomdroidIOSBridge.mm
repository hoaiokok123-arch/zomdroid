#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include "zomdroid_core_api.h"

enum {
    ZOMDROID_IOS_TOUCH_DOWN = 0,
    ZOMDROID_IOS_TOUCH_MOVE = 1,
    ZOMDROID_IOS_TOUCH_UP = 2
};

extern "C" int zomdroid_ios_initialize(const char* game_dir,
                                       const char* library_dir,
                                       const char* main_class_name) {
    zomdroid_launch_config_t cfg = {};
    cfg.game_dir_path = game_dir;
    cfg.library_dir_path = library_dir;
    cfg.main_class_name = main_class_name;
    cfg.renderer = ZOMDROID_RENDERER_GL4ES;
    cfg.renderer_name = "GL4ES";
    return zomdroid_core_init(&cfg);
}

extern "C" int zomdroid_ios_initialize_ex(const char* game_dir,
                                          const char* library_dir,
                                          const char* main_class_name,
                                          const char* renderer_name,
                                          const char* jvm_library_path,
                                          const char* linker_library_path,
                                          const char* const* jvm_args,
                                          int jvm_argc,
                                          const char* const* game_args,
                                          int game_argc,
                                          const char* const* env_vars,
                                          int env_varc) {
    zomdroid_launch_config_t cfg = {};
    cfg.game_dir_path = game_dir;
    cfg.library_dir_path = library_dir;
    cfg.main_class_name = main_class_name;
    cfg.renderer = ZOMDROID_RENDERER_GL4ES;
    cfg.renderer_name = renderer_name;
    cfg.jvm_library_path = jvm_library_path;
    cfg.linker_library_path = linker_library_path;
    cfg.jvm_args = jvm_args;
    cfg.jvm_argc = jvm_argc;
    cfg.game_args = game_args;
    cfg.game_argc = game_argc;
    cfg.env_vars = env_vars;
    cfg.env_varc = env_varc;
    return zomdroid_core_init(&cfg);
}

extern "C" int zomdroid_ios_start(void) {
    return zomdroid_core_start_game();
}

extern "C" int zomdroid_ios_set_surface(UIView* view) {
    if (view == nil) {
        return -1;
    }

    CGSize size = view.bounds.size;
    zomdroid_surface_t surface = {};
    surface.render_surface = (__bridge void*)view.layer;
    surface.width = (int)size.width;
    surface.height = (int)size.height;
    return zomdroid_core_set_surface(&surface);
}

extern "C" void zomdroid_ios_clear_surface(void) {
    zomdroid_core_clear_surface();
}

extern "C" void zomdroid_ios_shutdown(void) {
    zomdroid_core_shutdown();
}

extern "C" void zomdroid_ios_send_key(int key, bool is_pressed) {
    zomdroid_core_send_keyboard(key, is_pressed);
}

extern "C" void zomdroid_ios_send_cursor(double x, double y) {
    zomdroid_core_send_cursor_pos(x, y);
}

extern "C" void zomdroid_ios_send_mouse_button(int button, bool is_pressed) {
    zomdroid_core_send_mouse_button(button, is_pressed);
}

extern "C" void zomdroid_ios_send_touch(int phase, double x, double y) {
    zomdroid_core_send_cursor_pos(x, y);
    if (phase == ZOMDROID_IOS_TOUCH_DOWN) {
        zomdroid_core_send_mouse_button(0, true);
    } else if (phase == ZOMDROID_IOS_TOUCH_UP) {
        zomdroid_core_send_mouse_button(0, false);
    }
}

extern "C" void zomdroid_ios_send_joystick_axis(int axis, float value) {
    zomdroid_core_send_joystick_axis(axis, value);
}

extern "C" void zomdroid_ios_send_joystick_button(int button, bool is_pressed) {
    zomdroid_core_send_joystick_button(button, is_pressed);
}

extern "C" void zomdroid_ios_send_joystick_dpad(int dpad, int state) {
    zomdroid_core_send_joystick_dpad(dpad, state);
}
