// .H PARA A BIBLIOTECA DE INTERFACE COM O MICROFONE

#ifndef MICROPHONE_DMA_H
#define MICROPHONE_DMA_H

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 4800.f
#define SAMPLES 30000 // Número de amostras que serão feitas do ADC. (3s para clock div 4800)
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajuste do valor do ADC para Volts.
#define ADC_STEP (3.3f/5.f) // Intervalos de volume do microfone.

#define abs(x) ((x < 0) ? (-x) : (x))

// Canal e configurações do DMA
extern uint dma_channel;
extern dma_channel_config dma_cfg;

// Buffer de amostras do ADC.
extern uint16_t adc_buffer[SAMPLES];

/**
 * @brief Realiza as leituras do ADC e armazena os valores no buffer.
 */
void sample_mic_print();    

/**
 * @brief Calcula a potência média das leituras do ADC. (Valor RMS)
 */
float mic_power();

/**
 * @brief Calcula a intensidade do volume registrado no microfone, de 0 a 4, usando a tensão.
 * @param v tensão.
 */
uint8_t get_intensity(float v);



#endif