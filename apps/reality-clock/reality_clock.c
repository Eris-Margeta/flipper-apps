/**
 * @file reality_clock.c
 * @brief Reality Dimension Clock for Flipper Zero
 *
 * Multi-band electromagnetic ratio analyzer for dimensional stability detection.
 * Uses rolling 1000-sample buffer for ultra-stable readings.
 *
 * @author Eris Margeta (@Eris-Margeta)
 * @license MIT
 * @version 2.0
 *
 * SPDX-License-Identifier: MIT
 */

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_random.h>
#include <furi_hal_power.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stdlib.h>
#include <math.h>

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */

#define SCREEN_WIDTH         128
#define SCREEN_HEIGHT        64

#define INPUT_QUEUE_SIZE     8

/** Sample rates */
#define SAMPLE_INTERVAL_CALIB_MS  200   /**< 5 samples/sec during calibration */
#define SAMPLE_INTERVAL_NORMAL_MS 1000  /**< 1 sample/sec during normal */

/** Rolling buffer size */
#define BUFFER_SIZE          1000  /**< Rolling buffer for stability */
#define CALIBRATION_SAMPLES  100   /**< Samples needed before stable (20 sec at 5Hz) */

/** Stability thresholds - very generous with averaged readings */
#define HOME_THRESHOLD       90.0f
#define STABLE_THRESHOLD     75.0f
#define UNSTABLE_THRESHOLD   50.0f

/** Screen IDs */
#define SCREEN_HOME          0   /**< Main sci-fi display */
#define SCREEN_BANDS         1   /**< Band readings */
#define SCREEN_DETAILS       2   /**< Scrollable details */
#define SCREEN_COUNT         3

/** Details screen */
#define DETAILS_LINES        12
#define DETAILS_VISIBLE      5
#define LINE_HEIGHT          10

/* ============================================================================
 * TYPES
 * ============================================================================ */

typedef enum {
    DimStatusHome,
    DimStatusStable,
    DimStatusUnstable,
    DimStatusForeign,
    DimStatusCalibrating,
} DimensionStatus;

/** Rolling buffer for a single band */
typedef struct {
    float values[BUFFER_SIZE];
    uint16_t write_idx;
    uint16_t count;
    float sum;
} RollingBuffer;

typedef struct {
    bool is_running;
    bool is_calibrated;

    uint8_t current_screen;
    int8_t scroll_offset;

    /** Rolling buffers for each band */
    RollingBuffer lf_buffer;
    RollingBuffer hf_buffer;
    RollingBuffer uhf_buffer;

    /** Averaged readings (from buffers) */
    float lf_avg;
    float hf_avg;
    float uhf_avg;

    /** Current raw readings (for display) */
    float lf_raw;
    float hf_raw;
    float uhf_raw;

    float phi_current;       /**< Φ from averaged readings */
    float phi_baseline;      /**< Baseline Φ (first stable average) */
    float match_percent;

    DimensionStatus status;

    uint32_t total_samples;
    float voltage;
    float current_ma;
} RealityClockState;

/* ============================================================================
 * ROLLING BUFFER
 * ============================================================================ */

static void buffer_init(RollingBuffer* buf) {
    memset(buf->values, 0, sizeof(buf->values));
    buf->write_idx = 0;
    buf->count = 0;
    buf->sum = 0.0f;
}

static void buffer_add(RollingBuffer* buf, float value) {
    /* Subtract old value from sum if buffer is full */
    if(buf->count >= BUFFER_SIZE) {
        buf->sum -= buf->values[buf->write_idx];
    } else {
        buf->count++;
    }

    /* Add new value */
    buf->values[buf->write_idx] = value;
    buf->sum += value;

    /* Advance write pointer (circular) */
    buf->write_idx = (buf->write_idx + 1) % BUFFER_SIZE;
}

static float buffer_average(RollingBuffer* buf) {
    if(buf->count == 0) return 0.0f;
    return buf->sum / (float)buf->count;
}

/* ============================================================================
 * HARDWARE ENTROPY
 * ============================================================================ */

static float get_entropy_float(void) {
    uint32_t val;
    furi_hal_random_fill_buf((uint8_t*)&val, sizeof(val));
    return (float)(val & 0xFFFF) / 65536.0f;
}

static float get_timing_jitter(void) {
    uint32_t t1 = furi_get_tick();
    furi_delay_us(1);
    uint32_t t2 = furi_get_tick();
    return ((float)((t2 - t1) ^ (t1 & 0xFF)) / 1000.0f) - 0.5f;
}

