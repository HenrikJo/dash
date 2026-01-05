#include <assert.h>
#include <math.h>
#include <stdio.h>

#define debug 0

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

/* Index 0 is used for neutral */
float gear_ratio_yamaha_fjr_1300[] = { 0.0f,  2.5f, 1.722f, 1.35f, 1.111f, 0.962f, 0.846f };

struct vehicle_info {
    /* Static info */
    unsigned int gears; /* Number of gears, 0 is neutral */
    float *gear_ratio;

    unsigned int speed_torque_values;
    struct rpm_torque *torque_at_rpm;

    /* Engine specific */
    float rotor_mass;
    float rotor_radius;
    float rotor_drag_torque;
    float idle;
    float primary_gear_reduction; /* Gear reduction from rotor to gearbox input */
    float final_gear_reduction; /* Gear reduction from gearbox output to wheel */

    /* Vehicle specific */
    float wheel_diameter;
    float body_mass;
    float wind_coefficient;

    /* Dynamic state */
    unsigned int selected_gear;

    float engine_rpm;
    float vehicle_speed;

    float throttle; /* 0 -> 1 for 0% -> 100% of available torque */
    float brake;

    unsigned int clutch_engaged; /* Is the clutch engaged, 0 for disengaged and 1 for engaged */
};

float torque_at_rpm(struct vehicle_info *self, float rpm)
{
    /* Assume sorted rpm */
    /* Find closest rpm */
    struct rpm_torque *rpm_tq_pnt = self->torque_at_rpm;
    struct rpm_torque *prior_pnt = NULL;
    for (unsigned int i = 0; i < self->speed_torque_values; i++) {
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
            assert(scale <= 1.0f);
            assert(scale >= 0.0f);
            scale = fmin(1.0f, scale);
            scale = fmax(0.0f, scale);

            float scaled_torque = prior_pnt->torque * (1.0f - scale) + rpm_tq_pnt->torque * scale;
            return scaled_torque;
        }
        prior_pnt = rpm_tq_pnt;
        rpm_tq_pnt++;
    }

    if (debug > 0) {
        printf("Failed to find rpm within range, assume 0 Nm\n");
    }

    return 0.0f;
}

float get_wheel_torque(struct vehicle_info *self, float rpm, unsigned int gear) 
{
    if (gear == 0 || gear > self->gears) {
        /* Invalid gear or neutral selected, no torque */
        return 0.0f;
    }

    float motor_torque = torque_at_rpm(self, rpm);

    return motor_torque * self->gear_ratio[gear];
}

float rpm_to_rev(float rpm, unsigned int gear)
{
    return 0.0f;
}

float calculate_acceleration(float torque, float mass, float deltatime)
{
    float acceleration = (torque / mass) * deltatime;
}

float rads_to_rpm(float rads)
{
    return 60.0f * rads / (2.0f * M_PI);
}

float rpm_to_rads(float rpm) 
{
    return 2.0f * M_PI * rpm / 60.0f;
}

float kmh_to_ms(float kmh)
{
    return kmh / 3.6f;
}

float kmh_to_motor_side_rpm(struct vehicle_info *self)
{
    float speed_ms = kmh_to_ms(self->vehicle_speed);
    float wheel_circumference = self->wheel_diameter * M_PI;
    float wheel_rpm = (speed_ms * 60.0f) / wheel_circumference;
    float gear_ratio = self->primary_gear_reduction + self->gear_ratio[self->selected_gear] + self->final_gear_reduction;
    printf("speed_ms:%f\n", speed_ms);
    printf("wheel_rpm:%f\n", wheel_rpm);
    printf("self->selected_gear: %u\n", self->selected_gear);
    printf("gear_ratio: %f\n", gear_ratio);
    if (gear_ratio <= 0.0f) {
        return 0.0f;
    }
    return wheel_rpm * gear_ratio;
}

float motor_side_rpm_to_kmh(struct vehicle_info *self) {
    /* Todo here continue */
}

float calc_moment_of_inertia(float mass, float radius)
{
    return mass * radius * radius / 2.0f;
}

float calc_kinetic_energy(float mass, float velocity)
{
    return mass * velocity * velocity / 2.0f;
}

float calc_mass(float kinetic_energy, float velocity)
{
    return kinetic_energy * 2.0f / (velocity * velocity);
}

float calculate_velocity(float kinetic_energy, float mass)
{
    return sqrtf((kinetic_energy * 2.0f) / mass);
}

