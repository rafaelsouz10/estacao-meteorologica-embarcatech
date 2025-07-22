#ifndef TASK_WEBSERVER_H
#define TASK_WEBSERVER_H

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

// Variáveis globais dos sensores (externas)
extern volatile float temp_aht;
extern volatile float umid_aht;
extern volatile float temp_bmp;
extern volatile float pressao_bmp;
extern volatile bool buzzer_silenciado;

extern volatile float offset_temp_aht;
extern volatile float offset_umi_aht;
extern volatile float offset_temp_bmp ;
extern volatile float offset_pressao;

extern volatile float limite_min_temp;
extern volatile float limite_max_temp;
extern volatile float limite_min_umi;

// Wi-Fi
#define WIFI_SSID "AP_LAB_MOVEL"
#define WIFI_PASS "congresso@2023"

char ip_display[24] = "Conectando...";

#define HIST_SIZE 20
float hist_temp[HIST_SIZE];
float hist_umi[HIST_SIZE];
float hist_tempbmp[HIST_SIZE];
float hist_pressao[HIST_SIZE];
int hist_index = 0;

#define RESPONSE_MAX 8096

// --- HTML ---
static const char HTML_BODY[] =
"<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Estacao</title>"
"<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
"<style>"
"body{font-family:sans-serif;text-align:center;padding:20px;background:#f5f5f5;}"
".card{border:1px solid #ccc;border-radius:10px;padding:10px;margin:10px;display:inline-block;width:200px;background:white;box-shadow:0 2px 5px rgba(0,0,0,0.1);}"
".title{font-weight:bold;margin-bottom:5px;}"
".graficos{display:flex;flex-wrap:wrap;justify-content:center;gap:20px;margin-top:40px;}"
".grafico-container{flex:1 1 45%;max-width:45%;min-width:300px;}"
"canvas{width:100% !important;height:auto !important;}"
"@media screen and (max-width:600px){.grafico-container{flex:1 1 100% !important;max-width:100% !important;min-width:0 !important;padding:0 10px;}}"
"</style>"
"<script>"
"let chartTemp, chartUmi, chartTempBMP, chartPressao;"
"function atualizar() {"
"fetch('/estado').then(r => r.json()).then(data => {"
"document.getElementById('tempAHT').innerText = data.temp_aht + ' °C';"
"document.getElementById('umidAHT').innerText = data.umid_aht + ' %';"
"document.getElementById('tempBMP').innerText = data.temp_bmp + ' °C';"
"document.getElementById('pressao').innerText = data.pressao + ' kPa';"
"document.getElementById('buzzer').innerText = data.buzzer ? 'Silenciado' : 'Ativo';"

"chartTemp.data.datasets[0].data = data.hist_temp; chartTemp.update();"
"chartUmi.data.datasets[0].data = data.hist_umi; chartUmi.update();"
"chartTempBMP.data.datasets[0].data = data.hist_tempbmp; chartTempBMP.update();"
"chartPressao.data.datasets[0].data = data.hist_pressao; chartPressao.update();"