/* ============================================================================
 * SENSOR READINGS - Raw with natural variation
 * ============================================================================ */

static float read_lf_raw(void) {
    float base = -42.0f;
    float variation = get_entropy_float() * 8.0f - 4.0f;
    float jitter = get_timing_jitter() * 1.5f;
    return base + variation + jitter;
}

static float read_hf_raw(void) {
    float base = -58.0f;
    float variation = get_entropy_float() * 6.0f - 3.0f;
    float jitter = get_timing_jitter() * 1.2f;
    return base + variation + jitter;
}

static float read_uhf_raw(void) {
    float base = -70.0f;
    float variation = get_entropy_float() * 10.0f - 5.0f;
    float jitter = get_timing_jitter() * 2.0f;
    return base + variation + jitter;
}

/* ============================================================================
 * CALCULATIONS
 * ============================================================================ */

static float db_to_linear(float db) {
    return powf(10.0f, db / 20.0f);
}

static float calculate_phi(float lf_db, float hf_db, float uhf_db) {
    float lf_lin = db_to_linear(lf_db);
    float hf_lin = db_to_linear(hf_db);
    float uhf_lin = db_to_linear(uhf_db);

    if(hf_lin < 0.0001f) return 0.0f;
    return (lf_lin * uhf_lin) / (hf_lin * hf_lin);
}

static float calculate_match(float current, float baseline) {
    if(baseline < 0.0001f) return 100.0f;
    float diff = fabsf(current - baseline) / baseline;
    float match = 100.0f * (1.0f - diff);
    if(match < 0.0f) match = 0.0f;
    if(match > 100.0f) match = 100.0f;
    return match;
}

static DimensionStatus classify_status(float match_pct) {
    if(match_pct >= HOME_THRESHOLD) return DimStatusHome;
    if(match_pct >= STABLE_THRESHOLD) return DimStatusStable;
    if(match_pct >= UNSTABLE_THRESHOLD) return DimStatusUnstable;
    return DimStatusForeign;
}

static void update_readings(RealityClockState* state) {
    /* Read raw sensor values */
    state->lf_raw = read_lf_raw();
    state->hf_raw = read_hf_raw();
    state->uhf_raw = read_uhf_raw();

    /* Add to rolling buffers */
    buffer_add(&state->lf_buffer, state->lf_raw);
    buffer_add(&state->hf_buffer, state->hf_raw);
    buffer_add(&state->uhf_buffer, state->uhf_raw);

    /* Calculate averages from buffers */
    state->lf_avg = buffer_average(&state->lf_buffer);
    state->hf_avg = buffer_average(&state->hf_buffer);
    state->uhf_avg = buffer_average(&state->uhf_buffer);

    /* Calculate Φ from AVERAGED readings (stable!) */
    state->phi_current = calculate_phi(state->lf_avg, state->hf_avg, state->uhf_avg);

    state->total_samples++;

    /* Read battery */
    state->voltage = furi_hal_power_get_battery_voltage(FuriHalPowerICFuelGauge);
    state->current_ma = furi_hal_power_get_battery_current(FuriHalPowerICFuelGauge);

    /* Calibration check */
    if(!state->is_calibrated) {
        state->status = DimStatusCalibrating;

        if(state->lf_buffer.count >= CALIBRATION_SAMPLES) {
            /* Set baseline from current averaged Φ */
            state->phi_baseline = state->phi_current;
            state->is_calibrated = true;
        }
    } else {
        /* Normal operation */
        state->match_percent = calculate_match(state->phi_current, state->phi_baseline);
        state->status = classify_status(state->match_percent);
    }
}

/* ============================================================================
 * SCI-FI UI DRAWING UTILITIES
 * ============================================================================ */

/** Draw sci-fi corner brackets */
static void draw_scifi_corners(Canvas* canvas) {
    /* Top-left corner */
    canvas_draw_line(canvas, 0, 0, 10, 0);
    canvas_draw_line(canvas, 0, 0, 0, 10);
    canvas_draw_line(canvas, 2, 2, 8, 2);
    canvas_draw_line(canvas, 2, 2, 2, 8);

    /* Top-right corner */
    canvas_draw_line(canvas, 117, 0, 127, 0);
    canvas_draw_line(canvas, 127, 0, 127, 10);
    canvas_draw_line(canvas, 119, 2, 125, 2);
    canvas_draw_line(canvas, 125, 2, 125, 8);

    /* Bottom-left corner */
    canvas_draw_line(canvas, 0, 53, 0, 63);
    canvas_draw_line(canvas, 0, 63, 10, 63);
    canvas_draw_line(canvas, 2, 55, 2, 61);
    canvas_draw_line(canvas, 2, 61, 8, 61);

    /* Bottom-right corner */
    canvas_draw_line(canvas, 127, 53, 127, 63);
    canvas_draw_line(canvas, 117, 63, 127, 63);
    canvas_draw_line(canvas, 125, 55, 125, 61);
    canvas_draw_line(canvas, 119, 61, 125, 61);
}

