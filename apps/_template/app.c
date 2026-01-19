/**
 * @file app.c
 * @brief App Template for Flipper Zero
 *
 * TODO: Replace this with your app description
 */

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>

/* App state structure */
typedef struct {
    bool running;
    // TODO: Add your state variables here
} AppState;

/* Draw callback - called to render the screen */
static void draw_callback(Canvas* canvas, void* ctx) {
    AppState* state = ctx;
    UNUSED(state);

    canvas_clear(canvas);

    // TODO: Draw your UI here
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "App Template");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "Press Back to exit");
}

/* Input callback - called when buttons are pressed */
static void input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

/* Main app entry point */
int32_t app_template_main(void* p) {  // TODO: Rename to match entry_point in application.fam
    UNUSED(p);

    /* Allocate state */
    AppState* state = malloc(sizeof(AppState));
    state->running = true;

    /* Create message queue for input events */
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    /* Configure view port */
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    /* Register view port in GUI */
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    /* Main loop */
    InputEvent event;
    while(state->running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypePress || event.type == InputTypeRepeat) {
                switch(event.key) {
                case InputKeyBack:
                    state->running = false;
                    break;
                case InputKeyUp:
                    // TODO: Handle UP button
                    break;
                case InputKeyDown:
                    // TODO: Handle DOWN button
                    break;
                case InputKeyLeft:
                    // TODO: Handle LEFT button
                    break;
                case InputKeyRight:
                    // TODO: Handle RIGHT button
                    break;
                case InputKeyOk:
                    // TODO: Handle OK button
                    break;
                default:
                    break;
                }
            }
        }

        /* Update view */
        view_port_update(view_port);
    }

    /* Cleanup */
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_GUI);
    free(state);

    return 0;
}
