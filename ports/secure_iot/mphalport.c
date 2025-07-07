
// mphalport.c
#include "mphalport.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>      // Optional if you're using open()
#include <sys/types.h>  // Optional
#include <sys/stat.h>   // Optional
#include "utils.h"
#include "uart.h"
#include "py/mpconfig.h"
#include "shared/runtime/pyexec.h"

void mp_hal_delay_ms(mp_uint_t ms) {
    // stub implementation, replace with your delay code
    //printf("Delay ms: %lu\n", ms);
    //From the hard ware millis function as the reference
    millis_init();
    uint64_t a =millis();
    ms =(uint64_t)ms;
    while((millis()-a) <ms){
        //eat five star do nothing :)
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    // stub implementation
    //printf("Delay us: %lu\n", us);
    //DElay in us is ms *1000
    uint64_t a=millis();
    us =(uint64_t)(us/1000);
    while((millis()-a ) <us){
        //Eat five star do nothing :)
    }

}

void mp_hal_set_interrupt_char(int c) {
    // Implementation or empty stub if not supported
}

int mp_hal_stdin_rx_chr(void) {
    // unsigned char c = 0;
    // int r = read(STDIN_FILENO, &c, 1); //POSIX sytemcall
    // (void)r;
    // return c;
    int status;
    status=getchar();
    return status;
}

// Send the string of given length.
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
//     int r = write(STDOUT_FILENO, str, len);
//     (void)r;
//So the pointer points to the entire string
    if(str !=NULL){
    //len =(int) len;
    //not necessary pola -->might cause issues + overlap
    for(int i=0;i<len;i++){
        int value= (int) str[i];
        putchar(value);
        //The value that needs to take integer value
    }
    return len;
    }
    return 0;
 }

mp_uint_t mp_hal_ticks_ms(void) {
    //return esp_timer_get_time() / 1000;
    //time_t *p_timer;
    uint64_t value;
    value=millis();
    return (mp_uint_t)value;

}

mp_uint_t mp_hal_ticks_us(void) {
    //return esp_timer_get_time();
    uint32_t value;
    value=micros();
    return (mp_uint_t)value;
}

// static inline void mp_hal_set_interrupt_char(char c){
//     //no action
// }
//--------------------------------------------------------
//Newly added mphal functions
// static mp_obj_t mp_time_time_get(void) {

//     return mp_obj_new_int(mp_hal_ticks_ms(void)/ 1000);
// }

// uint64_t mp_hal_time_ns(void) {
//     return k_ticks_to_ns_near64(k_uptime_ticks());
// }