/* ----------------PROTECTO 3------------------------------------------
  Aida Patricia Toloza Bonilla - 20949
  Carne: 20949

 Estado 1 - Comunicacion UART
 Estado 2 - Contadores manuales y timer
 Estado 3 - Animación
 Estado 4 - Contador con ADC

----------------------------------------------------------------------*/

//----------------BIBLIOTECAS------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"

#define XTAL 16000000

//----------------VARIABLES GLOBALES------------------------------------------

char EntredaPropuesta = '0';
uint8_t modo = 0;
uint8_t estado_envio = 0;
uint8_t segmentos = 0;
uint8_t contadorh = 0;
int8_t contador4b = 0;
uint32_t ui32Period;
uint32_t PeriodB;
uint8_t DIP1 = 0;
uint8_t DIP2 = 0;
uint8_t secuencia1 = 0;
uint8_t secuencia2 = 0;
uint32_t lecturaADC;
uint8_t conversion = 0;
uint8_t unidades = 1;
uint8_t decenas = 0;

//----------------PROTOTIPOS DE FUNCIÓN------------------------------------------

void setup (void);
void InitUART(void);
void UART0ReadIntHandler(void);
void Timer0AIntHandler(void);
void Timer1AIntHandler(void);
void GPIOIntHandler (void);
unsigned short map(uint32_t val, uint32_t in_min, uint32_t in_max,
unsigned short out_min, unsigned short out_max);

//----------------CODIGO PRINCIPAL------------------------------------------

