#ifndef PICOLED_MAIN_HXX
#define PICOLED_MAIN_HXX

#include "ssd1306.h"
#include "textRenderer/TextRenderer.h"
#include "hardware/i2c.h"

#define LED_PIN PICO_DEFAULT_LED_PIN

#define GRAPH_BARS 40
// How long each screen should be displayed (seconds)
#define SCREEN_TIME 5

// The size of the serial read buffer (512 + 1)
#define MESSAGE_BUFFER_SIZE 513

#define TARGET_FRAME_TIME 500
// The number of missed reads before going to sleep
#define SLEEP_AFTER_MISSED_READS 10

/// \brief Update the values in graph_values, shifting each existing value by 1 and adding latest_value to the 0th position
/// \param values Pointer to the graph values to be updated
/// \param num_values The number of graph values
/// \param latest_value The new value to be added to the 0th position
void update_graph_values(uint8_t* values, size_t num_values, uint8_t latest_value);

/// \brief Attempt to read a line from the serial port. Fail if timeout_ms is reached before any data is received
/// \param timeout_ms Time in milliseconds to wait before timing out
/// \param buffer The buffer to read data into
/// \param buffer_size The size of the read buffer
int read_line_timeout(uint timeout_ms, char* buffer, size_t buffer_size);

/// \brief Attempt to parse a message from the serial interface, updating the relevant values
/// \param message A pointer to the message to be parsed
/// \param cpu_values A pointer to the cpu graph values
/// \param cpu_value_count A pointer to the number of cpu graph values
/// \param mem_values A pointer to the mem graph values
/// \param mem_value_count A pointer to the number of mem graph values
void parse_message(char* message, uint8_t* cpu_values, size_t cpu_value_count, uint8_t* mem_values, size_t mem_value_count);

#endif //PICOLED_MAIN_HXX