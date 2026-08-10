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

    /* Dynamic state */
    unsigned int selected_gear;

    float engine_rpm;
    float vehicle_speed;

    float throttle; /* 0 -> 1 for 0% -> 100% of available torque */
    float brake;
    float present_motor_torque;

    unsigned int clutch_engaged; /* Is the clutch engaged, 0 for disengaged and 1 for engaged */
};

struct vehicle_info *create_vehicle(void);
unsigned int set_gear(struct vehicle_info *self, unsigned int gear);
float get_total_gear_ratio(struct vehicle_info *self);
float torque_at_rpm(struct vehicle_info *self, float rpm);
float get_wheel_torque(struct vehicle_info *self, float rpm, unsigned int gear);
void update_speed(struct vehicle_info *self, float deltatime);
void engage_clutch(struct vehicle_info *self);