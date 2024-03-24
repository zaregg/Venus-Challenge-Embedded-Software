#include <arm_shared_memory_system.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <libpynq.h>
#include <platform.h>
#include <stdint.h>
#include <stepper.h>

void uart_read_array(const int uart, uint8_t *buf, uint8_t l) {
  for (uint8_t x = 0; x < l; x++) {
    buf[x] = uart_recv(uart);
  }
}

int main(void) {
  pynq_init();
  switchbox_set_pin(IO_AR0, SWB_UART0_RX);
  switchbox_set_pin(IO_AR1, SWB_UART0_TX);
  gpio_set_direction(IO_AR2, GPIO_DIR_INPUT);
  gpio_set_direction(IO_AR3, GPIO_DIR_INPUT);
  printf("AR2: %d\n", gpio_get_level(IO_AR2));
  printf("AR3: %d\n", gpio_get_level(IO_AR3));

  uart_init(UART0);

  uart_reset_fifos(UART0);

  stepper_init();

  stepper_enable();

  stepper_set_speed(5000, 5000);

  stepper_steps(1600, -1600);



  // while (1) {
  //   if (uart_has_data(UART0)) {
  //     uint32_t size = 0;
  //     uart_read_array(UART0, &size, 4);
  //     char array[size];
  //     uart_read_array(UART0, &array, size);
  //     printf("data: %.*s\n", size, array);

  //     json_tokener *tok = json_tokener_new();

  //     json_object *root = json_tokener_parse_ex(tok, array, size);
  //     if (root) {
  //       int16_t l = 0, r = 0;
  //       json_object *lo = json_object_object_get(root, "left");
  //       if (lo) {
  //         l = json_object_get_int(lo);
  //       }
  //       json_object *ro = json_object_object_get(root, "right");
  //       if (ro) {
  //         r = json_object_get_int(ro);
  //       }
  //       json_object *so = json_object_object_get(root, "lspeed");
  //       json_object *sor = json_object_object_get(root, "rspeed");
  //       if (so && sor) {
  //         uint16_t s = json_object_get_int(so);
  //         uint16_t sr= json_object_get_int(sor);
  //         stepper_set_speed(s, sr);
  //       }
  //       printf("%d %d\n", l, r);
  //       stepper_steps(l, r);
  //       json_object_put(root);
  //     }
  //     json_tokener_free(tok);
  //   }

  //   usleep(1 * 1000);
  // }

  while (!stepper_steps_done())
    ;

  stepper_destroy();

  pynq_destroy();
  return EXIT_SUCCESS;
}