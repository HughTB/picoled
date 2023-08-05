#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "pico/stdlib.h"

#include "main.hxx"

int main() {
    // Initialise serial port for logging
    stdio_init_all();

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
    size_t cpu_value_count = GRAPH_BARS;
    auto mem_values = (uint8_t*)calloc(GRAPH_BARS, sizeof(uint8_t));
    size_t mem_value_count = GRAPH_BARS;

    int current_screen = display_mem;

    auto message_buffer = (char*)calloc(MESSAGE_BUFFER_SIZE, sizeof(char));

    uint64_t prev_time = time_us_64();

    uint64_t frame_start_time;
    uint64_t frame_end_time;

    printf("PicOLED successfully initialised\n");

// Ignore endless loop warning; we want this to run forever!
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        frame_start_time = time_us_64();

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

        // Attempt to read a message from the serial interface, timeout after using half of the frame-time
        if (read_line_timeout(TARGET_FRAME_TIME / 2, message_buffer, MESSAGE_BUFFER_SIZE) > 0)
            parse_message(message_buffer, cpu_values, cpu_value_count, mem_values, mem_value_count);
        else {
            // At least for now, if no message is received, just shift the same value into the graph again
            update_graph_values(cpu_values, cpu_value_count, cpu_values[0]);
            update_graph_values(mem_values, mem_value_count, mem_values[0]);
        }

        // If more than SCREEN_TIME seconds have passed, move on to the next screen
        if (time_us_64() - prev_time > SCREEN_TIME * 1'000'000) {
            prev_time = time_us_64();

            current_screen++;

            if (current_screen == display_final) {
                current_screen = 0;
            }
        }

        frame_end_time = time_us_64();
        // Sleep for the remaining time to make up TARGET_FRAME_TIME
        sleep_us((TARGET_FRAME_TIME * 1000) - (frame_end_time - frame_start_time));
    }
#pragma clang diagnostic pop

    free(cpu_values);
    free(mem_values);
    free(message_buffer);

    return 0;
}

void update_graph_values(uint8_t* values, size_t num_values, uint8_t latest_value) {
    for (size_t i = num_values; i > 0; i--) {
        values[i] = values[i - 1];
    }

    values[0] = latest_value;
}

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

int read_line_timeout(uint timeout_ms, char* buffer, size_t buffer_size) {
    // Wait the full timeout while attempting to get the first character in the message
    char current_char = getchar_timeout_us(timeout_ms * 1000);
    int i = 0;

    // While the end of line is not reached, no timeout occurs, and there is space in the buffer, read characters
    while (current_char != '\n' && current_char != (char)PICO_ERROR_TIMEOUT && i < buffer_size - 1) {
        buffer[i] = current_char;
        i++;
        current_char = getchar_timeout_us(10); // Get the next char with a very short timeout to prevent stalling
    }

    buffer[i] = '\0'; // Ensure the string is correctly null-terminated

    if (current_char == (char)PICO_ERROR_TIMEOUT) return -1; // Tell the caller that a timeout occurred before EOL

    return i; // Tell the caller how many characters were read
}

void parse_message(char* message, uint8_t* cpu_values, size_t cpu_value_count, uint8_t* mem_values, size_t mem_value_count) {
    char* key_value_pairs[8] = {nullptr};

    // Split the message into the upto 8 key-value pairs
    key_value_pairs[0] = strtok(message, ",");

    for (int i = 1; i < 8; i++) {
        key_value_pairs[i] = strtok(nullptr, ",");
    }

    // Go through each key-value pair and split the key from the value and update the graphs if applicable
    for (auto & key_value_pair : key_value_pairs) {
        char* key = strtok(key_value_pair, ":");
        char* value = strtok(nullptr, ":");

        // If either the key is null, skip this key-value pair
        if (key == nullptr) continue;

        // This is going to be very long and ugly, but C++ cannot switch case on strings, so there you go
        if (!strcmp(key, "cpu")) {
            update_graph_values(cpu_values, cpu_value_count, strtol(value, nullptr, 10));
        } else if (!strcmp(key, "mem")) {
            update_graph_values(mem_values, mem_value_count, strtol(value, nullptr, 10));
        } else if (!strcmp(key, "ident")) {
            std::cout << "miaow" << std::endl;
        }
    }
}