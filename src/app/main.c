#include <stdio.h> // Biblioteca padrão
#include <math.h> // Biblioteca de matemática (função "round" foi utilizada)

#include "pico/stdlib.h" // Biblioteca padrão pico
#include "hardware/gpio.h" // Biblioteca de GPIOs
#include "hardware/adc.h" // Biblioteca do ADC
#include "hardware/pwm.h" // Biblioteca do PWM
#include <hardware/i2c.h>

#include "include/microphone_dma.h" // Biblioteca microfone
#include "include/buzzer_pwm1.h" // Biblioteca buzzer
#include "include/led_rgb.h" // Biblioteca led
#include "include/ssd1306.h" // Biblioteca led

// Definição dos pinos dos botões:
#define BUTTON_A 5
#define BUTTON_B 6

// Definição dos pinos para I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

// Definições de blocos de audio em uma gravação
#define BLOCK_SIZE 234
#define NUM_BLOCKS (SAMPLES / BLOCK_SIZE)

volatile bool rec = false; // Flag para determinar se deve gravar;
volatile bool play = false; // Flag para determinar se deve reproduzir;

ssd1306_t disp; // Instância do display.

/**
 * @brief Handler de interrupções para os botões.
 * @param gpio indica o número do gpio que gera a interrupção
 * @param event_mask indica o número referente ao evento que gera a interrupção
 */
void ButtonHandler(uint gpio, uint32_t event_mask){
  static absolute_time_t deb_time_B = 0; // Contador para debounce do botão B
  static absolute_time_t deb_time_A = 0; // Contador para debounce do botão A

  if(gpio == BUTTON_B){ // Para o botão B, soma um ao contador caso esteja contando e atualiza o display.

    if(event_mask == GPIO_IRQ_EDGE_FALL && absolute_time_diff_us(deb_time_B, get_absolute_time()) > 800){
      play = true;
    } else if(event_mask = GPIO_IRQ_EDGE_RISE && absolute_time_diff_us(deb_time_B, get_absolute_time()) > 800){
      deb_time_B = get_absolute_time();
    }

  }

  else if (gpio == BUTTON_A){ // Para o botão A, atualiza o contador de segundos, reiniciando a contagem e atualizando o display para o início.
    if(event_mask == GPIO_IRQ_EDGE_FALL && absolute_time_diff_us(deb_time_A, get_absolute_time()) > 800){
      rec = true;
    } else if(event_mask == GPIO_IRQ_EDGE_RISE && absolute_time_diff_us(deb_time_A, get_absolute_time()) > 800){
      deb_time_A = get_absolute_time();
    }
  }

}

void init_buttons(){

  // Inicializa botão A com pull_up:
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);

  // Inicializa botão B com pull_up:
  gpio_init(BUTTON_B);
  gpio_set_dir(BUTTON_B, GPIO_IN);
  gpio_pull_up(BUTTON_B);

  // Preparando interrupções para os botões:
  gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE, 1, &ButtonHandler);
  gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE, 1, &ButtonHandler);
  
}

void init_adc_dma(){

  adc_gpio_init(MIC_PIN);
  adc_init();
  adc_select_input(MIC_CHANNEL);

  adc_fifo_setup(
    true, // Habilitar FIFO
    true, // Habilitar request de dados do DMA
    1, // Threshold para ativar request DMA é 1 leitura do ADC
    false, // Não usar bit de erro
    false // Não fazer downscale das amostras para 8-bits, manter 12-bits.
  );

  adc_set_clkdiv(ADC_CLOCK_DIV);

   // Tomando posse de canal do DMA.
  dma_channel = dma_claim_unused_channel(true);

  // Configurações do DMA.
  dma_cfg = dma_channel_get_default_config(dma_channel);

  channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)
  channel_config_set_read_increment(&dma_cfg, false); // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)
  channel_config_set_write_increment(&dma_cfg, true); // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)
  
  channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usamos a requisição de dados do ADC

}

void init_led(){
    gpio_init(RED_PIN);
    gpio_init(GREEN_PIN);
    gpio_init(BLUE_PIN);

    gpio_set_dir(RED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);
}

void init_display(){
  // I2C Initialisation. Using it at 1Mhz.
  i2c_init(I2C_PORT, 1000*1000);
  
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  disp.external_vcc=false;
  ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
  ssd1306_clear(&disp);
  ssd1306_show(&disp);
}

void show_audio_display(ssd1306_t *p){
  static uint8_t audio_sample = 0; // Conta número de gravações.
  uint32_t avg_value = 0;

  if (audio_sample >= NUM_BLOCKS) return;

  for(int i = audio_sample*BLOCK_SIZE; i < BLOCK_SIZE*(1+audio_sample); i++){
    avg_value = avg_value+adc_buffer[i];
    printf("%d ", adc_buffer[i]);
  }
  avg_value = avg_value/BLOCK_SIZE;
  ssd1306_draw_square(p, audio_sample*3, 64-avg_value*64/4096, 3, avg_value*64/4096);
  audio_sample++;
}

int main() {

  stdio_init_all();
  
  init_adc_dma();
  init_buttons();
  init_led();
  init_display();
  pwm_init_audio(BUZZER_PIN);

  while(true){

    if(rec){
      set_led_color(1);
      sample_mic_print();
      rec = !rec;
      show_audio_display(&disp);
      ssd1306_show(&disp);
      set_led_color(2);
    } else if(play){
      set_led_color(0);
      play_audio_buffer(BUZZER_PIN, adc_buffer, SAMPLES); // Toca o áudio pelo PWM
      play = !play;
      set_led_color(2);
    }
  }

  return 0;
}