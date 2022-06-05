/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    ADCON1bits.PCFG = 0b1110; //AN0  como análogo los demás digitales
    
    PORTB=PORTA=PORTC=0;
    
    TRISAbits.TRISA0=1; //AN0 como entrada
    TRISCbits.RC6=1;    //serial TX
    TRISCbits.RC7=0;    //serial RX
    TRISCbits.RC0=0;    //led indicador
//    TRISAbits.TRISA2=1; //AN2 como entrada Vref-
//    TRISAbits.TRISA3=1; //AN0 como entrada Vref+
    
    INTCONbits.GIE=1; //habilitamos las interrupciones
    INTCONbits.PEIE=1;//habilitamos los periféricos
    
  /*conversión analogo digital*/
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1; //Activamos las interupciones de ADC
//    ADCON1bits.VCFG =0b11; //Vref-(AN2) y Vref+(AN3)
    ADCON2bits.ADFM = 1; //justificado a la derecha
    ADCON0bits.CHS = 0b000; //elegimos el AN0
    ADCON2bits.ACQT = 0b010; //tiempo de adquisición 2T_AD
    ADCON2bits.ADCS= 0b100; //tiempo de conversión ADC Fosc/4
    ADCON0bits.ADON = 1; //Activamos ADC
    
    /*comunicacion serial*/
    SPBRG = 10; //RESOLUCION DE 115200
    TXSTAbits.BRGH = 1; //habilitamos la alta velocidad
    TXSTAbits.SYNC = 0; //modo asincronico
    RCSTAbits.SPEN = 1; //habilitamos los periféricos TX y RX (USART)
    TXSTAbits.TXEN = 1; //habilitamos la transmisión
    //RCSTAbits.CREN = 1; //habilitamos la recepción
    BAUDCONbits.BRG16=0;
    RCSTAbits.SPEN=1;
    
    
    /*timer 0 (led de estado)*/
    T0CONbits.T08BIT = 0;       // 16 bits
    T0CONbits.T0CS = 0;         // Internal instruction cycle clock (CLKO)
    T0CONbits.PSA = 0;          //Enables Prescaler
    T0CONbits.T0PS = 0b000;     // prescaler 1:2
    TMR0 = 0xF63C;
    INTCONbits.TMR0IF=0;        //limpiamos la bandera
    INTCON2bits.TMR0IP = 1;     //alta proridad
    T0CONbits.TMR0ON = 1;       //activamos el timer0
    
    /* Enable interrupts */
    INTCONbits.TMR0IE = 1;
}

