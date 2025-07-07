#include <stdint.h>
#include "secure_iot.h"

// The HAL uses a slightly different naming than we chose, so we provide
// some #defines to massage things. Also I2S and SPI share the same
// peripheral.

typedef struct {
    uint8_t pinmux_num;
    //to signify the type of the special function.
    uint8_t gpio_num;
    const char *gpio_pin; //signifies the text for printing
    const char *peripheral2;
} pinmux_entry_t;

static const pinmux_entry_t pinmux_table[] = {
    {0, 0, "GPIO 0",     "PWM 0"},
    {1, 1, "GPIO 1",     "PWM 1"},
    {2, 2,"GPIO 2", "PWM 2"},
    {3,3, "GPIO 3", "PWM 3"},
    {4, 4,"GPIO 4", "PWM 4"},
    {5, 5,"GPIO 5", "PWM 5"},
    {6, 6,"GPIO 6", "PWM 6"},
    {7, 7,"GPIO 7", "PWM 7"},
    {8, 8, "GPIO 8",     "PWM 8"},
    {14, 32,"GPIO 32", "SPI2_MOSI"},
    {15,33, "GPIO 33", "SPI2_MISO"},
    {16, 34,"GPIO 34", "SPI2_NCS"},
    {17,35, "GPIO 35", "SPI3_MOSI"},
    {18, 36,"GPIO 36", "SPI3_MISO"},
    {19, 37,"GPIO 37", "SPI3_NCS"},
    {20, 8,"GPIO 8",     "UART3_TX"},
    {21, 9,"GPIO 9",     "UART3_RX"},
    {22, 11,"GPIO11",     "UART4_TX"},
    {23, 15,"GPIO15",     "UART4_RX"},
    {24, 38,"GPIO 38", "GPTIMER0"},
    {25, 39,"GPIO 39", "GPTIMER1"},
    {26, 40,"GPIO 40", "GPTIMER2"},
    {27, 41,"GPIO 41", "GPTIMER3"},
    {28, 42,"GPIO 42", "JTAG_TDI"},
    {29, 43,"GPIO 43", "JTAG_TMS"},
    {30, 44, "GPIO 44", "JTAG_TDO"},
};
//defined by meee !
typedef struct {
    uint8_t gpio_num;
    const char *peripheral2;
    uint8_t alt_func_num;
} alt_func;

static const alt_func alt_func_table[]{
    //0 ->only gpio
    {0,"PWM",1}, //pwm config
    {1,"PWM",1},
    {2,"PWM",1},
    {3,"PWM",1},
    {4,"PWM",1},
    {5,"PWM",1},
    {6,"PWM",1},
    {7,"PWM",1},
    {8,"UART",5},
    {9,"UART",5},
    {10,"GPIO",0},
    {11,"UART",5},
    {12,"GPIO",0},
    {13,"GPIO",0},
    {14,"GPIO",0},
    {15,"UART",5},
    {16,"GPIO",0},
    {17,"PWM",1},
    {18,"PWM",1},
    {19,"PWM",1},
    {20,"PWM",1},
    {21,"PWM",1},
    {22,"PWM",1},
    {23,"GPIO",0},
    {24,"GPIO",0},
    {25,"GPIO",0},
    {26,"GPIO",0},
    {27,"GPIO",0},
    {28,"GPIO",0},
    {29,"GPIO",0},
    {30,"GPIO",0},
    {31,"GPIO",0},
    {32,"SPI",2},
    {33,"SPI",2},
    {34,"SPI",2},
    {35,"SPI",2},
    {36,"SPI",2},
    {37,"SPI",2},
    {38,"GPTIMER",3},
    {39,"GPTIMER",3},
    {40,"GPTIMER",3},
    {41,"GPTIMER",3},
    {42,"JTAG",4},
    {43,"JTAG",4},
    {44,"JTAG",4}
}
#define  GPIO_MODE_INPUT                    0 /*!< Input Floating Mode                   */
#define  GPIO_MODE_OUTPUT                   1 /*!< Output  Mode                 */              
#define  GPIO_MODE_AF                       2/*!< Alternate Function Push Pull Mode     */
//machine_pins array
    
uint32_t pin_get_mode(const machine_pin_obj_t *pin);
const pinmux_entry_t* get_pinmux_info(uint8_t pinmux_num);
uint32_t pin_get_af(const machine_pin_obj_t *pin);
typedef GPIO_Type pin_gpio_t;
//THe internal defined struct of the gpio register