#ifndef PICOLED_DISPLAYS_HXX
#define PICOLED_DISPLAYS_HXX

#include "ssd1306.h"
#include "textRenderer/TextRenderer.h"
#include "hardware/i2c.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

using namespace pico_ssd1306;

enum {
    display_cpu = 0,
    display_mem,
    display_temps,
    display_final,
};

/// \brief Draw a graph on the screen, at the given x and y position
/// \param display Pointer to the display to be drawn to
/// \param x_pos The x position of the top-left corner of the graph
/// \param y_pos The y position of the top-left corner of the graph
/// \param values Pointer to the graph values to be drawn
/// \param num_values The number of graph values
void draw_graph(SSD1306* display, uint8_t x_pos, uint8_t y_pos, const uint8_t* values, size_t num_values);

/// \brief Draw the entire graph screen (draw label, percentage and graph)
/// \param display Pointer to the display to be drawn to
/// \param label_text String containing the label to be drawn above the current graph value (Maximum 6 characters)
/// \param values Pointer to the graph values to be drawn
/// \param num_values The number of graph values
void display_graph_screen(SSD1306* display, const char* label_text, const uint8_t* values, size_t num_values);

/// \brief Display text on the entire screen
/// \param display Pointer to the display to be drawn to
/// \param text The text to be drawn to the display (max 20 small or 8 large characters)
/// \param large Should the text be large?
void display_generic_screen(SSD1306* display, const char* text, bool large = false);

#endif //PICOLED_DISPLAYS_HXX
