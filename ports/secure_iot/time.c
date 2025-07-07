#include <time.h>
#include<stdint.h>
#include <stddef.h>  // for NULL

// Dummy implementation of time() that always returns a fixed value
// (for example, zero) if no real clock is available.
time_t time(time_t *timer) {
    time_t now = 0;  // Replace this with real time logic if available

    if (timer != NULL) {
        *timer = now;
    }

    return now;
}


// millis() derived from time()
uint32_t millis_s(void) {
    return (uint32_t)time(NULL) * 1000;
}

// micros() derived from time()
uint32_t micros(void) {
    return (uint32_t)time(NULL) * 1000000;
}