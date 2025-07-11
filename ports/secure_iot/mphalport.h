#include "py/mpconfig.h"
#include "py/mphal.h"
//Its there only :)
#include "time.h"
#include "py/obj.h"
//static inline void mp_hal_set_interrupt_char(int c);

mp_uint_t mp_hal_ticks_ms(void);
mp_uint_t mp_hal_ticks_us(void);
void mp_hal_delay_ms(mp_uint_t ms);
void mp_hal_delay_us(mp_uint_t us);

void mp_hal_set_interrupt_char(int c);

int mp_hal_stdin_rx_chr(void);
// Send the string of given length.
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len);
mp_uint_t mp_hal_time_ns(void);
mp_obj_t mp_time_time_get(void);
uint64_t mp_hal_time_ns_rtc(uint8_t i2c_number);