
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_HAS_SWITCH       (0)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LED_BLUE)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// RTC
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (1)


// UART config
#define MICROPY_HW_UART1_TX     (8)
#define MICROPY_HW_UART1_RX     (9)

#define MICROPY_HW_UART2_TX     (11)
#define MICROPY_HW_UART2_RX     (15)

#define MICROPY_HW_UART_REPL        (PYB_UART_1)
#define MICROPY_HW_UART_REPL_BAUD   (115200)

