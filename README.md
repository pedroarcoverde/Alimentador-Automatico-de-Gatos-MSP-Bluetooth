# Alimentador Automático de Gatos
### Este projeto consiste em um alimentador automático de gatos controlado por um microcontrolador MSP430. O sistema utiliza um motor de passo para liberar uma quantidade específica de ração em horários programados ou através de comandos enviados via Bluetooth.

## Funcionalidades
- Liberação de ração em horários programados: O alimentador libera ração automaticamente em horários pré-definidos (por exemplo, às 10h e às 20h).
- Controle manual via Bluetooth: É possível acionar o motor manualmente enviando comandos via um módulo Bluetooth HM-10.
- Motor de passo controlado por driver: Um motor de passo é utilizado para girar uma mola dentro de um tubo de PVC, transportando a ração até o pote.
- Simplicidade e baixo custo: O projeto utiliza componentes acessíveis e é fácil de montar.

## Componentes Utilizados
- Microcontrolador MSP430
- Módulo Bluetooth HM-10 (BLE 4.0)
- Motor de passo com driver
- Canos de PVC para transporte da ração
- Mola de arame galvanizado
- Resistores, jumpers e protoboard

## Esquema de Montagem
Motor de passo: Conectado ao driver, que é controlado pelo MSP430.
Módulo Bluetooth HM-10: Conectado aos pinos de UART do MSP430 (P1.1 para RX e P1.2 para TX).
Tubo de PVC: A mola ou parafuso sem-fim é acoplada ao motor de passo e inserida no tubo de PVC para transportar a ração.
Pote de ração: Posicionado abaixo da saída do tubo de PVC para coletar a ração liberada.

## Como Funciona
O MSP430 verifica o horário atual e, se for um dos horários programados, aciona o motor de passo para liberar ração.
O usuário pode enviar o comando run via Bluetooth para liberar ração manualmente.
O motor de passo gira a mola dentro do tubo de PVC, transportando a ração até o pote.

## Código Fonte
O código fonte está disponível na pasta src deste repositório. Ele foi desenvolvido em C para a MSP430 e inclui:

- Controle do motor de passo.
- Comunicação Bluetooth com o módulo HM-10.
- Verificação de horários para liberação automática de ração.

### Estrutura do Código
main.c: Contém a lógica principal do programa.

configurarClock.c: Configura o clock da MSP430.

configurarUART.c: Configura a comunicação UART com o módulo Bluetooth.

configurarGPIO.c: Configura os pinos de GPIO para controle do motor de passo.

## Como Usar
1. Conecte o motor de passo ao driver e ao MSP430.

2. Conecte o módulo Bluetooth HM-10 aos pinos de UART do MSP430.

3. Carregue o código na MSP430 usando um programador compatível (ex: MSP430 LaunchPad).

4. Alimente o sistema com uma fonte de energia adequada.

. Use um aplicativo Bluetooth (ex: Serial Bluetooth Terminal) para enviar comandos ou aguarde os horários programados para a liberação automática de ração.

### Comandos Bluetooth
- run: Libera ração manualmente.
- config1: configura o horario da primeira ração diaria (em desenvolvimento).
- config2: configura o horario da segunda ração diaria (em desenvolvimento).
- status: Retorna o status atual do alimentador (em desenvolvimento).
