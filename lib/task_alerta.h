#ifndef TASK_ALERTA_H
#define TASK_ALERTA_H

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#include "hardware/structs/timer.h" // Para usar alarmes via add_alarm_in_us

// GPIOs da BitDogLab
#define LED_R 13
#define LED_G 12
#define LED_B 11
#define BUZZER 21

// Limites de alerta
volatile float limite_min_temp = 10.0; // Alerta se temp < 10 °C
volatile float limite_max_temp = 35.0; // Ex: alerta se temp > 30 °C
volatile float limite_min_umi = 20.0; // Alerta se umidade < 20%

// Variáveis globais dos sensores
extern volatile float temp_aht;
extern volatile float umid_aht;
extern volatile float temp_bmp;

// Variáveis de controle
volatile bool buzzer_estado = false; // Controla do estado atual do buzzer (ligado ou desligado)
alarm_id_t buzzer_alarm_id = -1;    // Armazena o ID do alarm ativo (usado para cancelar depois)

// Callback do alarme: alterna o buzzer a cada 2000us (simula ~500Hz)
int64_t buzzer_alarm_callback(alarm_id_t id, void *user_data) {
    buzzer_estado = !buzzer_estado;
    gpio_put(BUZZER, buzzer_estado);
    return 2000; // Reexecuta o callback a cada 2ms (500Hz)
}

// Inicia o efeito sonoro do buzzer (se não estiver tocando).
void buzzer_start_alarm() {
    if (buzzer_alarm_id < 0) {  // Só agenda se não tiver um alarm ativo
        buzzer_alarm_id = add_alarm_in_us(2000, buzzer_alarm_callback, NULL, true);
    }
}

// Para o efeito sonoro do buzzer (se estiver tocando).
void buzzer_stop_alarm() {
    if (buzzer_alarm_id >= 0) {         // Só cancela se um alarm estiver ativo
        cancel_alarm(buzzer_alarm_id); // Cancela o alarme
        buzzer_alarm_id = -1;         // Reseta o ID
        gpio_put(BUZZER, 0);         // Desliga fisicamente o buzzer
        buzzer_estado = false;      // Reseta o estado de controle
    }
}

void vTaskAlerta(void *pvParameters) {
    // Inicializa pinos do LED RGB e buzzer
    gpio_init(LED_R);
    gpio_init(LED_G);
    gpio_init(LED_B);

    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_set_dir(LED_B, GPIO_OUT);

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);
    buzzer_stop_alarm();

    while (1) {
        bool alerta_temp = (temp_aht > limite_max_temp || temp_aht < limite_min_temp);
        bool alerta_tempbpm = (temp_bmp > limite_max_temp || temp_bmp < limite_min_temp);
        bool alerta_umi  = (umid_aht < limite_min_umi);

        if (alerta_tempbpm || alerta_temp || alerta_umi) {
            // Estado de alerta → LED vermelho + buzzer
            gpio_put(LED_R, 1);
            gpio_put(LED_G, 0);
            gpio_put(LED_B, 0);

            // Toque rápido do buzzer
            if (!buzzer_silenciado) {
                buzzer_start_alarm();
                vTaskDelay(pdMS_TO_TICKS(100));
                buzzer_stop_alarm();
            }
        } else {
            // Estado normal → LED verde
            gpio_put(LED_R, 0);
            gpio_put(LED_G, 1);
            gpio_put(LED_B, 0);

            buzzer_stop_alarm();
            buzzer_silenciado = false;  // Reativa buzzer após sair do alerta
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Verifica a cada 1s
    }
}

#endif
