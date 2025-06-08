// CRIADO PARA USAR https://github.com/BitDogLab/BitDogLab-C/blob/main/buzzer_pwm1/
// COMO BIBLIOTECA

#ifndef BUZZER_PWM1_H
#define BUZZER_PWM1_H

#include <math.h>

// Configuração do pino do buzzer
#define BUZZER_PIN 21
#define PWM_FREQ_HZ 20000  // 20 kHz para PWM (muito acima do áudio)
#define ADC_MAX 4095       // 12 bits do ADC
#define SAMPLE_DELAY_US 100 // tempo entre samples, 10 kHz (ajuste conforme seu ADC)

void pwm_init_audio(uint pin);
void play_audio_buffer(uint pin, uint16_t* buffer, size_t length);

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin);

// Toca uma nota com a frequência e duração especificadas
void play_tone(uint pin, uint frequency, uint duration_ms);



#endif