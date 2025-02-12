#include <msp430.h> 
#include <stdio.h>
#include <stdint.h>
#include <string.h>


// Variáveis globais para controles de tempo e buffers
volatile uint8_t segundos = 0;
volatile uint8_t minutos = 0;
volatile uint8_t horas = 0;

volatile uint8_t h1_racao = 10;
volatile uint8_t h2_racao = 20;
volatile uint8_t m1_racao = 0;
volatile uint8_t m2_racao = 0;

#define BUFFER_SIZE 32
volatile char rxBuffer[BUFFER_SIZE];
volatile unsigned int rxPtr = 0;

// Variável para armazenar o comando recebido
char comandoRecebido[10];
uint8_t indiceComando = 0;


void configuraUART() {
    P3SEL |= BIT3 + BIT4;    //CONFIGURA P3.3 TXD E P3.4 RXD
    UCA0CTL1 |= UCSWRST;     //COLOCA UART EM RESET
    UCA0CTL1 |= UCSSEL_2;     //SELECIONAR SMCLK
    UCA0BR0 |= 104;     //BAUD RATE 9600
    UCA0BR1 |= 0;     //DESLIGA SEGUNDO CANAL
    UCA0MCTL |= UCBRS0;     //MODULAÇÃO
    UCA0CTL1 &= ~UCSWRST;     //SAIR DO RESET
    UCA0IE |= UCRXIE;     //HABILITA INTERRUÇÃO DE RECEPÇÃO
}

void configuraEnvioUART() {
    UCA1CTL1  = UCSWRST;
	UCA1CTL1 |= UCSSEL__SMCLK;
	UCA1BRW   =	6;
	UCA1MCTL = UCOS16 | UCBRF_13;
	P4SEL    |= BIT4 | BIT5;
	UCA1CTL1 &= ~UCSWRST;
	UCA1IE |= UCRXIE;
}


// Funçãoo para enviar uma string via UART
void enviaUART(char * str) {

    while(*str){
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *str++;

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


// Configuraçãoo do Motor
void configuraGPIO() {

    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
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


// Configuração do Timer_A para contar segundos
void configuraTimer() {
    TA0CTL = TASSEL_2 | MC_1 | ID_3;  // Usa SMCLK (8MHz), modo up, divisor /8
    TA0CCR0 = 62500 - 1;              // Conta até 62500 (1 segundo com 8MHz / 8)
    TA0CCTL0 = CCIE;                  // Habilita interrupção do Timer
}





#pragma vector = USCI_A1_VRCTOR
__interrupt void USCI_A1_ISR(void) {

    switch(__even_in_range(UCA1IV, 4)) {
        case 0: break;
        case 2:
            rxBuffer[rxPtr] = UCA1RXBUF;
            rxPtr = (rxPtr + 1) % BUFFER_SIZE;

        case 4: break;
        default: break;
    }
}

// Interrupção de recepção da UART
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
    char entrada = UCA0RXBUF;  //LE O CARACTER RECEBIDO

    if (entrada == 'r'){
		run_motor();
        enviaUART("Motor acionado manualmente.\r\n");

    } else if (entrada == '1'){
		enviaUART(entrada);
		enviaUART(": o comando esta em desenvolvimento.\r\n");
        // RECEBE A CONFIGURAÇÃO DE HORA PARA A PRIMEIRA RAÇÃO

    } else if (entrada == '2'){
		enviaUART(entrada);
		enviaUART(": o comando esta em desenvolvimento.\r\n");
        // RECEBE A CONFIGURAÇÃO DE MINUTO PARA A PRIMEIRA RAÇÃO

    } else if (entrada == '3'){
		enviaUART(entrada);
		enviaUART(": o comando esta em desenvolvimento.\r\n");
        // RECEBE A CONFIGURAÇÃO DE HORA PARA A SEGUNDA RAÇÃO

    } else if (entrada == '4'){
		enviaUART(entrada);
		enviaUART(": o comando esta em desenvolvimento.\r\n");
        // RECEBE A CONFIGURAÇÃO DE MINUTO PARA A SEGUNDA RAÇÃO
        
    } else {
        enviaUART("Comando desconhecido.\r\n");
    }

}


// Interrupção do Timer_A
#pragma vector = TIMER0_A0_VECTOR
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





int main(void) {
    // Configurações iniciais
    WDTCTL = WDTPW | WDTHOLD;  // Desabilita o Watchdog Timer

    configuraGPIO();
    configuraUART();
    configuraEnvioUART();
    configuraTimer();
    __enable_interrupt();           //HABILITA INTERRUPÇÕES GLOBAIS

    // Mensagem inicial
    enviaUART("Sistema iniciado.\r\n");
    enviaUART("Digite 'r' para acionar o motor manualmente.\r\n");

    while (1) {
        // Verifica se é hora de acionar o motor no primeiro horario
        if (is_time_to_run(h1_racao, m1_racao)) {
            run_motor();
        }

        // Verifica se é hora de acionar o motor no segundo horario
        if (is_time_to_run(h2_racao, m2_racao)) {
            run_motor();
        }

        __bis_SR_register(LPM0_bits | GIE);  // Entra em modo de baixo consumo com interrupções habilitadas
    }
}





        // Verifica se hà comandos recebidos via Bluetooth
        //if (indiceComando > 0) {

        //    enviaUART(comandoRecebido);

        //     if (strcmp(comandoRecebido, "run") == 0) {
        //         run_motor();
        //         enviaUART("Motor acionado manualmente.\r\n");

        //     }
        //     else if(strcmp(comandoRecebido, "config1") == 0){
        //         enviaUART("Comando em desenvolvimento.\r\n");

        //     }
        //     else if(strcmp(comandoRecebido, "config2") == 0){
        //         enviaUART("Comando em desenvolvimento.\r\n");

        //     }
        //     else if(strcmp(comandoRecebido, "status") == 0){
        //         enviaUART("Comando em desenvolvimento.\r\n");

        //     }
        //     else {
        //         enviaUART("Comando desconhecido.\r\n");

        //     }
        //     indiceComando = 0;  // Reseta o índice do comando
        //     comandoRecebido[indiceComando] = '\0';  // Finaliza a string
        // }



    //if (entrada == '\r' || entrada == '\n') {
    //    comandoRecebido[indiceComando] = '\0';  // Finaliza o comando
    //    indiceComando = 0;                     // Reseta o índice
    //} else {
    //    comandoRecebido[indiceComando++] = entrada;  // Armazena o caractere
    //    if (indiceComando >= 8) {
    //        indiceComando = 0;  // Evita overflow
    //    }
    //}