/** Draw decorative horizontal lines */
static void draw_scifi_lines(Canvas* canvas, int16_t y) {
    /* Left side decorative line */
    canvas_draw_line(canvas, 5, y, 25, y);
    canvas_draw_dot(canvas, 27, y);
    canvas_draw_dot(canvas, 29, y);

    /* Right side decorative line */
    canvas_draw_line(canvas, 102, y, 122, y);
    canvas_draw_dot(canvas, 100, y);
    canvas_draw_dot(canvas, 98, y);
}

/** Draw a large stylized character for E137 */
static void draw_large_e137(Canvas* canvas, int16_t center_x, int16_t center_y) {
    /* Draw "E-137" in a large, blocky sci-fi style */
    /* Each character is approximately 12 pixels wide, 16 pixels tall */
    int16_t x = center_x - 30;  /* Start position */
    int16_t y = center_y - 8;

    /* 'E' */
    canvas_draw_box(canvas, x, y, 3, 16);
    canvas_draw_box(canvas, x, y, 10, 3);
    canvas_draw_box(canvas, x, y + 6, 8, 3);
    canvas_draw_box(canvas, x, y + 13, 10, 3);
    x += 14;

    /* '-' */
    canvas_draw_box(canvas, x, y + 6, 6, 3);
    x += 10;

    /* '1' */
    canvas_draw_box(canvas, x + 3, y, 3, 16);
    canvas_draw_box(canvas, x, y, 6, 3);
    canvas_draw_box(canvas, x, y + 13, 9, 3);
    x += 13;

    /* '3' */
    canvas_draw_box(canvas, x, y, 10, 3);
    canvas_draw_box(canvas, x + 7, y, 3, 16);
    canvas_draw_box(canvas, x + 2, y + 6, 8, 3);
    canvas_draw_box(canvas, x, y + 13, 10, 3);
    x += 14;

    /* '7' */
    canvas_draw_box(canvas, x, y, 10, 3);
    canvas_draw_box(canvas, x + 7, y, 3, 16);
}

/* ============================================================================
 * SCREEN DRAWING
 * ============================================================================ */

static void draw_screen_home(Canvas* canvas, RealityClockState* state) {
    /* Sci-fi corners */
    draw_scifi_corners(canvas);

    /* Title at top */
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, "REALITY DIMENSION CLOCK");

    /* Decorative line under title */
    draw_scifi_lines(canvas, 14);

    if(!state->is_calibrated) {
        /* Calibrating display */
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignCenter, "CALIBRATING...");

        char buf[32];
        float progress = (float)state->lf_buffer.count / (float)CALIBRATION_SAMPLES * 100.0f;
        snprintf(buf, sizeof(buf), "%d%%", (int)progress);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignCenter, buf);

        /* Progress bar */
        canvas_draw_frame(canvas, 24, 48, 80, 8);
        int16_t fill = (int16_t)(progress * 0.78f);
        if(fill > 0) canvas_draw_box(canvas, 25, 49, fill, 6);

        return;
    }

    /* Large E-137 in center */
    draw_large_e137(canvas, 64, 32);

    /* Decorative line above status */
    draw_scifi_lines(canvas, 48);

    /* Status text */
    canvas_set_font(canvas, FontSecondary);
    const char* status_text;
    switch(state->status) {
        case DimStatusHome:
            status_text = "[ HOME DIMENSION ]";
            break;
        case DimStatusStable:
            status_text = "[ STABLE ]";
            break;
        case DimStatusUnstable:
            status_text = "[ DRIFT DETECTED ]";
            break;
        case DimStatusForeign:
            status_text = "[ FOREIGN DIMENSION ]";
            break;
        default:
            status_text = "[ SCANNING... ]";
    }
    canvas_draw_str_aligned(canvas, 64, 56, AlignCenter, AlignCenter, status_text);

    /* Navigation hint - subtle */
    canvas_draw_str(canvas, 122, 32, ">");
}

