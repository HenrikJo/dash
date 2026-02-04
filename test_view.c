#include "view.h"

int main(int argc, char **argv) {
    const char *outdir = TRACE_PATH;
    float throttle[FRAMES], brake[FRAMES], gear[FRAMES];
    float torque[FRAMES], speed[FRAMES];

    // Create output directory
    if (mkdir(outdir, 0755) && errno != EEXIST) {
        perror("mkdir");
        return 1;
    }
    create_format_file();

    // Fill example data
    for (int i = 0; i < FRAMES; i++) {
        throttle[i] = (float)(i % 101) / 100.0f;   // 0.0 … 1.0
        brake[i]    = (float)((i * 2) % 101) / 100.0f;
        gear[i]     = (float)((i % 6) + 1);       // gears 1 … 6
        torque[i]   = 50.0f + 10.0f * (float)i;
        speed[i]    = 0.5f * (float)i;
    }

    // Write each field; order *not* critical for raw files, but format file should have the reference last
    if (write_field(outdir, "throttle", throttle, FRAMES)) return 1;
    if (write_field(outdir, "brake",    brake,    FRAMES)) return 1;
    if (write_field(outdir, "gear",     gear,     FRAMES)) return 1;
    if (write_field(outdir, "torque",   torque,   FRAMES)) return 1;
    if (write_field(outdir, "speed",    speed,    FRAMES)) return 1;

    // The 'format' file should be created *once* per directory manually
    printf("Telemetry written to %s/ (throttle, brake, gear, torque, speed)\n", outdir);
    printf("Add 'format' file as described to interpret them as floats.\n");
    return 0;
}
