
#ifndef SERIAL_H
#define SERIAL_H

#include <sys/types.h>

#define UART_16550_MINOR_MAX 10

int32_t UART_16550_init(void *address);
int32_t UART_16550_putchar(uint16_t minor, char c);
int32_t UART_16550_write(uint16_t minor, uint8_t *msgptr, uint32_t len);
uint8_t UART_16550_getchar(uint16_t minor);
int32_t UART_16550_read(uint16_t minor, uint8_t *msgptr, uint32_t len);

#endif
