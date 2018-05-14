#ifndef _EE_PRINTF_H
#define _EE_PRINTF_H

#include <stdarg.h>
#include "string_lib.h"
#include "utils.h"


int ee_printf(const char *fmt, ...);

static int ee_vsprintf(char *buf, const char *fmt, va_list args);

static char *flt(char *str, double num, int size, int precision, char fmt, int flags);

static void cropzeros(char *buffer);

static void decimal_point(char *buffer);

static void parse_float(double value, char *buffer, char fmt, int precision);

void ee_bufcpy(char *pd, char *ps, int count);

static char *iaddr(char *str, unsigned char *addr, int size, int precision, int type);

static char *eaddr(char *str, unsigned char *addr, int size, int precision, int type);

static char *number(char *str, long num, int base, int size, int precision, int type);

static int skip_atoi(const char **s);

static size_t strnlen(const char *s, size_t count);

#endif