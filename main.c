#include "model.h"
#include <stdio.h>

int main(void)
{
    struct vehicle_info *yamaha_fjr_1300 = create_vehicle();

    printf("Gears: %d\n", yamaha_fjr_1300->gears);
    for (int i = 0; i < yamaha_fjr_1300->gears; i++) {
        yamaha_fjr_1300->selected_gear = i;
        printf("Ratio at gear %d: %f total ratio:%f\n", yamaha_fjr_1300->selected_gear, yamaha_fjr_1300->gear_ratio[i], get_total_gear_ratio(yamaha_fjr_1300));
    }

    for (float rpm = 0.0f; rpm < 10000.0f; rpm += 100.0f) {
        float torque = torque_at_rpm(yamaha_fjr_1300, rpm);
        printf("Engine RPM: %f rpm, torque: %f Nm\n", rpm, torque);
    }

    /* Check wheel torque at peak torque rpm */
    float rpm = 6500.0f;
    yamaha_fjr_1300->clutch_engaged = 1;
    for (unsigned int gear = 0; gear < yamaha_fjr_1300->gears; gear++) {
        printf("Set gear: %u\n", gear);
        set_gear(yamaha_fjr_1300, gear); 
        float wheel_torque = get_wheel_torque(yamaha_fjr_1300, rpm, gear);
        float vehicle_motor_force = wheel_torque / yamaha_fjr_1300->wheel_diameter / 2.0f;
        printf("Engine RPM: %f rpm, wheel torque: %f Nm, force: %fN\n", rpm, wheel_torque, vehicle_motor_force);
    }

    /* Check rpm and speed when disengaged clutch */
    yamaha_fjr_1300->vehicle_speed = 100.0f;
    yamaha_fjr_1300->engine_rpm = 2000.0f;
    yamaha_fjr_1300->throttle = 1.0f;
    yamaha_fjr_1300->clutch_engaged = 0;
    for (int i = 0; i < 50; i++) {
        printf("Speed %f, rpm %f\n", yamaha_fjr_1300->vehicle_speed, yamaha_fjr_1300->engine_rpm);
        update_speed(yamaha_fjr_1300, 0.1f);
    }

    yamaha_fjr_1300->throttle = 0.0f;
    for (int i = 0; i < 50; i++) {
        printf("Speed %f, rpm %f\n", yamaha_fjr_1300->vehicle_speed, yamaha_fjr_1300->engine_rpm);
        update_speed(yamaha_fjr_1300, 0.1f);
    }

    /* Calculate speed going from disengaged to engaged clutch */
    yamaha_fjr_1300->selected_gear = 1;
    engage_clutch(yamaha_fjr_1300);

    printf("\nSimulate no throttle\n");
    yamaha_fjr_1300->throttle = 0.0f;
    for (int i = 0; i < 100; i++) {
        printf("Speed %f, rpm %f\n", yamaha_fjr_1300->vehicle_speed, yamaha_fjr_1300->engine_rpm);
        update_speed(yamaha_fjr_1300, 0.1f);
    }

    printf("\nSimulate full throttle\n");
    yamaha_fjr_1300->throttle = 1.0f;
    for (int i = 0; i < 20; i++) {
        printf("Speed %f, rpm %f\n", yamaha_fjr_1300->vehicle_speed, yamaha_fjr_1300->engine_rpm);
        update_speed(yamaha_fjr_1300, 0.1f);
    }

    printf("Go through the gears\n");
    for (unsigned int gear = 1; gear <= yamaha_fjr_1300->gears - 1; gear++) {
        printf("Set gear: %u\n", gear);
        set_gear(yamaha_fjr_1300, gear);
        for (int i = 0; i < 70; i++) {
            printf("Speed %f km/h, rpm %f torque: %f Nm\n", yamaha_fjr_1300->vehicle_speed, yamaha_fjr_1300->engine_rpm, torque_at_rpm(yamaha_fjr_1300, yamaha_fjr_1300->engine_rpm));
            update_speed(yamaha_fjr_1300, 0.1f);
        }
    }

	return 0;
}
