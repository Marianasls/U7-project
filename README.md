# Projeto da Unidade 7: Sistema de Irrigação Inteligente com BitDogLab RP2040

## Descrição
Este projeto implementa um **sistema de irrigação inteligente** simulado utilizando a **placa BitDogLab RP2040**. O sistema monitora a umidade e a temperatura para controlar a ativação da irrigação, exibindo informações no **display SSD1306**.

## Funcionalidades
- **Leitura de umidade e temperatura** através dos potenciômetros do joystick:
  - **Eixo X** simula a umidade (**0% a 100%**).
  - **Eixo Y** simula a temperatura (**0°C a 80°C**).
- **Controle da irrigação**:
  - O **LED vermelho** simula a bomba d'água.
  - A irrigação ativa quando a umidade está **abaixo de 30%** e desliga quando está **acima de 60%**.
  - A temperatura influencia o acionamento da irrigação, com alerta a partir de **28°C**.
- **Exibição no display SSD1306**:
  - Informações da leitura de **umidade e temperatura**.
  - Status do solo com base nas leituras (**alerta/normal/adequado**).


## Como Funciona
1. O sistema lê os valores de temperatura e umidade dos potenciômetros.
2. Se a umidade estiver **abaixo de 30%**, a bomba d'água (LED vermelho) é ativada.
3. Quando a umidade atinge **60% ou mais**, a bomba desliga.
4. O display SSD1306 exibe os valores de temperatura, umidade e status do solo.
5. A cada **500 ms**, uma nova leitura é realizada.

## Dependências
- **Placa BitDogLab RP2040 - Raspberry Pi Pico W**
- **Display SSD1306 (I2C)**
- **Bibliotecas do Raspberry Pi Pico SDK**
- **Biblioteca SSD1306 para exibição de dados**
- **LED RGB**

## Instalação e Execução
1. Configure o ambiente de desenvolvimento para o **RP2040**, com o vscode e extensão "Raspberry Pi Pico".
2. Importe o projeto e escolha o SDK 2.1.1 (utilizado no projeto)
3. Faça o build do projeto com o comando make (Configure CMake na extensão)
4. Compile e carregue o código na placa.
5. Monitore os valores exibidos no display SSD1306.

