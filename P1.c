//autor: Theógenes Gabriel Arújo de Andrade
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include <time.h>
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "ws2812b.pio.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
#define LED_COUNT 25
#define LED_PIN 7
#define PIN_VERDE 11
#define PIN_AZUL 12
#define BTN_A 5
#define BTN_B 6

volatile bool estA = false;
volatile bool estB = false;
volatile char l = '\n';
static void gpio_irq_handler(uint gpio, uint32_t events);
static volatile uint32_t tmp_ant = 0;

// Estrutura para armazenar cor do LED
typedef struct {
    uint8_t R, G, B;
} led;

led matriz_led[LED_COUNT];

uint32_t valor_rgb(uint8_t B, uint8_t R, uint8_t G) {
    return (G << 24) | (R << 16) | (B << 8);
}

void set_led(uint8_t indice, uint8_t r, uint8_t g, uint8_t b) {
    if (indice < LED_COUNT) {
        matriz_led[indice] = (led){r, g, b};
    }
}

void clear_leds() {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        matriz_led[i] = (led){0, 0, 0};
    }
}

void print_leds(PIO pio, uint sm) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        pio_sm_put_blocking(pio, sm, valor_rgb(matriz_led[i].B, matriz_led[i].R, matriz_led[i].G));
    }
}

// Rotina para exibir números na matriz WS2812
void num(uint8_t value, PIO pio, uint sm) {
        static const uint8_t segmentos[10][15] = {
            {23, 22, 21, 16, 13, 6, 3, 2, 1, 8, 11, 18, 0},     // 0
            {21, 18, 17, 11, 8, 1, 0},                          // 1
            {1, 2, 3, 6, 12, 18, 21, 22, 23, 0},                // 2
            {1, 2, 3, 8, 11, 12, 13, 18, 21, 22, 23, 0},        // 3
            {1, 8, 11, 12, 13, 16, 23, 21, 18, 0},              // 4
            {21, 22, 23, 16, 13, 12, 11, 8, 1, 2, 3, 0},        // 5
            {21, 22, 23, 16, 13, 12, 11, 8, 1, 2, 3, 6, 0},     // 6
            {23, 22, 21, 8, 11, 12, 10, 18, 1, 0},                      // 7
            {23, 22, 21, 16, 13, 6, 3, 2, 1, 8, 11, 18, 12, 0}, // 8
            {23, 22, 21, 16, 13, 1, 8, 11, 18, 12, 0}           // 9
        };

        clear_leds();
        for (int i = 0; segmentos[value][i] != 0; i++)
        {
            set_led(segmentos[value][i], 13, 13, 58);
        }
        print_leds(pio, sm);
}


// Interrupção para os botões
void gpio_irq_handler(uint gpio, uint32_t events) {

    static uint32_t tmp_ant = 0;
    uint32_t agora = to_ms_since_boot(get_absolute_time());
    
    // Debounce de 150 ms
    if (agora - tmp_ant > 195) {
        struct render_area frame_area = {
            start_column : 0,
            end_column : ssd1306_width - 1,
            start_page : 0,
            end_page : ssd1306_n_pages - 1
        };
        calculate_render_area_buffer_length(&frame_area);

        uint8_t ssd[ssd1306_buffer_length];
        memset(ssd, 0, ssd1306_buffer_length);

        // contorno da tela
        ssd1306_draw_string(ssd, 1, 1, "===============");
        ssd1306_draw_string(ssd, 1, 10, "=");
        ssd1306_draw_string(ssd, 1, 20, "=");
        ssd1306_draw_string(ssd, 1, 30, "=");
        ssd1306_draw_string(ssd, 1, 35, "=");
        ssd1306_draw_string(ssd, 1, 40, "=");
        ssd1306_draw_string(ssd, 1, 50, "=");
        ssd1306_draw_string(ssd, 113, 10, "=");
        ssd1306_draw_string(ssd, 113, 20, "=");
        ssd1306_draw_string(ssd, 113, 30, "=");
        ssd1306_draw_string(ssd, 113, 35, "=");
        ssd1306_draw_string(ssd, 113, 40, "=");
        ssd1306_draw_string(ssd, 113, 50, "=");
        ssd1306_draw_string(ssd, 1, 56, "===============");

        if (gpio == BTN_A && estB == false) {
            estA = !estA;
            gpio_put(PIN_VERDE, estA);

            if(estA){
                printf("Led Verde Ligado.\n");
                ssd1306_draw_string(ssd, 22, 32, "Verde:<ON>");
            }else{
                printf("Led Verde Desligado.\n");
                ssd1306_draw_string(ssd, 22, 32, "Verde:<OFF>");
            }
        } else if (gpio == BTN_B && estA == false) {
            estB = !estB;
            gpio_put(PIN_AZUL, estB);
            if(estB){
                printf("Led Azul Ligado.\n");
                ssd1306_draw_string(ssd, 22, 32, "Azul:<ON>");
            }else{
                printf("Led Azul Desligado.\n");
                ssd1306_draw_string(ssd, 22, 32, "Azul:<OFF>");
            }
        }

        render_on_display(ssd, &frame_area);
        tmp_ant = agora;
    }
}
// Inicialização do hardware
void init_hardware() {
    gpio_init(BTN_A);
    gpio_init(BTN_B);
    gpio_init(PIN_VERDE);
    gpio_init(PIN_AZUL);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_set_dir(PIN_VERDE, GPIO_OUT);
    gpio_set_dir(PIN_AZUL, GPIO_OUT);
    gpio_pull_up(BTN_A);
    gpio_pull_up(BTN_B);

    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BTN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

int main() {
    PIO pio = pio0;
    stdio_init_all();
    init_hardware();

    uint offset = pio_add_program(pio, &ws2812b_program);
    uint sm = pio_claim_unused_sm(pio, true);
    ws2812b_program_init(pio, sm, offset, LED_PIN);

    // Inicialização do I2C e display SSD1306
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    // Desenha o contorno do texto
    ssd1306_draw_string(ssd, 1, 1, "===============");
    ssd1306_draw_string(ssd, 1, 10, "=");
    ssd1306_draw_string(ssd, 1, 20, "=");
    ssd1306_draw_string(ssd, 1, 30, "=");
    ssd1306_draw_string(ssd, 1, 35, "=");
    ssd1306_draw_string(ssd, 1, 40, "=");
    ssd1306_draw_string(ssd, 1, 50, "=");
    ssd1306_draw_string(ssd, 113, 10, "=");
    ssd1306_draw_string(ssd, 113, 20, "=");
    ssd1306_draw_string(ssd, 113, 30, "=");
    ssd1306_draw_string(ssd, 113, 35, "=");
    ssd1306_draw_string(ssd, 113, 40, "=");
    ssd1306_draw_string(ssd, 113, 50, "=");
    ssd1306_draw_string(ssd, 1, 56, "===============");
    
    while (true) {
        //Verifica se o Usb está conectado
        if (stdio_usb_connected()){
            l = getchar(); // Captura um caractere
            if (l != '\n'){
                char msg[20];
                sprintf(msg, "Caracter:%c", l); // Formata a string corretamente

                ssd1306_draw_string(ssd, 22, 32, msg); // Exibe a string no display
                render_on_display(ssd, &frame_area);

                if (l >= '0' && l <= '9'){
                    num(l - '0', pio, sm);
                }
                else{
                    clear_leds();
                    print_leds(pio, sm);
                }
            }
        }

        sleep_ms(200);
    }

    return 0;
}