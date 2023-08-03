#include <cstdio>
#include <cstdlib>
#include "pico/stdlib.h"

#include "main.hxx"

int main() {
    srand(time_us_32());

    // Initialise the LED GPIO pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialise the I2C interface for the OLED display
    i2c_init(i2c0, 1000000);

    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_set_function(17, GPIO_FUNC_I2C);
    gpio_pull_up(16);
    gpio_pull_up(17);

    // Sleep for a short time to allow the display to initialise
    sleep_ms(250);

    SSD1306 display = SSD1306(i2c0, 0x3c, Size::W128xH32);
    display.setOrientation(false);

    auto cpu_values = (uint8_t*)calloc(GRAPH_BARS, sizeof(uint8_t));
    int cpu_value_count = GRAPH_BARS;
    auto mem_values = (uint8_t*)calloc(GRAPH_BARS, sizeof(uint8_t));
    int mem_value_count = GRAPH_BARS;

    int current_screen = display_mem;

    unsigned int prev_time = time_us_32();

    while (true) {
        // Clear the display
        display.clear();

        // Check which screen should be displayed
        switch (current_screen) {
            case display_cpu:
                display_graph_screen(&display, "CPU:", cpu_values, cpu_value_count);
                break;
            case display_mem:
                display_graph_screen(&display, "MEM:", mem_values, mem_value_count);
                break;
            default:
                continue;
        }

        // Send the data to the display
        display.sendBuffer();

        sleep_ms(500);

        // If more than SCREEN_TIME seconds have passed, move on to the next screen
        if (time_us_32() - prev_time > SCREEN_TIME * 1'000'000) {
            prev_time = time_us_32();

            current_screen++;

            if (current_screen == display_final) {
                current_screen = 0;
            }
        }

        // Update graph values (Will be replaced with serial comms code)
        update_graph_values(cpu_values, cpu_value_count, rand() % 101);
        update_graph_values(mem_values, mem_value_count, rand() % 101);
    }

    free(cpu_values);
    free(mem_values);

    return 0;
}

void update_graph_values(uint8_t* values, uint8_t num_values, uint8_t latest_value) {
    for (int i = num_values; i > 0; i--) {
        values[i] = values[i - 1];
    }

    values[0] = latest_value;
}

void draw_graph(SSD1306* display, uint8_t x_pos, uint8_t y_pos, const uint8_t* values, int num_values) {
    int bar_width = (SCREEN_WIDTH - x_pos) / num_values;

    // For each bar of the graph,
    for (int i = num_values; i > 0; i--) {
        // Draw each bar from bottom to top,
        for (int j = 0; j < (values[i] * SCREEN_HEIGHT) / 100; j++) {
            // Draw each row bar_width times
            for (int k = 0; k < bar_width; k++) {
                display->setPixel((int16_t)(x_pos + (bar_width * num_values) - (i * bar_width) - k), (int16_t)(y_pos + SCREEN_HEIGHT - j));
            }
        }
    }
}

void display_graph_screen(SSD1306* display, const char* label_text, const uint8_t* values, int num_values) {
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