static void draw_bar(Canvas* canvas, int16_t x, int16_t y, int16_t w, int16_t h, float percent) {
    if(percent < 0.0f) percent = 0.0f;
    if(percent > 100.0f) percent = 100.0f;
    int16_t fill_w = (int16_t)((w * percent) / 100.0f);
    if(fill_w > 0) {
        canvas_draw_box(canvas, x, y, fill_w, h);
    }
    for(int16_t i = fill_w; i < w; i += 2) {
        canvas_draw_dot(canvas, x + i, y + h/2);
    }
}

static float db_to_percent(float db) {
    float p = ((db + 80.0f) / 50.0f) * 100.0f;
    if(p < 0.0f) p = 0.0f;
    if(p > 100.0f) p = 100.0f;
    return p;
}

static void draw_screen_bands(Canvas* canvas, RealityClockState* state) {
    char buf[32];

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, "BAND ANALYSIS");
    canvas_draw_line(canvas, 0, 10, 127, 10);

    int16_t y = 14;
    int16_t bar_x = 35;
    int16_t bar_w = 55;

    /* LF - show both raw and averaged */
    snprintf(buf, sizeof(buf), "LF");
    canvas_draw_str(canvas, 2, y + 5, buf);
    draw_bar(canvas, bar_x, y, bar_w, 5, db_to_percent(state->lf_avg));
    snprintf(buf, sizeof(buf), "%.1f", (double)state->lf_avg);
    canvas_draw_str(canvas, 95, y + 5, buf);
    y += 10;

    /* HF */
    canvas_draw_str(canvas, 2, y + 5, "HF");
    draw_bar(canvas, bar_x, y, bar_w, 5, db_to_percent(state->hf_avg));
    snprintf(buf, sizeof(buf), "%.1f", (double)state->hf_avg);
    canvas_draw_str(canvas, 95, y + 5, buf);
    y += 10;

    /* UHF */
    canvas_draw_str(canvas, 2, y + 5, "UHF");
    draw_bar(canvas, bar_x, y, bar_w, 5, db_to_percent(state->uhf_avg));
    snprintf(buf, sizeof(buf), "%.1f", (double)state->uhf_avg);
    canvas_draw_str(canvas, 95, y + 5, buf);

    /* Separator */
    canvas_draw_line(canvas, 0, 44, 127, 44);

    /* Phi and match */
    snprintf(buf, sizeof(buf), "PHI: %.4f", (double)state->phi_current);
    canvas_draw_str(canvas, 2, 54, buf);

    snprintf(buf, sizeof(buf), "Match: %.1f%%", (double)state->match_percent);
    canvas_draw_str(canvas, 70, 54, buf);

    /* Buffer status */
    snprintf(buf, sizeof(buf), "Buffer: %d/%d", state->lf_buffer.count, BUFFER_SIZE);
    canvas_draw_str(canvas, 2, 62, buf);

    /* Navigation */
    canvas_draw_str(canvas, 2, 8, "<");
    canvas_draw_str(canvas, 120, 8, ">");
}

static void draw_screen_details(Canvas* canvas, RealityClockState* state) {
    char lines[DETAILS_LINES][32];
    int line_count = 0;

    snprintf(lines[line_count++], 32, "Baseline PHI: %.4f", (double)state->phi_baseline);
    snprintf(lines[line_count++], 32, "Current PHI:  %.4f", (double)state->phi_current);
    snprintf(lines[line_count++], 32, "Match:        %.2f%%", (double)state->match_percent);
    snprintf(lines[line_count++], 32, "Buffer Size:  %d", state->lf_buffer.count);
    snprintf(lines[line_count++], 32, "Total Samples:%lu", (unsigned long)state->total_samples);
    snprintf(lines[line_count++], 32, "LF Raw:       %.2f dB", (double)state->lf_raw);
    snprintf(lines[line_count++], 32, "HF Raw:       %.2f dB", (double)state->hf_raw);
    snprintf(lines[line_count++], 32, "UHF Raw:      %.2f dB", (double)state->uhf_raw);
    snprintf(lines[line_count++], 32, "LF Avg:       %.2f dB", (double)state->lf_avg);
    snprintf(lines[line_count++], 32, "HF Avg:       %.2f dB", (double)state->hf_avg);
    snprintf(lines[line_count++], 32, "UHF Avg:      %.2f dB", (double)state->uhf_avg);
    snprintf(lines[line_count++], 32, "Battery:      %.2fV", (double)state->voltage);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, "DETAILS");

    char scroll_buf[16];
    snprintf(scroll_buf, sizeof(scroll_buf), "[%d-%d/%d]",
        state->scroll_offset + 1,
        state->scroll_offset + DETAILS_VISIBLE,
        line_count);
    canvas_draw_str(canvas, 80, 8, scroll_buf);

    canvas_draw_line(canvas, 0, 10, 127, 10);

    int16_t y = 20;
    for(int i = 0; i < DETAILS_VISIBLE && (state->scroll_offset + i) < line_count; i++) {
        canvas_draw_str(canvas, 4, y, lines[state->scroll_offset + i]);
        y += LINE_HEIGHT;
    }

    /* Scroll hints */
    if(state->scroll_offset > 0) {
        canvas_draw_str(canvas, 118, 20, "^");
    }
    if(state->scroll_offset + DETAILS_VISIBLE < line_count) {
        canvas_draw_str(canvas, 118, 58, "v");
    }

    canvas_draw_str(canvas, 2, 62, "<");
}

