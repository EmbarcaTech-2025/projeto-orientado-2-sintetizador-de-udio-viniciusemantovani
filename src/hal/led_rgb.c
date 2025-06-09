#include <stdio.h> // Biblioteca padrão
#include <math.h> // Biblioteca de matemática (função "round" foi utilizada)

#include "pico/stdlib.h" // Biblioteca padrão pico
#include "hardware/gpio.h" // Biblioteca de GPIOs

#include "include/led_rgb.h" // Biblioteca led

// Função para atualizar os estados dos LEDs
void set_led_color(uint8_t R_G) {
    if(R_G == 1 || R_G == 0){
        gpio_put(RED_PIN, R_G);   // Configura o estado do LED vermelho
        gpio_put(GREEN_PIN, !R_G); // Configura o estado do LED verde
    } else{
        gpio_put(RED_PIN, 0);   // Configura o estado do LED vermelho
        gpio_put(GREEN_PIN, 0); // Configura o estado do LED verde
    }
}