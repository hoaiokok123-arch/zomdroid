#include "zomdroid_core_api.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "zomdroid.h"

typedef struct {
    zomdroid_launch_config_t cfg;
    pthread_t game_thread;
    int has_cfg;
    int is_running;
    int thread_created;
    pthread_mutex_t mutex;
    zomdroid_surface_t surface;
    int has_surface;
} zomdroid_runtime_state_t;

static zomdroid_runtime_state_t g_state = {
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

static char* dup_string(const char* src) {
    if (!src) return NULL;
    size_t len = strlen(src);
    char* out = (char*)malloc(len + 1);
    if (!out) return NULL;
    memcpy(out, src, len + 1);
    return out;
}

static void free_string_array(const char** values, int count) {
    if (!values) return;
    for (int i = 0; i < count; i++) {
        free((void*)values[i]);
    }
    free((void*)values);
}

static const char** dup_string_array(const char* const* values, int count) {
    if (!values || count <= 0) return NULL;
    const char** out = (const char**)calloc((size_t)count, sizeof(*out));
    if (!out) return NULL;
    for (int i = 0; i < count; i++) {
        out[i] = dup_string(values[i]);
        if (!out[i]) {
            free_string_array(out, i);
            return NULL;
        }
    }
    return out;
}

static void free_config(zomdroid_launch_config_t* cfg) {
    if (!cfg) return;
    free((void*)cfg->game_dir_path);
    free((void*)cfg->library_dir_path);
    free((void*)cfg->main_class_name);
    free((void*)cfg->renderer_name);
    free((void*)cfg->jvm_library_path);
    free((void*)cfg->linker_library_path);
    free_string_array(cfg->jvm_args, cfg->jvm_argc);
    free_string_array(cfg->game_args, cfg->game_argc);
    free_string_array(cfg->env_vars, cfg->env_varc);
    memset(cfg, 0, sizeof(*cfg));
}

static const char* renderer_to_name(zomdroid_renderer_t renderer) {
    switch (renderer) {
        case ZOMDROID_RENDERER_ZINK_ZFA:
            return "ZINK_ZFA";
        case ZOMDROID_RENDERER_ZINK_OSMESA:
            return "ZINK_OSMESA";
        case ZOMDROID_RENDERER_NG_GL4ES:
            return "NG_GL4ES";
        case ZOMDROID_RENDERER_GL4ES:
        default:
            return "GL4ES";
    }
}

static void apply_env_pairs(const char* const* values, int count) {
    for (int i = 0; i < count; i++) {
        const char* pair = values[i];
        if (!pair) continue;
        const char* eq = strchr(pair, '=');
        if (!eq || eq == pair || eq[1] == '\0') continue;
        size_t key_len = (size_t)(eq - pair);
        char* key = (char*)malloc(key_len + 1);
        if (!key) continue;
        memcpy(key, pair, key_len);
        key[key_len] = '\0';
        setenv(key, eq + 1, 1);
        free(key);
    }
}

static int copy_config(const zomdroid_launch_config_t* input, zomdroid_launch_config_t* output) {
    memset(output, 0, sizeof(*output));
    output->renderer = input->renderer;
    output->jvm_argc = input->jvm_argc;
    output->game_argc = input->game_argc;
    output->env_varc = input->env_varc;

    output->game_dir_path = dup_string(input->game_dir_path);
    output->library_dir_path = dup_string(input->library_dir_path);
    output->main_class_name = dup_string(input->main_class_name);
    output->renderer_name = dup_string(input->renderer_name);
    output->jvm_library_path = dup_string(input->jvm_library_path);
    output->linker_library_path = dup_string(input->linker_library_path);
    output->jvm_args = dup_string_array(input->jvm_args, input->jvm_argc);
    output->game_args = dup_string_array(input->game_args, input->game_argc);
    output->env_vars = dup_string_array(input->env_vars, input->env_varc);

    if (!output->game_dir_path || !output->library_dir_path || !output->main_class_name) {
        free_config(output);
        return -1;
    }
    if (input->jvm_argc > 0 && !output->jvm_args) {
        free_config(output);
        return -1;
    }
    if (input->game_argc > 0 && !output->game_args) {
        free_config(output);
        return -1;
    }
    if (input->env_varc > 0 && !output->env_vars) {
        free_config(output);
        return -1;
    }
    return 0;
}

static ZomdroidNativeWindow* to_native_surface(void* render_surface) {
    return (ZomdroidNativeWindow*)render_surface;
}

static void* game_thread_entry(void* arg) {
    (void)arg;

    pthread_mutex_lock(&g_state.mutex);
    zomdroid_launch_config_t cfg = g_state.cfg;
    zomdroid_surface_t surface = g_state.surface;
    int has_surface = g_state.has_surface;
    pthread_mutex_unlock(&g_state.mutex);

    const char* renderer_name = cfg.renderer_name ? cfg.renderer_name : renderer_to_name(cfg.renderer);
    setenv("ZOMDROID_RENDERER", renderer_name, 1);

    if (cfg.jvm_library_path && cfg.jvm_library_path[0]) {
        setenv("ZOMDROID_JVM_LIB", cfg.jvm_library_path, 1);
    }
    if (cfg.linker_library_path && cfg.linker_library_path[0]) {
        setenv("ZOMDROID_LINKER_LIB", cfg.linker_library_path, 1);
    }

    apply_env_pairs(cfg.env_vars, cfg.env_varc);

    if (zomdroid_init() != 0) {
        pthread_mutex_lock(&g_state.mutex);
        g_state.is_running = 0;
        pthread_mutex_unlock(&g_state.mutex);
        return NULL;
    }

    if (has_surface) {
        zomdroid_surface_init(to_native_surface(surface.render_surface), surface.width, surface.height);
    }

    zomdroid_start_game(
        cfg.game_dir_path,
        cfg.library_dir_path,
        cfg.jvm_argc,
        cfg.jvm_args,
        cfg.main_class_name,
        cfg.game_argc,
        cfg.game_args
    );

    pthread_mutex_lock(&g_state.mutex);
    g_state.is_running = 0;
    pthread_mutex_unlock(&g_state.mutex);
    return NULL;
}

int zomdroid_core_init(const zomdroid_launch_config_t* config) {
    if (!config || !config->game_dir_path || !config->library_dir_path || !config->main_class_name) {
        return -1;
    }
    if (config->jvm_argc < 0 || config->game_argc < 0 || config->env_varc < 0) {
        return -1;
    }

    pthread_mutex_lock(&g_state.mutex);
    if (g_state.is_running) {
        pthread_mutex_unlock(&g_state.mutex);
        return -2;
    }

    free_config(&g_state.cfg);
    if (copy_config(config, &g_state.cfg) != 0) {
        pthread_mutex_unlock(&g_state.mutex);
        return -3;
    }
    g_state.has_cfg = 1;
    pthread_mutex_unlock(&g_state.mutex);
    return 0;
}

int zomdroid_core_start_game(void) {
    pthread_mutex_lock(&g_state.mutex);
    if (!g_state.has_cfg) {
        pthread_mutex_unlock(&g_state.mutex);
        return -1;
    }
    if (g_state.is_running) {
        pthread_mutex_unlock(&g_state.mutex);
        return -2;
    }

    g_state.is_running = 1;
    int err = pthread_create(&g_state.game_thread, NULL, game_thread_entry, NULL);
    if (err != 0) {
        g_state.is_running = 0;
        pthread_mutex_unlock(&g_state.mutex);
        return -3;
    }
    g_state.thread_created = 1;
    pthread_detach(g_state.game_thread);
    pthread_mutex_unlock(&g_state.mutex);
    return 0;
}

void zomdroid_core_shutdown(void) {
    pthread_mutex_lock(&g_state.mutex);
    g_state.has_surface = 0;
    g_state.is_running = 0;
    g_state.thread_created = 0;
    free_config(&g_state.cfg);
    g_state.has_cfg = 0;
    pthread_mutex_unlock(&g_state.mutex);

    zomdroid_surface_deinit();
    zomdroid_deinit();
}

int zomdroid_core_set_surface(const zomdroid_surface_t* surface) {
    if (!surface) {
        return -1;
    }

    pthread_mutex_lock(&g_state.mutex);
    g_state.surface = *surface;
    g_state.has_surface = 1;
    int should_apply = g_state.is_running;
    pthread_mutex_unlock(&g_state.mutex);

    if (should_apply) {
        zomdroid_surface_init(to_native_surface(surface->render_surface), surface->width, surface->height);
    }
    return 0;
}

void zomdroid_core_clear_surface(void) {
    pthread_mutex_lock(&g_state.mutex);
    g_state.has_surface = 0;
    pthread_mutex_unlock(&g_state.mutex);
    zomdroid_surface_deinit();
}

void zomdroid_core_send_keyboard(int key, bool is_pressed) {
    zomdroid_event_keyboard(key, is_pressed);
}

void zomdroid_core_send_cursor_pos(double x, double y) {
    zomdroid_event_cursor_pos(x, y);
}

void zomdroid_core_send_mouse_button(int button, bool is_pressed) {
    zomdroid_event_mouse_button(button, is_pressed);
}

void zomdroid_core_send_joystick_axis(int axis, float state) {
    zomdroid_event_joystick_axis(axis, state);
}

void zomdroid_core_send_joystick_dpad(int dpad, int state) {
    zomdroid_event_joystick_dpad(dpad, (char)state);
}

void zomdroid_core_send_joystick_button(int button, bool is_pressed) {
    zomdroid_event_joystick_button(button, is_pressed);
}

void zomdroid_core_send_joystick_connected(void) {
    zomdroid_event_joystick_connected();
}

void zomdroid_core_send_joystick_disconnected(void) {
    /*
     * Current native core doesn't expose a dedicated disconnect event API.
     * Keep this as a no-op for now.
     */
}
