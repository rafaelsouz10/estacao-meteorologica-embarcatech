#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#include "lib/config_btn.h" 
#include "lib/task_sensores.h"
#include "lib/task_alerta.h"
#include "lib/task_display.h"
#include "lib/task_webserver.h"
#include "lib/task_matriz.h"

int main() {
    stdio_init_all();
    init_btn_callback();

    // Cria a tasks
    xTaskCreate(vTaskSensores, "Sensores", 1024, NULL, 2, NULL);
    xTaskCreate(vTaskAlerta, "Alerta", 512, NULL, 1, NULL);
    xTaskCreate(vTaskDisplay, "Display", 1024, NULL, 1, NULL);
    xTaskCreate(vTaskWebServer, "WebServer", 1024, NULL, 1, NULL);
    xTaskCreate(vTaskMatriz, "Matriz", 1024, NULL, 1, NULL);

    // Inicia o agendador do FreeRTOS
    vTaskStartScheduler();

    // return 0;
}
