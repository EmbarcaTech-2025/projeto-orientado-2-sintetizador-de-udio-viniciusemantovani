#ifndef LED_RGB_H
#define LED_RGB_H

#include <math.h> // Biblioteca de matemática (função "round" foi utilizada)

// Configuração dos pinos GPIO
#define RED_PIN 13   // Pino para o LED vermelho
#define GREEN_PIN 11 // Pino para o LED verde
#define BLUE_PIN 12  // Pino para o LED azul

/**
 * @brief seta cor do led.
 * @param R_G define se deve ser vermelho (1) ou verde (0), apagado (2).
 */
void set_led_color(uint8_t R_G);

#endif