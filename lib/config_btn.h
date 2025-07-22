#ifndef CONFIG_BTN_H
#define CONFIG_BTN_H

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define BOTAO_A 5
#define botaoB 6

// --- Controle de estado e debounce ---
volatile bool buzzer_silenciado = true;
absolute_time_t ultimo_clique = 0;

// --- Handler único de interrupções dos botões ---
void gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BOTAO_A) {
        if (absolute_time_diff_us(ultimo_clique, get_absolute_time()) > 300000) {
            buzzer_silenciado = true;
            ultimo_clique = get_absolute_time();
            printf("Botão A pressionado: buzzer silenciado\n");
        }
    } else if (gpio == botaoB) {
        reset_usb_boot(0, 0);
    }
}

void init_btn_callback(){
    // Botão A
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Fim do trecho para modo BOOTSEL com botão B
}

#endif