#define MICROPY_PIN_DEFS_PORT_H "gpio.h"
#include "secure_iot.h"
// This file requires pin_defs_xxx.h (which has port specific enums and
// defines, so we include it here. It should never be included directly

//that gpio type pin_gpio_t 
#define GPIO_REG ((GPIO_Type*)(GPIO_BASE))
#define GPIO_PINMUX_REG ((GPIO_PINMUX_Type*)(GPIO_PINMUX_BASE))
typedef GPIO_Type pin_gpio_t;

#include MICROPY_PIN_DEFS_PORT_H
#include "py/obj.h"
//----------------------------------------------------------------------
typedef struct {
    mp_obj_base_t base;
    qstr name;
    uint8_t idx;
    uint8_t fn;
    uint8_t unit;
    uint8_t type;
    void *reg; //Hardware peripheral register associated // The peripheral associated with this AF
} pin_af_obj_t;

typedef struct {
    mp_obj_base_t base; //THe base adress -> Micropython object base
    qstr name; //The name of the pin?
    //uint32_t port   : 4; //Port number // Allows GPIOA through GPIOP
    uint32_t pin    : 6;  //Pin number in that port // 2^6 ,but only 45 are needed
    uint32_t num_af : 4; //NUmber of alternte function ->separate map
    uint32_t adc_channel : 5; //Adc channel number if the pin supports it// Some ARM processors use 32 bits/PORT
    uint32_t adc_num  : 3; //dc peripheral number // 1 bit per ADC
    uint32_t pin_mask; //pin mask number
    pin_gpio_t *gpio; //Pointer to the GPIO port's hardware structure
    const pin_af_obj_t *af; //Pointer to the array of alternate functions for this particular pin
} machine_pin_obj_t;
//------------------------------------------------------------------

extern const mp_obj_type_t pin_type;
extern const mp_obj_type_t pin_analog_type;
extern const mp_obj_type_t pin_af_type;

// Include all of the individual pin objects
//#include "genhdr/pins.h"

extern const mp_obj_type_t pin_board_pins_obj_type;
//extern const mp_obj_type_t pin_cpu_pins_obj_type;

extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;
extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;

MP_DECLARE_CONST_FUN_OBJ_KW(pin_init_obj);

void pin_init0(void);

#define IS_GPIO_AF(num)   (((num) > 0) && ((num) <= 5))


const machine_pin_obj_t *pin_find(mp_obj_t user_obj);
const machine_pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name);
const pin_af_obj_t *pin_find_af(const machine_pin_obj_t *pin, uint8_t fn, uint8_t unit);
const pin_af_obj_t *pin_find_af_by_index(const machine_pin_obj_t *pin, mp_uint_t af_idx);
const pin_af_obj_t *pin_find_af_by_name(const machine_pin_obj_t *pin, const char *name);

