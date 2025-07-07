#include "py/obj.h"
#include "pin.h"
#include "pin_defs_mind.h"
#include "gpio.h"
#include "secure_iot.h"
// Returns the pin mode. This value returned by this macro should be one of:
// GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD,
// GPIO_MODE_AF_PP, GPIO_MODE_AF_OD, or GPIO_MODE_ANALOG.

uint32_t pin_get_mode(const machine_pin_obj_t *pin) {
    uint32_t num =pin->gpio;
    //this gives us the gpio number
    uint8_t mode;
    if (pin->num_af ==0){
        if(num<32){
            uint32_t val =pow(2,num);
            mode =((uint32_t)GPIO_REG->GPIO_DIRECTION & val);
        }
        return mode;
    }
    else {
        return 2;
        //2 for alternate function.
    }
}

const pinmux_entry_t* get_pinmux_info(uint8_t pinmux_num) {
    //returns a pointer to that entry for future use
    for (int i = 0; i < sizeof(pinmux_table)/sizeof(pinmux_entry_t); ++i) {
        if (pinmux_table[i].pinmux_num == pinmux_num) {
            return &pinmux_table[i];
        }
    }
    return NULL; // not found
}


// Returns the af (alternate function) index currently set for a pin.

uint32_t pin_get_af(const machine_pin_obj_t *pin) {
    //return (pin->num_af);
    //This has to be defined based on baremetal pola
    //based on one of the 30 values we can know the alternate function
    uint32_t num=pin->gpio;
    for (int i = 0; i < sizeof(pinmux_table)/sizeof(pinmux_entry_t); ++i) {
        if (pinmux_table[i].gpio_num == num) {
            return pinmux_table[i].pinmux_num;
        }
    }
}