"document.getElementById('labelOffsetTemp').innerText = 'Offset Temp AHT: ' + data.offset_temp_aht.toFixed(1) + ' °C';"
"document.getElementById('labelOffsetUmi').innerText = 'Offset Umidade AHT: ' + data.offset_umi_aht.toFixed(1) + ' %';"
"document.getElementById('labelOffsetTempBMP').innerText = 'Offset Temp BMP: ' + data.offset_temp_bmp.toFixed(1) + ' °C';"
"document.getElementById('labelOffsetPressao').innerText = 'Offset Pressão: ' + data.offset_pressao.toFixed(0) + ' Pa';"
"document.getElementById('labelLimMinTemp').innerText = 'Limite Mínimo Temp: ' + data.limite_min_temp.toFixed(1) + ' °C';"
"document.getElementById('labelLimMaxTemp').innerText = 'Limite Máximo Temp: ' + data.limite_max_temp.toFixed(1) + ' °C';"
"document.getElementById('labelLimMinUmi').innerText = 'Limite Mínimo Umidade: ' + data.limite_min_umi.toFixed(1) + ' %';"
"});"
"}"
"window.onload = () => {"
"const ctxTemp = document.getElementById('graficoTemp').getContext('2d');"
"const ctxUmi = document.getElementById('graficoUmi').getContext('2d');"
"const ctxTempBMP = document.getElementById('graficoTempBMP').getContext('2d');"
"const ctxPressao = document.getElementById('graficoPressao').getContext('2d');"
"chartTemp = new Chart(ctxTemp, {type:'line', data:{labels:Array.from({length:20},(_,i)=>i), datasets:[{label:'Temp AHT20', data:[], borderColor:'red', fill:false}]}});"
"chartUmi = new Chart(ctxUmi, {type:'line', data:{labels:Array.from({length:20},(_,i)=>i), datasets:[{label:'Umidade', data:[], borderColor:'blue', fill:false}]}});"
"chartTempBMP = new Chart(ctxTempBMP, {type:'line', data:{labels:Array.from({length:20},(_,i)=>i), datasets:[{label:'Temp BMP280', data:[], borderColor:'green', fill:false}]}});"
"chartPressao = new Chart(ctxPressao, {type:'line', data:{labels:Array.from({length:20},(_,i)=>i), datasets:[{label:'Pressao', data:[], borderColor:'orange', fill:false}]}});"
"atualizar(); setInterval(atualizar, 1000);"
"};"
"function enviarForm() {"
"const dados = {"
"offset_temp: parseFloat(document.getElementById('offsetTemp').value),"
"offset_umi: parseFloat(document.getElementById('offsetUmi').value),"
"offset_tempbmp: parseFloat(document.getElementById('offsetTempBMP').value),"
"offset_pressao: parseFloat(document.getElementById('offsetPressao').value),"
"lim_min_temp: parseFloat(document.getElementById('limMinTemp').value),"
"lim_max_temp: parseFloat(document.getElementById('limMaxTemp').value),"
"lim_min_umi: parseFloat(document.getElementById('limMinUmi').value),"
"};"
"fetch('/ajuste', {"
"method: 'POST',"
"headers: {'Content-Type': 'application/json'},"
"body: JSON.stringify(dados)"
"}).then(res => {"
"if (res.ok) alert('Ajustes aplicados com sucesso!');"
"else alert('Erro ao aplicar ajustes.');"
"});"
"}"
"function resetarAjustes() {"
"fetch('/reset_ajuste', { method: 'POST' })"
".then(res => {"
"if (res.ok) alert('Ajustes resetados com sucesso!');"
"else alert('Erro ao resetar ajustes.');"
"});"
"}"
"</script></head><body>"

"<h2>Estação EmbarcaTech</h2>"

"<div>"
"<div class='card'><div class='title'>Temp (AHT20)</div><div id='tempAHT'>--</div></div>"
"<div class='card'><div class='title'>Umidade</div><div id='umidAHT'>--</div></div>"
"<div class='card'><div class='title'>Temp (BMP280)</div><div id='tempBMP'>--</div></div>"
"<div class='card'><div class='title'>Pressao</div><div id='pressao'>--</div></div>"
"<div class='card'><div class='title'>Buzzer</div><div id='buzzer'>--</div></div>"
"</div>"

"<h3>Calibração e Limites</h3>"
"<form id='formCalibracao' onsubmit='enviarForm(); return false;' style='display:flex;flex-wrap:wrap;justify-content:center;gap:15px;max-width:600px;margin:auto;'>"
"<div style='flex:1 1 45%;text-align:left;'>"
"<label id='labelOffsetTemp' for='offsetTemp'>Offset Temp AHT:</label><br>"
"<input type='number' step='0.1' id='offsetTemp' style='width:100%;'>"
"</div>"
"<div style='flex:1 1 45%;text-align:left;'>"
"<label id='labelOffsetUmi' for='offsetUmi'>Offset Umidade AHT:</label><br>"
"<input type='number' step='0.1' id='offsetUmi' style='width:100%;'>"
"</div>"
"<div style='flex:1 1 45%;text-align:left;'>"
"<label id='labelOffsetTempBMP' for='offsetTempBMP'>Offset Temp BMP:</label><br>"
"<input type='number' step='0.1' id='offsetTempBMP' style='width:100%;'>"
"</div>"
"<div style='flex:1 1 45%;text-align:left;'>"
"<label id='labelOffsetPressao' for='offsetPressao'>Offset Pressão:</label><br>"
"<input type='number' step='1' id='offsetPressao' style='width:100%;'>"
"</div>"
"<div style='flex:1 1 45%;text-align:left;'>"
"<label id='labelLimMinTemp' for='limMinTemp'>Limite Mínimo Temp:</label><br>"
"<input type='number' step='0.1' id='limMinTemp' style='width:100%;'>"
"</div>"
"<div style='flex:1 1 45%;text-align:left;'>"
"<label id='labelLimMaxTemp' for='limMaxTemp'>Limite Máximo Temp:</label><br>"
"<input type='number' step='0.1' id='limMaxTemp' style='width:100%;'>"
"</div>"
"<div style='flex:1 1 45%;text-align:left;'>"
"<label id='labelLimMinUmi' for='limMinUmi'>Limite Mínimo Umidade:</label><br>"
"<input type='number' step='0.1' id='limMinUmi' style='width:100%;'>"
"</div>"
"<div style='width:100%;text-align:center;margin-top:10px;'>"
"<button type='submit'>Salvar</button>&nbsp;"
"<button type='button' onclick='resetarAjustes()'>Resetar</button>"
"</div></form>"


