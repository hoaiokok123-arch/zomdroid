#ifndef ZOMDROID_IOS_BRIDGE_H
#define ZOMDROID_IOS_BRIDGE_H

#include <stdbool.h>

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#else
typedef struct objc_object UIView;
#endif

#ifdef __cplusplus
extern "C" {
#endif

int zomdroid_ios_initialize(const char* game_dir,
                            const char* library_dir,
                            const char* main_class_name);

int zomdroid_ios_initialize_ex(const char* game_dir,
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
                               int env_varc);

int zomdroid_ios_start(void);
int zomdroid_ios_set_surface(UIView* view);
void zomdroid_ios_clear_surface(void);
void zomdroid_ios_shutdown(void);

void zomdroid_ios_send_key(int key, bool is_pressed);
void zomdroid_ios_send_cursor(double x, double y);
void zomdroid_ios_send_mouse_button(int button, bool is_pressed);
void zomdroid_ios_send_touch(int phase, double x, double y);
void zomdroid_ios_send_joystick_axis(int axis, float value);
void zomdroid_ios_send_joystick_button(int button, bool is_pressed);
void zomdroid_ios_send_joystick_dpad(int dpad, int state);

#ifdef __cplusplus
}
#endif

#endif

