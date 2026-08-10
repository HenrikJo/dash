struct rpm_torque {
    float rpm;
    float torque;
}

struct rpm_torque rpm_tq[] = {
    { .rpm = 0, .torque = 0 }
    { .rpm = 1600, .torque = 45 }
    { .rpm = 2000, .torque = 62 }
    { .rpm = 3000, .torque = 73 }
    { .rpm = 4000, .torque = 80 }
    { .rpm = 5000, .torque = 85 }
    { .rpm = 6000, .torque = 85 }
    { .rpm = 6500, .torque = 90 }
    { .rpm = 7000, .torque = 89 }
    { .rpm = 8000, .torque = 80 }
    { .rpm = 9000, .torque = 65 }
    { .rpm = 9500, .torque = 0 }
}
float gear_ratio[] = { 2.5, 1.722, 1.35, 1.111, 0.962, 0.846 };

float rpm_to_rev(float rpm, uint8_t gear)
{

}

int main(void) {
	
}