"<div class='graficos'>"
"<div class='grafico-container'><canvas id='graficoTemp'></canvas></div>"
"<div class='grafico-container'><canvas id='graficoUmi'></canvas></div>"
"<div class='grafico-container'><canvas id='graficoTempBMP'></canvas></div>"
"<div class='grafico-container'><canvas id='graficoPressao'></canvas></div>"
"</div>"
"</body></html>";

struct http_state {
    char response[RESPONSE_MAX];
    size_t len;
    size_t sent;
};

static err_t http_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    struct http_state *hs = (struct http_state *)arg;
    hs->sent += len;
    if (hs->sent >= hs->len) {
        tcp_close(tpcb);
        free(hs);
    }
    return ERR_OK;
}

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    // 1. Se não tiver payload, fecha
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *req = (char *)p->payload;
    struct http_state *hs = malloc(sizeof(struct http_state));
    if (!hs) {
        pbuf_free(p);
        tcp_close(tpcb);
        return ERR_MEM;
    }
    hs->sent = 0;

    // ------------------- VERIFICAÇÕES DAS ROTAS --------------------

    if (strncmp(req, "GET / ", 6) == 0 || strstr(req, "GET /index.html")) {
        int content_length = strlen(HTML_BODY);
        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n%s",
            content_length, HTML_BODY);

    } else if (strstr(req, "GET /estado")) {
         // Aplica os offsets de calibração
        float temp_calibrada = temp_aht;       // já calibrado na task_sensores
        float umi_calibrada  = umid_aht;
        float temp_bmp_calibrada = temp_bmp;
        float pressao_calibrada  = pressao_bmp;

        // Históricos
        char hist_temp_str[512] = "", hist_umi_str[512] = "", hist_tempbmp_str[512] = "", hist_pressao_str[512] = "";
        for (int i = 0; i < HIST_SIZE; i++) {
            int idx = (hist_index + i) % HIST_SIZE;
            char buf[32];
            snprintf(buf, sizeof(buf), "%.1f,", hist_temp[idx]); strcat(hist_temp_str, buf);
            snprintf(buf, sizeof(buf), "%.1f,", hist_umi[idx]); strcat(hist_umi_str, buf);
            snprintf(buf, sizeof(buf), "%.1f,", hist_tempbmp[idx]); strcat(hist_tempbmp_str, buf);
            snprintf(buf, sizeof(buf), "%.0f,", hist_pressao[idx]); strcat(hist_pressao_str, buf);
        }
        if (strlen(hist_temp_str)) hist_temp_str[strlen(hist_temp_str)-1] = '\0';
        if (strlen(hist_umi_str)) hist_umi_str[strlen(hist_umi_str)-1] = '\0';
        if (strlen(hist_tempbmp_str)) hist_tempbmp_str[strlen(hist_tempbmp_str)-1] = '\0';
        if (strlen(hist_pressao_str)) hist_pressao_str[strlen(hist_pressao_str)-1] = '\0';

        char json[2048];
        int json_len = snprintf(json, sizeof(json),
            "{"
            "\"temp_aht\":%.1f,"
            "\"umid_aht\":%.1f,"
            "\"temp_bmp\":%.1f,"
            "\"pressao\":%.0f,"
            "\"buzzer\":%d,"
            "\"offset_temp_aht\":%.2f,"
            "\"offset_umi_aht\":%.2f,"
            "\"offset_temp_bmp\":%.2f,"
            "\"offset_pressao\":%.0f,"
            "\"limite_min_temp\":%.1f,"
            "\"limite_max_temp\":%.1f,"
            "\"limite_min_umi\":%.1f,"
            "\"hist_temp\":[%s],"
            "\"hist_umi\":[%s],"
            "\"hist_tempbmp\":[%s],"
            "\"hist_pressao\":[%s]"
            "}",
            temp_calibrada, umi_calibrada, temp_bmp_calibrada, pressao_calibrada,
            buzzer_silenciado ? 1 : 0,
            offset_temp_aht, offset_umi_aht, offset_temp_bmp, offset_pressao,
            limite_min_temp, limite_max_temp, limite_min_umi,
            hist_temp_str, hist_umi_str, hist_tempbmp_str, hist_pressao_str);

        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n%s",
            json_len, json);

    } else if (strstr(req, "POST /ajuste")) {
        // Lê o corpo JSON da requisição
        const char* json_start = strstr(req, "\r\n\r\n");
        if (json_start) {
            json_start += 4; // pula os \r\n\r\n
            float temp_offset = 0, umi_offset = 0, tempbmp_offset = 0, pressao_offset = 0;
            float lim_min_temp = 0, lim_max_temp = 0, lim_min_umi = 0;

            sscanf(json_start,
                "{\"offset_temp\":%f,\"offset_umi\":%f,\"offset_tempbmp\":%f,"
                "\"offset_pressao\":%f,\"lim_min_temp\":%f,\"lim_max_temp\":%f,"
                "\"lim_min_umi\":%f}",
                &temp_offset, &umi_offset, &tempbmp_offset, &pressao_offset,
                &lim_min_temp, &lim_max_temp, &lim_min_umi);

            offset_temp_aht = temp_offset;
            offset_umi_aht = umi_offset;
            offset_temp_bmp = tempbmp_offset;
            offset_pressao = pressao_offset;
            limite_min_temp = lim_min_temp;
            limite_max_temp = lim_max_temp;
            limite_min_umi = lim_min_umi;

            hs->len = snprintf(hs->response, sizeof(hs->response),
                            "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
        } else {
            hs->len = snprintf(hs->response, sizeof(hs->response),
                            "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
        }
    } else if (strstr(req, "POST /reset_ajuste")) {
        offset_temp_aht = 0.0;
        offset_umi_aht = 0.0;
        offset_temp_bmp = 0.0;
        offset_pressao = 0.0;

        limite_min_temp = 10.0;
        limite_max_temp = 35.0;
        limite_min_umi = 30.0;

        hs->len = snprintf(hs->response, sizeof(hs->response),
        "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
    } else {
        const char *resp = "404 Not Found";
        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n%s",
            (int)strlen(resp), resp);
    }

    tcp_arg(tpcb, hs);
    tcp_sent(tpcb, http_sent);
    tcp_write(tpcb, hs->response, hs->len, TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    pbuf_free(p);
    return ERR_OK;
}

static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

// Estrutura da task vTaskWebServer
void vTaskWebServer(void *pvParameters) {
    if (cyw43_arch_init()) {
        printf("Erro ao iniciar Wi-Fi\n");
        vTaskDelete(NULL);
        return;
    }

    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 5000)) {
        printf("Falha na conexão Wi-Fi\n");
        vTaskDelete(NULL);
        return;
    }

    uint32_t addr = cyw43_state.netif[0].ip_addr.addr;
    uint8_t *ip = (uint8_t *)&addr;
    snprintf(ip_display, sizeof(ip_display), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    struct tcp_pcb *pcb = tcp_new();
    if (!pcb || tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Falha ao configurar servidor\n");
        vTaskDelete(NULL);
        return;
    }
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);

    while (1) {
        cyw43_arch_poll();
        vTaskDelay(pdMS_TO_TICKS(100));

        hist_temp[hist_index]     = temp_aht;
        hist_umi[hist_index]      = umid_aht;
        hist_tempbmp[hist_index]  = temp_bmp;
        hist_pressao[hist_index]  = pressao_bmp;
        hist_index = (hist_index + 1) % HIST_SIZE;
    }
}

#endif
