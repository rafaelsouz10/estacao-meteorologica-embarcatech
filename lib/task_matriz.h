#ifndef TASK_MATRIZ_H
#define TASK_MATRIZ_H

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ws2818b.pio.h"

// Sensores e limites
extern volatile float temp_aht;
extern volatile float temp_bmp;
extern volatile float umid_aht;
extern volatile float limite_max_temp;
extern volatile float limite_min_umi;

// Configuração da matriz
#define LED_COUNT 25
#define LED_PIN 7
#define BRILHO 0.05

struct pixel_t {
  uint8_t G, R, B;
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t;

npLED_t leds[LED_COUNT];
PIO np_pio;
uint sm;

void npInit(uint pin) {
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true);
  }
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0; leds[i].G = 0; leds[i].B = 0;
  }
}

void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = (uint8_t)(r * BRILHO);
  leds[index].G = (uint8_t)(g * BRILHO);
  leds[index].B = (uint8_t)(b * BRILHO);
}

void npClear() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

void npWrite() {
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100);
}

// Task da matriz RGB – alerta simples em vermelho
void vTaskMatriz(void *params) {
  npInit(LED_PIN);
  bool estado = true;

  while (true) {
    bool alerta_temp = (temp_aht > limite_max_temp || temp_bmp > limite_max_temp);
    bool alerta_umi = (umid_aht < limite_min_umi);

    if (alerta_temp || alerta_umi) {
      npClear();
      if (estado) {
        for (int i = 0; i < LED_COUNT; i++) {
          npSetLED(i, 255, 0, 0); // Vermelho
        }
      }
      npWrite();
      estado = !estado;
    } else {
      npClear();
      npWrite();
    }

    vTaskDelay(pdMS_TO_TICKS(500)); // Pisca a cada 500ms
  }
}

#endif
