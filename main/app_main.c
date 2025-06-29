/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// TAG para os logs do nosso projeto
static const char *TAG = "MQTT_LED_CONTROL";

// Define o pino do LED. 
// O pino 2 é o mais comum, mas altere aqui se o seu for diferente.
#define LED_PIN GPIO_NUM_2

/**
 * @brief Configura o pino do LED como uma saída digital.
 */
static void configure_led(void)
{
    ESP_LOGI(TAG, "Configurando o pino do LED (GPIO %d) como saida", LED_PIN);
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

/**
 * @brief O handler de eventos para o cliente MQTT.
 * Esta função é chamada para cada evento do ciclo de vida do cliente.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        // Inscreve-se no tópico do LED assim que a conexão com o broker é estabelecida
        const char* led_topic = "/ifpe/ads/embarcados/esp32/led";
        msg_id = esp_mqtt_client_subscribe(client, led_topic, 1);
        ESP_LOGI(TAG, "Inscricao no topico \"%s\" enviada, msg_id=%d", led_topic, msg_id);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        ESP_LOGI(TAG, "Inscricao no topico do LED realizada com sucesso!");
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        // Imprime o tópico e o dado recebido para debug
        printf("TOPICO=%.*s\r\n", event->topic_len, event->topic);
        printf("DADO=%.*s\r\n", event->data_len, event->data);

        // LÓGICA DE CONTROLE DO LED
        // Verifica se a mensagem recebida é para o tópico do LED
        if (strncmp(event->topic, "/ifpe/ads/embarcados/esp32/led", event->topic_len) == 0) {
            ESP_LOGI(TAG, "Comando para o topico do LED recebido!");
            
            // Verifica se o conteúdo da mensagem (payload) é "1"
            if (strncmp(event->data, "1", event->data_len) == 0) {
                ESP_LOGI(TAG, "Comando: LIGAR LED");
                gpio_set_level(LED_PIN, 1); // Acende o LED
            
            // Verifica se o conteúdo da mensagem (payload) é "0"
            } else if (strncmp(event->data, "0", event->data_len) == 0) {
                ESP_LOGI(TAG, "Comando: DESLIGAR LED");
                gpio_set_level(LED_PIN, 0); // Apaga o LED
            
            } else {
                ESP_LOGW(TAG, "Comando desconhecido recebido: %.*s", event->data_len, event->data);
            }
        }
        break;
        
    // Outros eventos para debug
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

/**
 * @brief Inicia e configura o cliente MQTT.
 */
static void mqtt_app_start(void)
{
    // Configuração do cliente MQTT
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL, // Pega a URL do broker do menuconfig
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    
    // Registra o handler de eventos e inicia o cliente
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

/**
 * @brief Função principal da aplicação.
 */
void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Inicializando...");
    
    // Inicializações padrão do sistema e de rede
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Conecta ao Wi-Fi usando a função auxiliar dos exemplos
    ESP_ERROR_CHECK(example_connect());

    // Configura o pino do LED como saída
    configure_led();

    // Inicia o cliente MQTT
    mqtt_app_start();

    // Loop infinito para manter a tarefa principal "app_main" viva.
    // Sem isso, o programa poderia terminar e causar instabilidade.
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}