void calc_engaged_clutch_speeds(struct vehicle_info *self)
{
    /* Calculate rotor energy */
    float moment_of_inertia = calc_moment_of_inertia(self->rotor_mass, self->rotor_radius);
    float engine_rads = rpm_to_rads(self->engine_rpm);
    float rotor_energy = calc_kinetic_energy(moment_of_inertia, engine_rads);
    printf("Rotor energy: %f\n", rotor_energy);

    float vehicle_energy = calc_kinetic_energy(self->body_mass, kmh_to_ms(self->vehicle_speed));
    printf("Vehicle energy: %f\n", vehicle_energy);

    float vehicle_motor_rpm = kmh_to_motor_side_rpm(self);
    printf("vehicle_motor_rpm: %f\n", vehicle_motor_rpm);
    float vehicle_weight_motor_side = calc_mass(vehicle_energy, rpm_to_rads(vehicle_motor_rpm));

    float combined_velocity = rads_to_rpm(calculate_velocity(vehicle_energy + rotor_energy, vehicle_weight_motor_side + moment_of_inertia));
    printf("combined_velocity: %f\n", combined_velocity);

    self->engine_rpm = combined_velocity;
    self->vehicle_speed = motor_side_rpm_to_kmh(self);
}

/**
 * Update motor and vehicle separately, since clutch is disengaged
 */
void update_speed_separately(struct vehicle_info *self, float deltatime)
{
    /* Update motor speed */
    float torque = torque_at_rpm(self, self->engine_rpm) * self->throttle;
    float mass = self->rotor_mass;
    self->engine_rpm += rads_to_rpm(calculate_acceleration(torque + self->rotor_drag_torque, mass, deltatime));
    if (self->engine_rpm < self->idle) {
        self->engine_rpm = self->idle;
    }

    /* Update vehicle speed */
    float force = self->wind_coefficient;
    mass = self->body_mass;
    self->vehicle_speed += (force / mass) * deltatime;
}

int main(void) 
{
    struct vehicle_info yamaha_fjr_1300;
    yamaha_fjr_1300.gears = sizeof(gear_ratio_yamaha_fjr_1300) / sizeof(float);
    yamaha_fjr_1300.gear_ratio = gear_ratio_yamaha_fjr_1300;

    yamaha_fjr_1300.speed_torque_values = sizeof(rpm_torque_yamaha_fjr_1300) / sizeof(rpm_torque_yamaha_fjr_1300[0]);
    yamaha_fjr_1300.torque_at_rpm = rpm_torque_yamaha_fjr_1300;

    yamaha_fjr_1300.rotor_mass = 1.0f;
    yamaha_fjr_1300.rotor_radius = 0.1f;
    yamaha_fjr_1300.rotor_drag_torque = -10.0f;
    yamaha_fjr_1300.idle = 1400.0f;

    yamaha_fjr_1300.primary_gear_reduction = 1.563f;
    yamaha_fjr_1300.final_gear_reduction = 2.773f;

    yamaha_fjr_1300.selected_gear = 1;

    /* Wheel size 180/55ZR17 */
    yamaha_fjr_1300.wheel_diameter = (17.0f * 25.4 + 55.0f) / 1000.0f;
    yamaha_fjr_1300.body_mass = 250.0f;
    yamaha_fjr_1300.wind_coefficient = -5.0f;


    printf("Gears: %d\n", yamaha_fjr_1300.gears);
    for (int i = 0; i < yamaha_fjr_1300.gears; i++) {
        yamaha_fjr_1300.selected_gear = i+1;
        printf("Ratio at gear %d: %f\n", yamaha_fjr_1300.selected_gear, yamaha_fjr_1300.gear_ratio[i]);
    }

    for (float rpm = 0.0f; rpm < 10000.0f; rpm += 100.0f) {
        float torque = torque_at_rpm(&yamaha_fjr_1300, rpm);
        printf("Engine RPM: %f rpm, torque: %f Nm\n", rpm, torque);
    }

    /* Check wheel torque at peak torque rpm */
    float rpm = 6500.0f;
    for (unsigned int gear = 0; gear <= yamaha_fjr_1300.gears; gear++) {
        float wheel_torque = get_wheel_torque(&yamaha_fjr_1300, rpm, gear);
        float vehicle_motor_force = wheel_torque / yamaha_fjr_1300.wheel_diameter / 2.0f;
        printf("Engine RPM: %f rpm, wheel torque: %f Nm, force: %fN\n", rpm, wheel_torque, vehicle_motor_force);
    }

    /* Check rpm and speed when disengaged clutch */
    yamaha_fjr_1300.vehicle_speed = 100.0f;
    yamaha_fjr_1300.engine_rpm = 2000.0f;
    yamaha_fjr_1300.throttle = 1.0f;
    for (int i = 0; i < 1000; i++) {
        printf("Speed %f, rpm %f\n", yamaha_fjr_1300.vehicle_speed, yamaha_fjr_1300.engine_rpm);
        update_speed_separately(&yamaha_fjr_1300, 0.1f);
    }

    yamaha_fjr_1300.throttle = 0.0f;
    for (int i = 0; i < 1000; i++) {
        printf("Speed %f, rpm %f\n", yamaha_fjr_1300.vehicle_speed, yamaha_fjr_1300.engine_rpm);
        update_speed_separately(&yamaha_fjr_1300, 0.1f);
    }

    yamaha_fjr_1300.selected_gear = 1;
    calc_engaged_clutch_speeds(&yamaha_fjr_1300);

	return 0;
}
