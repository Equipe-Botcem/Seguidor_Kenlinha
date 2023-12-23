#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_event.h"
#include "esp_adc/adc_continuous.h"
#include "esp_timer.h"
#include "esp_rom_uart.h"

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include <NimBLEDevice.h>
#include <ESP_PWM/ESP_PWM.h>
#include <seguidor.hpp>
#include <string.h>
#include <stdio.h>

#include "esp_mac.h"
#include "esp_system.h"
#include "esp_wifi.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


//WIFI
#define EXAMPLE_ESP_WIFI_SSID      "KenLinha"
#define EXAMPLE_ESP_WIFI_PASS      "jeielsantos"
#define EXAMPLE_ESP_WIFI_CHANNEL   11
#define EXAMPLE_MAX_STA_CONN       3

//UDP Server
#define PORT 8889
static const char *TAG = "example";
char rx_buffer[128];
char addr_str[128];
int addr_family = (int)AF_INET;
int ip_protocol = 0;
struct sockaddr_in6 dest_addr;
int sock = -1;

//UUIDs para configurar o BLE
#define SERVICE_UUID        "4cc35f22-1978-41da-b944-ac9fdc39b747"
#define CHARACTERISTIC_S_UUID "016210bb-89a6-41af-9db9-2cef7bdf36eb"
#define CHARACTERISTIC_E_UUID "92da7b08-bc25-4bfd-b5ac-0e1c722310b1"
#define DESCRITOR_UUID      "f7aeb4f3-fb2b-466a-b1a4-0ac53610902e"

