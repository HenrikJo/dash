#ifndef __VIEW_H__
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Telemetry fields count
#define FRAMES 100
#define TRACE_PATH "trace"
#define FORMAT_FILENAME "format"

void create_format_file(void);
int write_field(const char *dir, const char *name, float *data, size_t count);

#endif