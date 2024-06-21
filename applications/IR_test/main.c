#include <stdio.h>
#include <libpynq.h>

int main() {
    pynq_init();
    gpio_init();

    // Set AR7 as input
    gpio_set_direction(IO_AR7, GPIO_DIR_INPUT);

    while (1) {
        // Read the digital value from AR7
        int value = gpio_get_level(IO_AR7);
        printf("Value: %d\n", value);
        sleep_msec(300); 
    }

    gpio_destroy();
    pynq_destroy();
    return 0;
}

