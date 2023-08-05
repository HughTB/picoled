#include <cstdio>
#include "displays.hxx"

void draw_graph(SSD1306* display, uint8_t x_pos, uint8_t y_pos, const uint8_t* values, size_t num_values) {
    uint bar_width = (SCREEN_WIDTH - x_pos) / num_values;

    // For each bar of the graph,
    for (size_t i = num_values; i > 0; i--) {
        // Draw each bar from bottom to top,
        for (int j = 0; j < (values[i] * SCREEN_HEIGHT) / 100; j++) {
            // Draw each row bar_width times
            for (int k = 0; k < bar_width; k++) {
                display->setPixel((int16_t)(x_pos + (bar_width * num_values) - (i * bar_width) - k), (int16_t)(y_pos + SCREEN_HEIGHT - j));
            }
        }
    }
}

void display_graph_screen(SSD1306* display, const char* label_text, const uint8_t* values, size_t num_values) {
    // Create the bottom text using the first value in the list, displaying a percent sign if <100, or just the number 100
    char bottom_text[4];
    snprintf(bottom_text, 4, "%2d%%", values[0]);

    // Ensure that only 6 characters (+ null terminator) from label_text are drawn to the display
    char top_text[7];
    snprintf(top_text, 7, "%s", label_text);

    // Draw the label text and current percentage
    drawText(display, font_8x8, top_text, 0, 0);
    drawText(display, font_16x32, bottom_text, 0, 6);

    // Draw the graph
    draw_graph(display, 48, 0, values, num_values);
}

void display_generic_screen(SSD1306* display, const char* text, bool large) {
    if (large) {
        // Ensure only up to 8 characters (and null-terminator) are drawn to the screen
        char displayed_text[9];
        snprintf(displayed_text, 9, "%s", text);

        drawText(display, font_16x32, displayed_text, 0, 0);
    } else {
        // Ensure only the first 10 characters are drawn to the screen
        char top_text[11];
        snprintf(top_text, 11, "%s", text);

        drawText(display, font_12x16, top_text, 0, 0);

        if (strlen(text) > 10) {
            // Grab the next upto 10 characters
            char bottom_text[11];
            snprintf(bottom_text, 11, "%s", text + 10);

            drawText(display, font_12x16, bottom_text, 0, 16);
        }
    }
}