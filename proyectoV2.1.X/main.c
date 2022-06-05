/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
#include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#define _XTAL_FREQ 20000000
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

int resultadoADC;
extern bool conversionADC;
char salida[10];
char longer = 64;
char wid = 6;
int array[64][7]; //Matriz de coeficientes Wavelet array[longer][wid+1]
char n = 0;
char scale;
char l;
char m;
char a = 2; //Qué aproximación Wavelet vamos a usar {de wid hasta 0, con 0 la peor}
char multi = 16; //(longer/2^a)
int data[64]; //Vector de coeficientes Wavelet Haar de aproximación 'a' data[longer]
int dato;
bool sa = 0;
extern bool entrada;


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void Transmision(char *salida);
void arrayADC(void);
void transmision2(void);

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    /* TODO <INSERT USER APPLICATION CODE HERE> */
    
    while (1) {
//    PORTCbits.RC2=1;
        /*****************Módulo ADC*****************/
        if (ADCON0bits.GODONE == 0) {
           
            array[n][0] = ADRES; //Alamacenamos en la memoria del programa
            ADCON0bits.GODONE = 1; //Lanzar la conversión
            n++; //Número de posiciones de almacenamiento

        }

        /****************Modulo wavelet*************/
        if (n == longer && conversionADC == 1) {
            //TMR0 = 0x3CB0; //20 mseg
            ADCON0bits.ADON = 0; //Desactivamos ADC  
            conversionADC = 0;
            scale = longer;
            n = 0;
            for (char j = 0; j < wid; j++) {
                m = 0;
                for (char i = 0; i < scale; i = i + 2) {
                    l = 0;
                    l = i + 1;
                    array[m][j + 1] = (array[i][j] + array[l][j]) / 2; //High Filter
                    array[(scale / 2) - 1 + m][j + 1] = (array[l][j] - array[i][j]) / 2; //Low Filter
                    m++;
                }
                for (char i = scale; i < longer; i++) {
                    array[i][j + 1] = array[i][j];
                }
                scale = scale / 2; //Re-escalar
            }
            l = 0;
            for (int i = 0; i < longer; i = i + multi) {
                dato = array[l][a - 2];
                for (char k = 0; k < multi; k++) {
                    data[i + k] = dato;
                }
                l++;
            }
            //arrayADC(); //Para mandar la señal reconstruida
            transmision2(); //Para mandar a la App
        }
    }
}

void Transmision(char *salida) {
    while (*salida != '\0') {
        TXREG = *salida;
        while (TXSTAbits.TRMT == 0);
        salida++;
    }
    TXREG = 13;
    while (TXSTAbits.TRMT == 0);
    TXREG = 10;
    while (TXSTAbits.TRMT == 0);
}

void arrayADC(void) {

    for (char i = 0; i < longer; i++) {
        resultadoADC = data[i];
        salida[0] = (resultadoADC / 1000) + 48;
        salida[1] = ((resultadoADC / 100) % 10) + 48;
        salida[2] = ((resultadoADC / 10) % 10) + 48;
        salida[3] = ((resultadoADC / 1) % 10) + 48;
        Transmision(salida);
    }
    entrada = 0;
}

void transmision2(void) {
    for (char i = 0; i < 64; i++) {
        if (data[i] < 335) {
            sa = 0;
            continue;
        } else {
            sa = 1;
            break;
        }
    }
    if (sa == 1) {
        TXREG = '5'; //Enviar 5
        while (TXSTAbits.TRMT == 0);
        __delay_ms(500);
        sa = 0;
    } else {
        TXREG = '0'; //Enviar 0
        while (TXSTAbits.TRMT == 0);
        __delay_ms(500);
        sa = 0;
    }
    entrada = 0;
    ADCON0bits.ADON = 1; //Activamos ADC
}

