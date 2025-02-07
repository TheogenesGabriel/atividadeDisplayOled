# Projeto: Comunicação Serial no RP2040 com BitDogLab

## Descrição
Este projeto utiliza a placa **BitDogLab** para consolidar conceitos de comunicação serial no **RP2040**. O sistema interage com o usuário por meio de **UART**, um **display SSD1306**, uma **matriz de LEDs WS2812** e **botões com interrupções** para controle de um **LED RGB**.

## Funcionalidades
- Entrada de caracteres via **UART** e exibição no **display SSD1306**.
- Exibição de símbolos na **matriz 5x5 WS2812** para números de 0 a 9.
- Controle de um **LED RGB** através de botões, utilizando **interrupções e debounce**.
- Mensagens informativas no **display SSD1306** e no **Serial Monitor** sobre a interação com os botões.
- Modificação da biblioteca `font.h` para incluir caracteres minúsculos.
- Organização e documentação do código para facilitar futuras manutenções.
- Implementação de debounce para evitar leituras falsas nos botões.
- Exibição de caracteres recebidos via UART no display SSD1306.

## Hardware Utilizado
- Placa **BitDogLab (RP2040)**
- **Display SSD1306** (I2C)
- **Matriz de LEDs WS2812** (5x5)
- **LED RGB** (conectado aos GPIOs 11, 12 e 13)
- **Botões** para interação

## Demonstração
O funcionamento do projeto pode ser visto no vídeo demonstrativo disponível em:
[Link para o vídeo]

## Autor
Theógenes Gabriel Araújo de Andrade
