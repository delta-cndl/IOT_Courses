#ifndef _KPRINTF_H_
#define _KPRINTF_H_

#include <stdarg.h>
#include <stdint.h>


/* Affiche sur la sortie série (UART) */
void kprintf(const char *fmt, ...);

#endif /* _KPRINTF_H_ */
