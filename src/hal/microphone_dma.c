// FROM https://github.com/BitDogLab/BitDogLab-C/blob/main/microphone_dma
// EDITADO PARA USO COMO BIBLIOTECA E ADIÇÃO DE FUNCIONALIDADES

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "include/microphone_dma.h"

// Canal e configurações do DMA
uint dma_channel;
dma_channel_config dma_cfg;

// Buffer de amostras do ADC.
uint16_t adc_buffer[SAMPLES];

void sample_mic_print() {
  adc_fifo_drain(); // Limpa o FIFO do ADC.
  adc_run(false); // Desliga o ADC (se estiver ligado) para configurar o DMA.

  dma_channel_configure(dma_channel, &dma_cfg,
    adc_buffer, // Escreve no buffer.
    &(adc_hw->fifo), // Lê do ADC.
    SAMPLES, // Faz SAMPLES amostras.
    true // Liga o DMA.
  );

  // Liga o ADC e espera acabar a leitura.
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_channel);
  
  // Acabou a leitura, desliga o ADC de novo.
  adc_run(false);

  // for(int i = 0; i < SAMPLES; i++)
  // printf("%d ", adc_buffer[i]);
}

float mic_power() {
  float avg = 0.f;

  for (uint i = 0; i < SAMPLES; ++i)
    avg += adc_buffer[i] * adc_buffer[i];
  
  avg /= SAMPLES;
  return sqrt(avg);
}

uint8_t get_intensity(float v) {
  uint count = 0;

  while ((v -= ADC_STEP/20) > 0.f)
    ++count;
  
  return count;
}

// int main() {
  
//   printf("\n----\nIniciando loop...\n----\n");
//   while (true) {

//     // Realiza uma amostragem do microfone.
//     sample_mic();

//     // Pega a potência média da amostragem do microfone.
//     float avg = mic_power();
//     avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)

//     uint intensity = get_intensity(avg); // Calcula intensidade a ser mostrada na matriz de LEDs.

//     // Envia a intensidade e a média das leituras do ADC por serial.
//     printf("%2d %8.4f\r", intensity, avg);
//   }
// }


