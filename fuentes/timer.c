/*********************************************************************************************
* Fichero:		timer.c
* Autor:
* Descrip:		funciones de control del timer0 del s3c44b0x
* Version:
*********************************************************************************************/

//funcion que te devuelve los tics en timer0 y funcion en button que hace una espera activa comprobando si los tics llegan al número establecido

/*--- ficheros de cabecera ---*/
#include "timer.h"
#include "44b.h"
#include "44blib.h"

/*--- variables globales ---*/
int num_inter=0;
int encendido=0;
volatile static int timer0_num_int=0;


/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer_ISR(void){
	timer0_num_int++;
	num_inter++;
		if(encendido==0){
			led1_on();
			encendido=1;
		}
		else{
			led1_off();
			encendido=0;
		}

	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER0; // BIT_TIMER0 está definido en 44b.h y pone un uno en el bit 13 que correponde al Timer0
}

void timer_init(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK &= ~(BIT_TIMER0); // habilitamos en vector de mascaras de interrupcion el Timer0 (bits 26 y 13, BIT_GLOBAL y BIT_TIMER0 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER0 */
	pISR_TIMER0 = (unsigned) timer_ISR;

	/* Configura el Timer0 */
	rTCFG0 |= (0xFF);	 // ajusta el preescalado
	rTCFG1 &= (0x0);  // selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
	rTCNTB0 = 31250; // valor inicial de cuenta (la cuenta es descendente)
	rTCMPB0 = 0;// valor de comparación
	/* establecer update=manual (bit 1) + inverter=on (¿? será inverter off un cero en el bit 2 pone el inverter en off)*/
	rTCON |= 0x2;
	rTCON &= ~(0x2); //update=manual (bit 13) a 0
	/* iniciar timer (bit 0) con auto-reload (bit 3)*/
	rTCON |= 0x09;
}

int timer0_leer(){
	return (timer0_num_int*rTCNTB0+(rTCNTB0-rTCNTO0));
}
