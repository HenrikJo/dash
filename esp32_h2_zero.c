#include <stdio.h>
#include "model.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"

#define SERVO1_GPIO 4
#define SERVO2_GPIO 5

#define BTN_UP_GPIO       8
#define BTN_DOWN_GPIO     9
#define BTN_INC_GPIO      10

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_SERVO1     LEDC_CHANNEL_0
#define LEDC_CHANNEL_SERVO2     LEDC_CHANNEL_1
#define LEDC_DUTY_RES           LEDC_TIMER_14_BIT
#define LEDC_FREQUENCY          50  // 50Hz

static int servo_angle = 90;
static int step_size = 5;

static uint32_t servo_angle_to_duty(int angle)
{
    uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1;

    float min_us = 500.0;
    float max_us = 2500.0;
    float pulse_us = min_us + (angle / 180.0) * (max_us - min_us);

    return (uint32_t)((pulse_us / 20000.0) * max_duty);
}

static void servo_set_angle(ledc_channel_t channel, int angle)
{
    uint32_t duty = servo_angle_to_duty(angle);
    ledc_set_duty(LEDC_MODE, channel, duty);
    ledc_update_duty(LEDC_MODE, channel);
}

static void servo_init(void)
{
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t servo1 = {
        .gpio_num = SERVO1_GPIO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_SERVO1,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };

    ledc_channel_config_t servo2 = servo1;
    servo2.gpio_num = SERVO2_GPIO;
    servo2.channel = LEDC_CHANNEL_SERVO2;

    ESP_ERROR_CHECK(ledc_channel_config(&servo1));
    ESP_ERROR_CHECK(ledc_channel_config(&servo2));
}

static void buttons_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN_UP_GPIO) |
                        (1ULL << BTN_DOWN_GPIO) |
                        (1ULL << BTN_INC_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}

static void clamp_angle(void)
{
    if (servo_angle < 0) servo_angle = 0;
    if (servo_angle > 180) servo_angle = 180;
}

static void update_view(struct vehicle_info *self)
{
    float rpm = self->engine_rpm / max_rpm;
    rpm = rpm > 180 ? 180 : rpm;
    servo_set_angle(LEDC_CHANNEL_SERVO1, rpm);

    float speed = self->vehicle_speed / max_speed;
    speed = speed > 180 ? 180 : speed;
    servo_set_angle(LEDC_CHANNEL_SERVO2, speed);
}

void app_main(void)
{
    buttons_init();
    servo_init();

    /* Create model */
    struct vehicle_info *yamaha_fjr_1300 = create_vehicle();
    engage_clutch(yamaha_fjr_1300);
    int gear = 0; // 0=N, 1..6


    while (1) {
        if (!gpio_get_level(BTN_UP_GPIO)) {
            gear = gear >) 6 ? 6 : gear + 1;
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (!gpio_get_level(BTN_DOWN_GPIO)) {
            gear = gear <= 0 ? 0 : gear - 1;
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (!gpio_get_level(BTN_INC_GPIO))  {
            throttle.value += 0.01f;
        } else {
            throttle.value -= 0.01f;
        }

        if (throttle.value > throttle.max) throttle.value = throttle.max;
        if (throttle.value < throttle.min) throttle.value = throttle.min;

        yamaha_fjr_1300->throttle = throttle.value;
        set_gear(yamaha_fjr_1300, gear);
        update_speed(yamaha_fjr_1300, 0.016f);
        update_view(outdir, yamaha_fjr_1300);
    }
}
