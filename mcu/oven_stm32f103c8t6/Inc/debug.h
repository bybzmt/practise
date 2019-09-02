#ifndef __DEBUG_H
#define __DEBUG_H

void Error_Handler(char *msg);

void printHex(const char *msg, const uint8_t *data, size_t len);
void byte2hex(const uint8_t *source, uint8_t *dest, size_t len);
void hex2byte(const uint8_t *source, uint8_t *dest, size_t len);

#endif