string Robo = "Kenlinha";
int ESP_PWM::pinChannels[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
Seguidor_de_Linha seguidor = Seguidor_de_Linha();

//Comunicacao
BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pSaida;
BLECharacteristic *pEntrada;
BLEAdvertising *pAdvertising;

string buffer_saida = "";
string buffer_entrada = "";
string wbuffer_ent = "";
string wbuffer_sai = "";
string cmd = "";
unsigned long tmp_uart = 0;

string active_cmc = "";

SemaphoreHandle_t commMutex = NULL;

void BLE_INIT();
void WIFI_INIT();
void UDP_INIT();
void communication(void * args);
void readUartBLE();

//Leituras
void continuous_adc_INIT();
unsigned int tempo_passado = 0;
bool leitura_atualizada = false;
int cont_leituras = 0;
unsigned long cont_time = 0;

//funcoes de intr
void enc1(void*arg){
    seguidor.intr_enc_dir();
}
void enc2(void*arg){
    seguidor.intr_enc_esq();
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    //nvs_flash_init_partition("storage");

    //intr
    gpio_install_isr_service(0);
    gpio_isr_handler_add((gpio_num_t)37, enc1, (void*) 0);
    gpio_isr_handler_add((gpio_num_t)36, enc2, (void*) 0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    //ini
    commMutex = xSemaphoreCreateMutex();
    //BLE_INIT();
    WIFI_INIT();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    UDP_INIT();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    continuous_adc_INIT();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    cont_time = esp_timer_get_time();

    seguidor.init();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    seguidor.output(Robo + " pronto!");
    long tmp_comeco = 0;
    long tmp_acc = 0;
    long tmp_ini_stop = 0;
    bool TwoSec = false;
    xTaskCreatePinnedToCore(communication,"Comunicacao", 2048, NULL,4,NULL,1);
    //main loop
    while(1) {  
        
        if(esp_timer_get_time() - cont_time >= 250000){
            cont_time = esp_timer_get_time();
            cont_leituras = 0;
            //seguidor.printEncoders();
            if (seguidor.Estado_corrida == true){
                seguidor.printEncoders();
            }
        }
        if(tmp_ini_stop != 0 && (abs(seguidor.getVelDir()) < 0.10) && (abs(seguidor.getVelEsq()) < 0.10)){
            seguidor.output("Tempo:" + to_string(esp_timer_get_time() - tmp_ini_stop));
            tmp_ini_stop = 0;
            //seguidor.printEncoders();
            
        }
        if(tmp_acc > 0 && (abs(seguidor.getVelDir()) >= 1)){
            seguidor.output("Tempo:" + to_string(esp_timer_get_time() - tmp_acc));
            tmp_acc = 0;
            //seguidor.printEncoders();
            
        }

		if (seguidor.Estado_corrida == true){
            /*f(tmp_comeco == 0){
                seguidor.setVel(1,1);
                //seguidor.set_velocidade(2800,0);

                tmp_comeco = esp_timer_get_time();
                tmp_acc = tmp_comeco;
            }
            else if((esp_timer_get_time() - tmp_comeco >= 4000000) && !TwoSec){
                seguidor.printEncoders();
                TwoSec = true;

            }
            else if((esp_timer_get_time() - tmp_comeco >= 8000000)){
                if(seguidor.getDir() != 'P'){
                    seguidor.printEncoders();
                    tmp_ini_stop = esp_timer_get_time();
                    seguidor.set_direcao('P');
                    
                    //seguidor.output("Inicio Parada");
                    
                }
            }*/
            
            if(leitura_atualizada){
                seguidor.seguir_linha();
                cont_leituras++;
                leitura_atualizada = false;
            }
			
		}
		else if(seguidor.get_modo() != 'B' && (seguidor.get_modo() != 'J')){
			seguidor.stop("Loop");
		}
        //readUartBLE();
        if(xSemaphoreTake(commMutex, 0) == pdTRUE){
            if(wbuffer_ent != ""){
                seguidor.ControlCMD(wbuffer_ent);
                wbuffer_ent = "";
            }
            if(cmd != "" && (tempo_passado >= 10)){
                seguidor.ControlCMD(cmd);
                cmd = "";
            }
            xSemaphoreGive(commMutex);
        }
        seguidor.loop();
    }
}



#define Tamanho_Leitura                    4*10

static adc_channel_t channel[10] = {ADC_CHANNEL_2, ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_8, ADC_CHANNEL_9, ADC_CHANNEL_0, ADC_CHANNEL_1};
esp_err_t ret;
uint32_t ret_num = 0;
uint8_t result[Tamanho_Leitura] = {0};
static TaskHandle_t s_task_handle;


static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    //Notify that ADC continuous driver has done enough number of conversions
    //vTaskNotifyGiveFromISR(s_task_handle, &mustYield);
    ret = adc_continuous_read(handle, result, Tamanho_Leitura, &ret_num, 0);
    for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
        uint32_t chan_num = (p)->type2.channel;
        uint32_t data = (p)->type2.data;
        
        if(chan_num == 0) {
            seguidor.sensor_chegada.ler(data);
            
        }
        else if (chan_num == 1) seguidor.sensor_mapa.ler(data);
        else seguidor.sns_frontais.ler_sensor(chan_num, data, (!seguidor.troca_max_min));

        //if(chan_num > cont_leituras) cont_leituras = (int)chan_num;
    }
    leitura_atualizada = true;
    return (mustYield == pdTRUE);
}

static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
{
    adc_continuous_handle_t handle = NULL;

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = Tamanho_Leitura,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX];
    for (int i = 0; i < channel_num; i++) {
        adc_pattern[i].atten = ADC_ATTEN_DB_11;
        adc_pattern[i].channel = channel[i];
        adc_pattern[i].unit = ADC_UNIT_1;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }
    adc_continuous_config_t dig_cfg = {
        .pattern_num = 10,
        .adc_pattern = adc_pattern,
        .sample_freq_hz = 80 * 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
    };
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    *out_handle = handle;
}

void readUartBLE(){
    uint8_t letra;
    while(!esp_rom_uart_rx_one_char(&letra)){
        cmd += (char)letra;
        tmp_uart = esp_timer_get_time();
    }
    unsigned int tempo_passado = (esp_timer_get_time() - tmp_uart)/1000;
    if(strcmp(pEntrada->getValue().c_str(),"")){
        cmd = pEntrada->getValue().c_str();
        pEntrada->setValue("");
        tempo_passado = 11;
    }

    if(cmd != "" && (tempo_passado >= 10)){
        seguidor.ControlCMD(cmd);
        cmd = "";
    }
}

