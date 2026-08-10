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

    unsigned int clutch_engaged; /* Is the clutch engaged, 0 for disengaged and 1 for engaged */
};
