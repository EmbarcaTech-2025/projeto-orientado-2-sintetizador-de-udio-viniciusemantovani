/**
 * FROM https://github.com/BitDogLab/BitDogLab-C/blob/main/buzzer_pwm1/
 * EDITADA PARA FUNCIONAR COMO BIBLIOTECA PARA BUZZER PWM
 * Exemplo de acionamento do buzzer utilizando sinal PWM no GPIO 21 da Raspberry Pico / BitDogLab
 * 06/12/2024
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "include/buzzer_pwm1.h"

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Ajusta divisor de clock
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0); // Desliga o PWM inicialmente
}

// Toca uma nota com a frequência e duração especificadas
void play_tone(uint pin, uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle

    sleep_ms(duration_ms);

    pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
    sleep_ms(50); // Pausa entre notas
}

// Inicializa PWM para gerar PWM com frequência fixa e duty variável
void pwm_init_audio(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();

    // Ajusta o PWM para frequência fixa alta
    // clock = sys_clk / div; top = clock / freq - 1
    // Exemplo com clock 125 MHz
    pwm_config_set_clkdiv(&config, 1.0f); // sem divisor
    pwm_init(slice_num, &config, false);

    // Configura top para PWM_FREQ_HZ fixo
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / PWM_FREQ_HZ - 1;
    pwm_set_wrap(slice_num, top);

    pwm_set_gpio_level(pin, 0); // inicia desligado
    pwm_set_enabled(slice_num, true);
}

// Função que envia o buffer ADC para o PWM para tocar áudio
void play_audio_buffer(uint pin, uint16_t* buffer, size_t length) {
    uint slice_num = pwm_gpio_to_slice_num(pin);

    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / PWM_FREQ_HZ - 1;

    for (size_t i = 0; i < length; i++) {
        // Mapear sample ADC (0-4095) para nível PWM (0 - top)
        uint32_t level = ((uint32_t)buffer[i] * top) / ADC_MAX;

        pwm_set_gpio_level(pin, level);

        // Aguarda o tempo para próxima amostra
        sleep_us(SAMPLE_DELAY_US);
    }

    // Desliga após tocar
    pwm_set_gpio_level(pin, 0);
}