void BLE_INIT(){
    //BLE
    BLEDevice::init(Robo);
    BLEDevice::setMTU(512);
    
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);
    pSaida = pService->createCharacteristic(
                CHARACTERISTIC_S_UUID,
                NIMBLE_PROPERTY::READ |
                NIMBLE_PROPERTY::WRITE |
                NIMBLE_PROPERTY::NOTIFY |
                NIMBLE_PROPERTY::INDICATE
            ,4096);

    pSaida->setValue("");

    pEntrada = pService->createCharacteristic(
                    CHARACTERISTIC_E_UUID,
                    NIMBLE_PROPERTY::READ |
                    NIMBLE_PROPERTY::WRITE |
                    NIMBLE_PROPERTY::NOTIFY |
                    NIMBLE_PROPERTY::INDICATE
                ,4096);

    pEntrada->setValue("");

    pService->start();
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    
    BLEDevice::startAdvertising();
    seguidor.set_BLE_CHAR(pSaida);
}

void continuous_adc_INIT(){
    memset(result, 0xcc, Tamanho_Leitura);
    s_task_handle = xTaskGetCurrentTaskHandle();
    adc_continuous_handle_t handle = NULL;
    continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &handle);
    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
        .on_pool_ovf = NULL
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    ESP_ERROR_CHECK(adc_continuous_start(handle));
}
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            

#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}
void WIFI_INIT(){
    seguidor.set_WIFIBuffer(&wbuffer_sai);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
}


void UDP_INIT(){
    
    

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(PORT);
    ip_protocol = IPPROTO_IP;

    sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock >= 0){
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;
        setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
        bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        
        if(active_cmc == "B" || active_cmc == "") active_cmc += "W";
    }
}

void communication(void *args){
    while(1){
        if(xSemaphoreTake(commMutex, 10 / portTICK_PERIOD_MS) == pdTRUE){
            uint8_t letra = '0';
            while(!esp_rom_uart_rx_one_char(&letra)){
                cmd += (char)letra;
                tmp_uart = esp_timer_get_time();
            }
            tempo_passado = (esp_timer_get_time() - tmp_uart)/1000;
            if(active_cmc == "B"){
                if(strcmp(pEntrada->getValue().c_str(),"") != 0){
                    cmd = pEntrada->getValue().c_str();
                    pEntrada->setValue("");
                    tempo_passado = 11;
                }
            }
            if(active_cmc == "W"){
                
                if(sock >= 0){
                    struct sockaddr_storage source_addr;
                    socklen_t socklen = sizeof(source_addr);
                    int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
                    if (len >= 0){
                        //inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1); obter quem enviou
                        rx_buffer[len] = 0;
                        wbuffer_ent = rx_buffer;
                    }
                    
                    if(wbuffer_sai != ""){
                        struct sockaddr_in client;
                        client.sin_addr.s_addr = inet_addr("192.168.4.2");
                        client.sin_family = AF_INET;
                        client.sin_port = htons(PORT);
                        addr_family = AF_INET;
                        ip_protocol = IPPROTO_IP;
                        int err = sendto(sock, wbuffer_sai.c_str(), wbuffer_sai.length(), 0, (struct sockaddr *)&client, sizeof(client));
                        if (err < 0) {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                        }
                        else{
                            wbuffer_sai = "";
                        }
                    }
                }

                if (sock != -1) {
                    shutdown(sock, 0);
                    close(sock);
                    UDP_INIT();
                }

                
                //printf((active_cmc + "\n").c_str());
            }

            
            xSemaphoreGive(commMutex);
            long tmp = esp_timer_get_time();
            while (esp_timer_get_time() - tmp <= 500);
        }
        
    }
}