static void render_callback(Canvas* canvas, void* ctx) {
    RealityClockState* state = (RealityClockState*)ctx;
    canvas_clear(canvas);

    switch(state->current_screen) {
        case SCREEN_HOME:
            draw_screen_home(canvas, state);
            break;
        case SCREEN_BANDS:
            draw_screen_bands(canvas, state);
            break;
        case SCREEN_DETAILS:
            draw_screen_details(canvas, state);
            break;
        default:
            draw_screen_home(canvas, state);
    }
}

/* ============================================================================
 * INPUT
 * ============================================================================ */

static void input_callback(InputEvent* event, void* ctx) {
    FuriMessageQueue* queue = (FuriMessageQueue*)ctx;
    furi_message_queue_put(queue, event, FuriWaitForever);
}

static void process_input(RealityClockState* state, InputEvent* event) {
    if(event->type != InputTypePress && event->type != InputTypeRepeat) return;

    switch(event->key) {
        case InputKeyLeft:
            if(state->current_screen > 0) {
                state->current_screen--;
                state->scroll_offset = 0;
            }
            break;

        case InputKeyRight:
            if(state->current_screen < SCREEN_COUNT - 1) {
                state->current_screen++;
                state->scroll_offset = 0;
            }
            break;

        case InputKeyUp:
            /* Only scroll on details screen */
            if(state->current_screen == SCREEN_DETAILS && state->scroll_offset > 0) {
                state->scroll_offset--;
            }
            break;

        case InputKeyDown:
            /* Only scroll on details screen */
            if(state->current_screen == SCREEN_DETAILS) {
                if(state->scroll_offset + DETAILS_VISIBLE < DETAILS_LINES) {
                    state->scroll_offset++;
                }
            }
            break;

        case InputKeyOk:
            /* Recalibrate */
            state->is_calibrated = false;
            buffer_init(&state->lf_buffer);
            buffer_init(&state->hf_buffer);
            buffer_init(&state->uhf_buffer);
            state->total_samples = 0;
            state->phi_baseline = 0;
            state->match_percent = 0;
            break;

        case InputKeyBack:
            state->is_running = false;
            break;

        default:
            break;
    }
}

/* ============================================================================
 * LIFECYCLE
 * ============================================================================ */

static RealityClockState* state_alloc(void) {
    RealityClockState* state = malloc(sizeof(RealityClockState));
    memset(state, 0, sizeof(RealityClockState));

    state->is_running = true;
    state->status = DimStatusCalibrating;

    buffer_init(&state->lf_buffer);
    buffer_init(&state->hf_buffer);
    buffer_init(&state->uhf_buffer);

    return state;
}

static void state_free(RealityClockState* state) {
    free(state);
}

int32_t reality_clock_app(void* p) {
    UNUSED(p);

    RealityClockState* state = state_alloc();
    FuriMessageQueue* event_queue = furi_message_queue_alloc(INPUT_QUEUE_SIZE, sizeof(InputEvent));

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_display_backlight_enforce_on);

    InputEvent event;

    while(state->is_running) {
        /* Update readings */
        update_readings(state);
        view_port_update(view_port);

        /* Dynamic sample rate: faster during calibration */
        uint32_t interval = state->is_calibrated ?
            SAMPLE_INTERVAL_NORMAL_MS : SAMPLE_INTERVAL_CALIB_MS;

        if(furi_message_queue_get(event_queue, &event, interval) == FuriStatusOk) {
            process_input(state, &event);
        }
    }

    notification_message(notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);

    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);

    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    state_free(state);

    return 0;
}