int main(void)
{
    setup();
    while (1)
    {
        if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&0b00000010)==0b00000010)   // DIP 1 encendido
        {
            DIP1 = 1;
        }
        else if((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&0b00000010)==0b00000000) // DIP1 apagado
        {
            DIP1 = 0;
        }
        if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&0b00000100)==0b00000100)   // DIP2 encendido
        {
            DIP2 = 1;
        }
        else if((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&0b00000100)==0b00000000) //  DIP 2 apagado
        {
            DIP2 = 0;
        }
        if ((DIP1 == 0)&&(DIP2 == 0)) //Restablecer los leds segun los modos
        {
            modo = 1;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0b10);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        }
        else if ((DIP1 == 0)&&(DIP2 == 1))
        {
            modo = 2;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0b100);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        }

        else if ((DIP1 == 1)&&(DIP2 == 0))
        {
            modo = 3;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0b1000);
        }

        else if ((DIP1 == 1)&&(DIP2 == 1))
        {
            modo = 4;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0b10);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0b100);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0b1000);
        }

        if (modo == 1)         // Si esta en el MODO 1
        {
            if (EntredaPropuesta == 'a') // Se recibe el carecter propuesto y se cambia el estado del LEDS
            {
                EntredaPropuesta = 'x';
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0b00010000);
                }
            }
            if (EntredaPropuesta == 'b')
            {
                EntredaPropuesta = 'x';
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0b00100000);
                }
            }
            if (EntredaPropuesta == 'c')
            {
                EntredaPropuesta = 'x';
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0b01000000);
                }
            }
            if (EntredaPropuesta == 'd')
            {
                EntredaPropuesta = 'x';
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0b10000000);
                }
            }
            if (EntredaPropuesta == '0') // Se recibe el número propuesto y se cambia el estado del display
            {
                EntredaPropuesta = 'x';
                segmentos = 0;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111110);
            }
            if (EntredaPropuesta == '1')
            {
                EntredaPropuesta = 'x';
                segmentos = 1;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001000);
            }
            if (EntredaPropuesta == '2')
            {
                EntredaPropuesta = 'x';
                segmentos = 2;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111101);
            }
            if (EntredaPropuesta == '3')
            {
                EntredaPropuesta = 'x';
                segmentos = 3;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101101);
            }T
            if (EntredaPropuesta == '4')
            {
                EntredaPropuesta = 'x';
                segmentos = 4;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001011);
            }
            if (EntredaPropuesta == '5')
            {
                EntredaPropuesta = 'x';
                segmentos = 5;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01100111);
            }
            if (EntredaPropuesta == '6')
            {
                EntredaPropuesta = 'x';
                segmentos = 6;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110111);
            }T
            if (EntredaPropuesta == '7')
            {
                EntredaPropuesta = 'x';
                segmentos = 7;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001100);
            }
            if (EntredaPropuesta == '8')
            {
                EntredaPropuesta = 'x';
                segmentos = 8;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111111);
            }
            if (EntredaPropuesta == '9')
            {
                EntredaPropuesta = 'x';
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001111);
            }
            else // Caso si se recibe otro caracter fuera de los propuestos
            {
                EntredaPropuesta = EntredaPropuesta;
            }
            if (estado_envio == 1) // Si se presionan algun pushbotton se envia el estado de los leds via UART a la consola de la PC
            {
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    UARTCharPut(UART0_BASE, 'LED 1:');
                    UARTCharPut(UART0_BASE, 'ON');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    UARTCharPut(UART0_BASE, 'LED 1:');
                    UARTCharPut(UART0_BASE, 'OFF');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    UARTCharPut(UART0_BASE, 'LED 2:');
                    UARTCharPut(UART0_BASE, 'ON');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    UARTCharPut(UART0_BASE, 'LED 2:');
                    UARTCharPut(UART0_BASE, 'OFF');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    UARTCharPut(UART0_BASE, 'LED 3:');
                    UARTCharPut(UART0_BASE, 'ON');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    UARTCharPut(UART0_BASE, 'LED 3:');
                    UARTCharPut(UART0_BASE, 'OFF');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    UARTCharPut(UART0_BASE, 'LED 4:');
                    UARTCharPut(UART0_BASE, 'ON');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    UARTCharPut(UART0_BASE, 'LED 4:');
                    UARTCharPut(UART0_BASE, 'OFF');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 0)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '0');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 1)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 2)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 3)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 4)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 5)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '5');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 6)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '6');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 7)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 8)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '8');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 9)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY:');
                    UARTCharPut(UART0_BASE, '9');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                estado_envio = 0;
            }
        }

        else if (modo == 2) // Si esta en el MODO 2
        {
            if (contadorh == 0)  // Mostrar valor de contador HEX en display de 7 segmentos
            {
                segmentos = 0;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111110);
            }

            if (contadorh == 1)
            {
                segmentos = 1;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001000);
            }

            if (contadorh == 2)
            {
                segmentos = 2;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111101);
            }

            if (contadorh == 3)
            {
                segmentos = 3;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101101);
            }

            if (contadorh == 4)
            {
                segmentos = 4;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001011);
            }

            if (contadorh == 5)
            {
                segmentos = 5;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01100111);
            }

            if (contadorh == 6)
            {
                segmentos = 6;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110111);
            }

            if (contadorh == 7)
            {
                segmentos = 7;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001100);
            }

            if (contadorh == 8)
            {
                segmentos = 8;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111111);
            }

            if (contadorh == 9)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001111);
            }

            if (contadorh == 10)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01011111);
            }

            if (contadorh == 11)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110011);
            }

            if (contadorh == 12)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00110110);
            }

            if (contadorh == 13)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111001);
            }

            if (contadorh == 14)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00110111);
            }

            if (contadorh == 15)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00010111);
            }
            if (EntredaPropuesta == '+') // Si se recibio un + por UART se incrementa el contador de 4 bits o hace overflow
            {
                EntredaPropuesta = 'x';
                contador4b++;
                if (contador4b == 16)
                {
                    contador4b = 0;
                }
            }
            if (EntredaPropuesta == '-') //Si se recibio un - por UART se decrementa el contador de 4 bits o hace underflow
            {
                EntredaPropuesta = 'x';
                contador4b--;
                if (contador4b == -1)
                {
                    contador4b = 15;
                }
            }
            if (contador4b == 0)  // Mostrar contador de 4 bits en los LEDs
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
            }
            else if (contador4b == 1)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x10);
            }
            else if (contador4b == 2)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x20);
            }
            else if (contador4b == 3)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x30);
            }
            else if (contador4b == 4)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x40);
            }
            else if (contador4b == 5)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x50);
            }
            else if (contador4b == 6)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x60);
            }
            else if (contador4b == 7)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x70);
            }
            else if (contador4b == 8)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x80);
            }
            else if (contador4b == 9)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x90);
            }
            else if (contador4b == 10)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xA0);
            }
            else if (contador4b == 11)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xB0);
            }
            else if (contador4b == 12)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xC0);
            }
            else if (contador4b == 13)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xD0);
            }
            else if (contador4b == 14)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xE0);
            }
            else if (contador4b == 15)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xF0);
            }

            if (estado_envio == 1)  // Enviar el estado de ambos contadores por UART
            {
                if (contadorh == 0)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '0');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 1)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 2)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 3)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 4)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 5)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '5');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 6)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '6');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 7)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 8)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '8');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 9)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, '9');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 10)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 11)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 12)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 13)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 14)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contadorh == 15)
                {
                    UARTCharPut(UART0_BASE, 'DISPLAY H:');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 0)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '0');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 1)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 2)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 3)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 4)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 5)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '5');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 6)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '6');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 7)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 8)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '8');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 9)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '9');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 10)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '10');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 11)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '11');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 12)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '12');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 13)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '13');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                if (contador4b == 14)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '14');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 15)
                {
                    UARTCharPut(UART0_BASE, 'CONTADOR:');
                    UARTCharPut(UART0_BASE, '15');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                estado_envio = 0;
            }
        }

        else if (modo == 3) // Si esta en el MODO 3
        {
            if (secuencia1 == 0)  // Se realia la animacion de display de 7 segmentos
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00010001);
            }
            if (secuencia1 == 1)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10000010);
            }
            if (secuencia1 == 2)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00000101);
            }
            if (secuencia1 == 3)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10001000);
            }
            if (secuencia1 == 4)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01000001);
            }
            if (secuencia1 == 5)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10100000);
            }

            if (secuencia2 == 0)             // Se inicia la animacion de los 4 LEDs
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b00010000);
            }
            if (secuencia2 == 2)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b00100000);
            }
            if (secuencia2 == 4)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b01000000);
            }
            if (secuencia2 == 6)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b10000000);
            }
        }

        else if (modo == 4) // Si esta en el MODO 4
        {
            ADCProcessorTrigger(ADC0_BASE, 3);  // Iniciamos conversión de ADC, se limpia la bandera de interripcion, se guarda el valor a la variable y se mapea
            while(!ADCIntStatus(ADC0_BASE, 3, false)){}
            ADCIntClear(ADC0_BASE, 3);
            ADCSequenceDataGet(ADC0_BASE, 3, &lecturaADC);
            conversion = map(lecturaADC, 0, 4096, 0, 100);
            decenas = (conversion/10); // Se realiza la división de decenas y unidades
            unidades = (conversion-(decenas*10));
            if (unidades == 0) // Se muestra las unidades en el display
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111110);
            }
            if (unidades == 1)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001000);
            }
            if (unidades == 2)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111101);
            }
            if (unidades == 3)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101101);
            }
            if (unidades == 4)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001011);
            }
            if (unidades == 5)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01100111);
            }
            if (unidades == 6)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110111);
            }
            if (unidades == 7)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001100);
            }
            if (unidades == 8)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111111);
            }
            if (unidades == 9)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001111);
            }

            if (decenas == 0) // Se muestra las decenas en 4 LEDs en binario
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
            }
            if (decenas == 1)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x10);
            }
            if (decenas == 2)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x20);
            }
            if (decenas == 3)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x30);
            }
            if (decenas == 4)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x40);
            }
            if (decenas == 5)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x50);
            }
            if (decenas == 6)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x60);
            }
            if (decenas == 7)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x70);
            }
            if (decenas == 8)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x80);
            }
            if (decenas == 9)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x90);
            }
        }
    }
}

