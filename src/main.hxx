#ifndef PICOLED_MAIN_HXX
#define PICOLED_MAIN_HXX

#include "ssd1306.h"
#include "textRenderer/TextRenderer.h"
#include "hardware/i2c.h"

#define LED_PIN PICO_DEFAULT_LED_PIN

#define GRAPH_BARS 40
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_TIME 10

using namespace pico_ssd1306;

enum {
    display_cpu = 0,
    display_mem,
    display_final,
};

/// \brief Update the values in graph_values, shifting each existing value by 1 and adding latest_value to the 0th position
/// \param values Pointer to the graph values to be updated
/// \param num_values The number of graph values
/// \param latest_value The new value to be added to the 0th position
void update_graph_values(uint8_t* values, uint8_t num_values, uint8_t latest_value);

/// \brief Draw a graph on the screen, at the given x and y position
/// \param display Pointer to the display to be drawn to
/// \param x_pos The x position of the top-left corner of the graph
/// \param y_pos The y position of the top-left corner of the graph
/// \param values Pointer to the graph values to be drawn
/// \param num_values The number of graph values
void draw_graph(SSD1306* display, uint8_t x_pos, uint8_t y_pos, const uint8_t* values, int num_values);

/// \brief Draw the entire graph screen (draw label, percentage and graph)
/// \param display Pointer to the display to be drawn to
/// \param label_text String containing the label to be drawn above the current graph value (Maximum 6 characters)
/// \param values Pointer to the graph values to be drawn
/// \param num_values The number of graph values
void display_graph_screen(SSD1306* display, const char* label_text, const uint8_t* values, int num_values);

#endif //PICOLED_MAIN_HXX