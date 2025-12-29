#include <math.h>
#include <stdio.h>

struct rpm_torque {
    float rpm;
    float torque;
};

struct rpm_torque rpm_torque_yamaha_fjr_1300[] = {
    { .rpm = 0, .torque = 0 },
    { .rpm = 1600, .torque = 45 },
    { .rpm = 2000, .torque = 62 },
    { .rpm = 3000, .torque = 73 },
    { .rpm = 4000, .torque = 80 },
    { .rpm = 5000, .torque = 85 },
    { .rpm = 6000, .torque = 85 },
    { .rpm = 6500, .torque = 90 },
    { .rpm = 7000, .torque = 89 },
    { .rpm = 8000, .torque = 80 },
    { .rpm = 9000, .torque = 65 },
    { .rpm = 9500, .torque = 0 }
};

float gear_ratio_yamaha_fjr_1300[] = { 2.5, 1.722, 1.35, 1.111, 0.962, 0.846 };

struct vehicle_info {
    unsigned int gears;
    float *gear_ratio;
    struct rpm_torque *torque_at_rpm;
    float wheel_diameter;
    float rotor_mass;
    float body_mass;
};

float torque_at_rpm(struct vehicle_info *self, float rpm)
{
    /* Assume sorted rpm */
    /* Find closest rpm */
    struct rpm_torque *rpm_tq_pnt = self->torque_at_rpm;
    struct rpm_torque *prior_pnt = NULL;
    while (rpm_tq_pnt != NULL) {
        if (rpm_tq_pnt->rpm > rpm) {
            /* Found greater rpm, check if prior value exists and where between the torque should be located */
            if (prior_pnt == NULL) {
                /* Nothing prior to present value, use this one as is */
                return rpm_tq_pnt->torque;
            }

            /* Prior value exits interpolate the torque from the two values */
            float upper = rpm_tq_pnt->rpm - prior_pnt->rpm;
            float actual = rpm - prior_pnt->rpm;
            float scale = actual / upper; /* How close is the value to the upper or lower value, use this to scale the torque */

            /* Make sure range of scale is 0 -> 1 */
            scale = fmin(1.0f, scale);
            scale = fmax(0.0f, scale);

            float scaled_torque = prior_pnt->torque - rpm_tq_pnt->torque;

        }
    }

    /* Failed to find rpm within range, assume 0 Nm */
    return 0.0f;
}

float rpm_to_rev(float rpm, unsigned int gear)
{
    return 0.0f;
}

int main(void) 
{
    struct vehicle_info yamaha_fjr_1300;
    yamaha_fjr_1300.gears = sizeof(gear_ratio_yamaha_fjr_1300) / sizeof(float);
    yamaha_fjr_1300.gear_ratio = gear_ratio_yamaha_fjr_1300;

    printf("Gears: %d\n", yamaha_fjr_1300.gears);
    for (int i = 0; i < yamaha_fjr_1300.gears; i++) {
        printf("Ratio at gear %d: %f\n", i+1, yamaha_fjr_1300.gear_ratio[i]);
    }



	return 0;
}
