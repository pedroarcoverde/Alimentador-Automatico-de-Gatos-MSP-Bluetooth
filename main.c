#include <msp430.h> 
#include <stdio.h>
#include <stdint.h>
#include <string.h>


void configurarClock();
void configurarUART();
void configurarGPIO();
void configurarTimer();
void run_motor();
int is_time_to_run(int target_hour, int target_min);
void enviarStringUART(const char *str);


// Variáveis globais para controle de tempo
volatile uint8_t segundos = 0;
volatile uint8_t minutos = 0;
volatile uint8_t horas = 0;


// Variável para armazenar o comando recebido
char comandoRecebido[10];
uint8_t indiceComando = 0;



int main(void) {
    // Configurações iniciais
    WDTCTL = WDTPW | WDTHOLD;  // Desabilita o Watchdog Timer

    configurarClock();
    configurarGPIO();
    configurarUART();
    configurarTimer();

    // Mensagem inicial
    enviarStringUART("Sistema iniciado.\r\n");
    enviarStringUART("Digite 'run' para acionar o motor.\r\n");

    while (1) {
        // Verifica se é hora de acionar o motor às 10h
        if (is_time_to_run(10, 0)) {
            run_motor();
        }

        // Verifica se é hora de acionar o motor às 20h
        if (is_time_to_run(20, 0)) {
            run_motor();
        }

        // Verifica se há comandos recebidos via Bluetooth
        if (indiceComando > 0) {
            comandoRecebido[indiceComando] = '\0';  // Finaliza a string
            if (strcmp(comandoRecebido, "run") == 0) {
                run_motor();
                enviarStringUART("Motor acionado manualmente.\r\n");
            } else {
                enviarStringUART("Comando desconhecido.\r\n");
            }
            indiceComando = 0;  // Reseta o índice do comando
        }

        __bis_SR_register(LPM0_bits | GIE);  // Entra em modo de baixo consumo com interrupções habilitadas
    }
}


// Função para acionar o motor
void run_motor() {

      // gira o motor no sentido horário
      int i;
      for(i = 0; i<1000; i++)
      {

        P2OUT |= BIT2;
        P2OUT &= ~BIT0;
        P2OUT &= ~BIT5;
        P2OUT &= ~BIT4;
        __delay_cycles(1000);

        P2OUT |= BIT2;
        P2OUT |= BIT0;
        P2OUT &= ~BIT5;
        P2OUT &= ~BIT4;
        __delay_cycles(1000);

        P2OUT &= ~BIT2;
        P2OUT |= BIT0;
        P2OUT &= ~BIT5;
        P2OUT &= ~BIT4;
        __delay_cycles(1000);

        P2OUT &= ~BIT2;
        P2OUT |= BIT0;
        P2OUT |= BIT5;
        P2OUT &= ~BIT4;
        __delay_cycles(1000);

        P2OUT &= ~BIT2;
        P2OUT &= ~BIT0;
        P2OUT |= BIT5;
        P2OUT &= ~BIT4;
        __delay_cycles(1000);

        P2OUT &= ~BIT2;
        P2OUT &= ~BIT0;
        P2OUT |= BIT5;
        P2OUT |= BIT4;
        __delay_cycles(1000);

        P2OUT &= ~BIT2;
        P2OUT &= ~BIT0;
        P2OUT &= ~BIT5;
        P2OUT |= BIT4;
        __delay_cycles(1000);

        P2OUT |= BIT2;
        P2OUT &= ~BIT0;
        P2OUT &= ~BIT5;
        P2OUT |= BIT4;
        __delay_cycles(1000);

      }
  P2OUT |= BIT2;
  P2OUT &= ~BIT0;
  P2OUT &= ~BIT5;
  P2OUT &= ~BIT4;
  __delay_cycles(1000);

}



// Configuração do Motor
void configurarGPIO() {

    P2DIR |= BIT0 | BIT2 | BIT4 | BIT5;
    P2OUT &= ~(BIT0 | BIT2 | BIT4 | BIT5);
      // bobina 1 -> 2.2
      //        2 -> 2.0
      //        3 -> 2.5
      //        4 -> 2.4
}


// Função para verificar se é hora de acionar o motor
int is_time_to_run(int target_hour, int target_min) {
    if (horas == target_hour && minutos == target_min && segundos == 0) {
        return 1;
    }
    return 0;
}


// Função para enviar uma string via UART
void enviarStringUART(const char *str) {
    while (*str) {
        while (!(UCA0IFG & UCTXIFG));  // Espera o buffer de transmissão estar livre
        UCA0TXBUF = *str++;            // Envia o caractere
    }
}


// Configuração do clock
void configurarClock() {
    // Configura o DCO para 8MHz
    UCSCTL1 = DCORSEL_5;
    UCSCTL4 = SELA_2 | SELS_3 | SELM_3;  // ACLK = XT1, SMCLK = DCO, MCLK = DCO
}


void configurarUART() {
    // Configura os pinos da UART (P1.1 = RX, P1.2 = TX)
    P1SEL |= BIT1 | BIT2;

    // Configura a UART para 9600 baud rate (ajuste conforme o HM-10)
    UCA0CTL1 |= UCSWRST;  // Reseta a UART
    UCA0CTL1 |= UCSSEL_2; // Usa SMCLK como clock
    UCA0BR0 = 104;        // 9600 baud rate (1MHz / 9600)
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;    // Modulação
    UCA0CTL1 &= ~UCSWRST; // Libera a UART

    // Habilita a interrupção de recepção
    UCA0IE |= UCRXIE;  // Habilita a interrupção de recepção
}


// Configuração do Timer_A para contar segundos
void configurarTimer() {
    TA0CTL = TASSEL_2 | MC_1 | ID_3;  // Usa SMCLK (8MHz), modo up, divisor /8
    TA0CCR0 = 62500 - 1;              // Conta até 62500 (1 segundo com 8MHz / 8)
    TA0CCTL0 = CCIE;                  // Habilita interrupção do Timer
}


// Interrupção de recepção da UART
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
    if (UCA0RXBUF == '\r' || UCA0RXBUF == '\n') {
        comandoRecebido[indiceComando] = '\0';  // Finaliza o comando
        indiceComando = 0;                     // Reseta o índice
    } else {
        comandoRecebido[indiceComando++] = UCA0RXBUF;  // Armazena o caractere
        if (indiceComando >= sizeof(comandoRecebido) - 1) {
            indiceComando = 0;  // Evita overflow
        }
    }
}


// Interrupção do Timer_A
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
    segundos++;
    if (segundos >= 60) {
        segundos = 0;
        minutos++;
        if (minutos >= 60) {
            minutos = 0;
            horas++;
            if (horas >= 24) {
                horas = 0;
            }
        }
    }
    __bic_SR_register_on_exit(LPM0_bits);  // Sai do modo de baixo consumo
}