//----------------CONFIGUACIÓN------------------------------------------

void setup (void)
{
    SysCtlClockSet(
    SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    InitUART();     // Se inicializa la comunicación UART

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);     // Se habilita el puerto F, B, C, D y E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);     // Se establecen las salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);

    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);     // Se establecen las entradas
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);     // Se hace la configuracion del ADC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);     // Se habilita los timer 0 y 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    IntEnable(INT_UART0);     // Se habilita las interrupciones
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
    UARTIntEnable(UART0_BASE, UART_INT_RX);
    IntEnable(INT_GPIOD);
    IntEnable(INT_GPIOE);
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_INT_PIN_6, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_INT_PIN_4, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_INT_PIN_4);


    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);     // Configuración de los timer a temporizadores
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ui32Period = (SysCtlClockGet());
    PeriodB = (SysCtlClockGet() / 10);
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
    TimerLoadSet(TIMER1_BASE, TIMER_A, PeriodB - 1);
    IntEnable(INT_TIMER0A);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);

    IntMasterEnable(); // Habilitar interrupciones globales

}

//----------------FUNCIONES------------------------------------------

void InitUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(
            UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
}

unsigned short map(uint32_t x, uint32_t x0, uint32_t x1,
            unsigned short y0, unsigned short y1){
    return (unsigned short)(y0+((float)(y1-y0)/(x1-x0))*(x-x0));
}

//----------------INTERRUPCIONES------------------------------------------

void UART0ReadIntHandler(void)
{
    UARTIntClear(UART0_BASE, UART_INT_RX);
    EntredaPropuesta = UARTCharGet(UART0_BASE);
    UARTCharPut(UART0_BASE, 'ENTRADA USUARIO:');
    UARTCharPut(UART0_BASE, EntredaPropuesta);
    UARTCharPut(UART0_BASE, 10);
    UARTCharPut(UART0_BASE, 13);

}

void GPIOIntHandler(void)
{
   if ((GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6)&0b01000000)==0b01000000)
   {
       GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

       if (modo == 1)
       {
           estado_envio = 1;
       }
       else if (modo == 2)
       {
           contador4b++;
           if (contador4b == 16)
           {
               contador4b = 0;
           }
       }
   }

   if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_4)&0b00010000)==0b00010000)
      {
           GPIOIntClear(GPIO_PORTE_BASE, GPIO_INT_PIN_4);

           if (modo == 1)
          {
              estado_envio = 1;
          }

          else if (modo == 2)
          {
              contador4b--;
              if (contador4b == -1)
              {
                  contador4b = 15;
              }
          }
      }
}

void Timer0AIntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if (modo == 2)
    {
        contadorh++;
        estado_envio = 1;
        if (contadorh == 0x10)
        {
            contadorh = 0;
        }
    }
}

void Timer1AIntHandler(void)
{
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    secuencia1++;
    secuencia2++;
    if (secuencia1 == 6)
    {
        secuencia1 = 0;
    }
    if (secuencia2 == 8)
    {
        secuencia2 = 0;
    }
}

