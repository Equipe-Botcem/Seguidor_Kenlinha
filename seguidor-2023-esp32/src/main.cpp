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
string cmd = "";
unsigned long tmp_uart = 0;

void BLE_INIT();
void readUartBLE();

//Leituras
void continuous_adc_INIT();
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
    gpio_isr_handler_add((gpio_num_t)16, enc1, (void*) 0);
    gpio_isr_handler_add((gpio_num_t)17, enc2, (void*) 0);
    
    //ini
    BLE_INIT();
    continuous_adc_INIT();
    cont_time = esp_timer_get_time();

    seguidor.init();
    seguidor.output(Robo + " pronto!");

    //main loop
    while(1) {  
        
        if(esp_timer_get_time() - cont_time >= 500000){
            cont_time = esp_timer_get_time();
            cont_leituras = 0;
            if (seguidor.Estado_corrida == true){
                seguidor.printEncoders();
            }
        }

		if (seguidor.Estado_corrida == true){
            if(leitura_atualizada){
                seguidor.seguir_linha();
                cont_leituras++;
                leitura_atualizada = false;
            }
			
		}
		else if(seguidor.get_modo() != 'B' && (seguidor.get_modo() != 'J')){
			seguidor.stop("Loop");
		}
        
        readUartBLE();
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




