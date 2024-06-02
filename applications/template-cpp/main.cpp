extern "C" {
  #include <libpynq.h>
  #include <stdio.h>
}

int main(void) {
  pynq_init();

  printf("Hello, world!\n");

  // your code here

  pynq_destroy();
  return EXIT_SUCCESS;
}
