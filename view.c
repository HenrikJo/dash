#include "view.h"

void create_format_file(void)
{
    FILE *f = fopen(TRACE_PATH "/" FORMAT_FILENAME, "wb");
    if (!f) {
        perror("fopen");
        return;
    }

    fprintf(f, "throttle RAW f 1\n");
    fprintf(f, "brake RAW f 1\n");
    fprintf(f, "gear RAW f 1\n");
    fprintf(f, "torque RAW f 1\n");
    fprintf(f, "speed RAW f 1\n");

    fclose(f);

}

int write_field(const char *dir, const char *name, float *data, size_t count) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", dir, name);
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror(path);
        return -1;
    }
    // write raw float array
    if (fwrite(data, sizeof(float), count, f) != count) {
        perror("write");
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}
