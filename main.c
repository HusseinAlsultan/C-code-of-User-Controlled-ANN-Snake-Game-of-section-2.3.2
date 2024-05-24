#include <msp430.h>

void UART_Setup(void);               // set-up UART A0 and A1 ports
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer

    UART_Setup();

    PM5CTL0 &= ~LOCKLPM5;           //turn on GPIO system

     P1DIR |= BIT0;                 //make P1.0 (LED1) in an output mode
     P1OUT &= ~BIT1;                //turn off LED1 initially
    //--setup IRQ A1 RXIFG
    UCA0IE |= UCRXIE;               //local enable for A0 RXIFG
    UCA1IE |= UCRXIE;               //local enable for A1 RXIFG
    __enable_interrupt();           //global Enable for maskable interrupts...(GIE bits in SR)

    while(1) {}                     //do nothing

    return 0;
}

//----------------------------------------------------------------
//---ISRs---------------------------------------------------------
#pragma vector = EUSCI_A0_VECTOR
__interrupt void EUSCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
      {
        case USCI_NONE:
            break;

        case USCI_UART_UCRXIFG:
        {
            UCA1TXBUF = UCA0RXBUF;
            break;
        }

        case USCI_UART_UCTXCPTIFG:
            break;

        default:
            break;
      }
}

#pragma vector = EUSCI_A1_VECTOR
__interrupt void EUSCI_A1_ISR(void)
{
    switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
      {
        case USCI_NONE:
            break;

        case USCI_UART_UCRXIFG:
        {
            UCA0TXBUF = UCA1RXBUF;
            break;
        }

        case USCI_UART_UCTXCPTIFG:
            break;

        default:
            break;
      }
}

void UART_Setup(void)
{
    //I used default UART framing; 8-bit, LSB first, No Parity, No address, No extra Stop Bit

    //-----------set up UART A0
    UCA0CTLW0 |= UCSWRST;              //put UART A0 in to SW reset

    UCA0CTLW0 |= UCSSEL__SMCLK;        //choose SMCLK for UART A0(at 115200 BR)
    UCA0BRW = 8;                       //set Prescalar to 8
    UCA0MCTLW = 0xD600;                //setup modulation settings + low frequency

    //--configure pins for UART A0 RX
    P1SEL1 &= ~BIT6;                   //setup P1.6 to use UART A0 RX instead of digital I/O
    P1SEL0 |= BIT6;                    //P1SEL1.6 : P1SEL0.6 = 01

    //--configure pins for UART A0 RX
    P1SEL1 &= ~BIT7;                   //setup P1.7 to use UART A0 TX instead of digital I/O
    P1SEL0 |= BIT7;                    //P1SEL1.7 : P1SEL0.7 = 01


    //-----------set up UART A1
    UCA1CTLW0 |= UCSWRST;              //put UART A1 in to SW reset

    UCA1CTLW0 |= UCSSEL__SMCLK;        //choose SMCLK for UART A1(at 115200 BR)
    UCA1BRW = 8;                       //set Prescalar to 8
    UCA1MCTLW = 0xD600;                //setup modulation settings + low frequency

    //--configure pins for UART A1 RX
    P4SEL1 &= ~BIT2;                   //setup P4.2 to use UART A1 RX instead of digital I/O
    P4SEL0 |= BIT2;                    //P4SEL1.2 : P4SEL0.2 = 01

    //--configure pins for UART A1 TX
    P4SEL1 &= ~BIT3;                   //setup P4.3 to use UART A1 TX instead of digital I/O
    P4SEL0 |= BIT3;                    //P4SEL1.3 : P4SEL0.3 = 01

    UCA0CTLW0 &= ~UCSWRST;             //take UART A0 out of SW reset
    UCA1CTLW0 &= ~UCSWRST;             //take UART A1 out of SW reset
}
