/**
 * Sistema de irrigação inteligente simulado com a placa BitDogLab RP2040.
 * O sensor de umidade e temperatura foi simulado com o uso dos potenciometros no joystick. Eixo X para umidade e Eixo Y para temperatura.
 * A bomba d'agua foi simulado com o uso do LED vermelho.
 * O Display SSD1306 exibe informações sobre as leituras e status da qualidade do solo.
 * Autor: Mariana Santos.
 * 
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"      
#include "hardware/pwm.h"
#include "include/ssd1306.h"

// Definição dos pinos
#define I2C_PORT i2c1   // I2C1 em GPI14 (SDA) e GPI15 (SCL)
#define I2C_SDA 14
#define I2C_SCL 15
#define address 0x3C
#define VRX_PIN 26      // Eixo X joysitck
#define VRY_PIN 27      // Eixo Y joystick
#define SW_PIN 22       // botão joystick
#define BUTTON_A 5      // Botão A
#define PWM_WRAP 4096    
#define CLKDIV 4.0f     // Divisor do clock, freq calculada de 122.07 kHz
#define ADC_MAX 4095    // Valor máximo do ADC
#define ADC_CENTER 2048 // Posição central do joystick
#define PUMP_PIN 13
// Definição dos limites da tela do display 
#define DISPLAY_WIDTH  128
#define DISPLAY_HEIGHT 64
// Limites de umidade e temperatura
#define TEMP_MAX 80
#define HUM_MAX 100
#define HUM_ALERT 30
#define HUM_OK 60
#define TEMP_ALERT 28
#define TIME_READ 500
// Variaveis globais
static volatile uint8_t state = 0;
ssd1306_t ssd;

// Prototipo das funções
void init_all_pins();
uint setup_pwm(uint pin);
float read_humity();
float read_temperature();
void control_irrigation();
void set_pump_intensity(uint16_t intensity);

int main() {
    stdio_init_all();
    init_all_pins();

    // Configura PWM nos pinos do eixo X e Y do joystick
    uint slice_x = setup_pwm(VRX_PIN); 
    uint slice_y = setup_pwm(VRY_PIN); 

    // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, address, I2C_PORT); 
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd);

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    set_pump_intensity(20); // intensidade reduzida 
    
    while(true){
        control_irrigation();
        sleep_ms(TIME_READ);
    }
    return 0;
}

void init_all_pins() {
    // Inicializa os LEDs RGB
    gpio_init(PUMP_PIN);              // Inicializa o pino do LED RED, simulando bomba d'agua
    gpio_set_dir(PUMP_PIN, GPIO_OUT); // Configura o pino como saída
    
    // Inicializa os botões
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);    // Configura o pino como entrada
    gpio_pull_up(BUTTON_A);  

    // Inicializa ADC
    adc_init();
    adc_gpio_init(VRX_PIN); 
    adc_gpio_init(VRY_PIN); 
    // Inicializa I2C com frequência de 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Configura pino PWM
uint setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);  // Define o pino como PWM
    uint slice_num = pwm_gpio_to_slice_num(pin);
    
    //pwm_set_clkdiv(slice_num, CLKDIV); 
    pwm_set_wrap(slice_num, PWM_WRAP);
    pwm_set_enabled(slice_num, true);
    return slice_num;
} 

void set_pump_intensity(uint16_t intensity){
    // Configura o pino 13 como saída PWM
    gpio_set_function(PUMP_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PUMP_PIN);

    pwm_set_wrap(slice_num, 255);  // 8 bits (0-255)
    pwm_set_gpio_level(PUMP_PIN, intensity); 
    // Ativa o PWM
    pwm_set_enabled(slice_num, true);
}

float read_humity() {
    adc_select_input(0);  // Seleciona o canal do eixo X para simular leitura da umidade
    uint adc_x = adc_read();
    printf("\nX: %d\n", adc_x);
    pwm_set_gpio_level(VRX_PIN, adc_x); 
    // Converte os valores ADC (0-4095) para umidade (0-100%)
    float value = (((float)adc_x / (float)ADC_MAX) * 100);
    return value;
}
float read_temperature() {
    adc_select_input(1);  // Seleciona o canal do eixo Y para simular leitura da temperatura
    uint adc_y = adc_read();
    printf("Y: %d\n", adc_y);
    pwm_set_gpio_level(VRY_PIN, adc_y); 
    // Converte os valores ADC (0-4095) para temperatura (0 a 80°C)
    float value = (((float)adc_y / (float)ADC_MAX) * 80);
    return value;
}

void control_irrigation() {
    uint humity, temperature;
    humity = read_humity();
    temperature = read_temperature();
    printf("umid: %d\n", humity);
    printf("temp: %d\n", temperature);

    char buffer[20];
    ssd1306_fill(&ssd, false);
    // exibe no display as informações
    snprintf(buffer, sizeof(buffer), "UMID %d", humity);
    printf("\n\nbuffer: %s \t", buffer);
    ssd1306_draw_string(&ssd, buffer, 10, 10); // Desenha uma string
    
    snprintf(buffer, sizeof(buffer), "TEMP %d C", temperature);
    printf(buffer);
    ssd1306_draw_string(&ssd, buffer, 10, 30); // Desenha uma string

    if(humity <= HUM_ALERT || (temperature >= TEMP_ALERT && humity <= HUM_OK)){
        ssd1306_draw_string(&ssd, "ALERTA", 10, 50);
        // ativa led alerta (bomba d'agua)
        pwm_set_gpio_level(PUMP_PIN, 20); 
        state=1;
    } else if(state && humity >= HUM_OK){
        // desliga alerta 
        pwm_set_gpio_level(PUMP_PIN, 0); 
        state=0;
    }
    if(!state){
        if(humity >= HUM_OK) {
            // escreve no display emoji cara feliz
            ssd1306_draw_face(&ssd, 63*8, 10, 50);
            ssd1306_draw_string(&ssd, "adequado", 20, 50);
        }else {
            ssd1306_draw_string(&ssd, "normal", 10, 50);
        }
    }
    ssd1306_send_data(&ssd); // Atualiza o display
}