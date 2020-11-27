/*********************************************************************************************
* Fichero:		timer2.c
* Autor:		Daniel Cay (741066) y Alejandro Gutiérrez (735089)
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
//#include "timer2.h"
#include "44b.h"
#include "44blib.h"

/*--- variables globales ---*/
volatile static int timer2_num_int=0;

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer2_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer2_ISR(void)
{
	 timer2_num_int++;

	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER2; // BIT_TIMER2 está definido en 44b.h y pone un uno en el bit 11 que correponde al Timer2
}

void timer2_inicializar(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD &= 0x3FFF7FF; // Configura las linas como de tipo IRQ. bit 11 a 0
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK &= ~(BIT_TIMER2); // habilitamos en vector de mascaras de interrupcion el Timer2 (bits 26 y 11, BIT_GLOBAL y BIT_TIMER2 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER2 */
	pISR_TIMER2 = (unsigned) timer2_ISR;

	/* Configura el Timer2 */
	//rTCFG0 = 1; // ajusta el preescalado
	rTCFG0 &= (0xFFFF00FF);	// ajusta el preescalado
	rTCFG1 &= (0xFFFF0FF); // selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2. bit 11-8 a 0
	rTCNTB2 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB2 = 0;// valor de comparación

}

/* Permite al timer2 empezar a contar */
void timer2_empezar(void) {
	rTCON |= 0x02000;	//update=manual (bit 13) a 1
	rTCON |= 0x09000;	//bit start (bit 12) con auto-reload (bit 15) a 1
	rTCON &= ~(0x2000); //update=manual (bit 13) a 0
	timer2_num_int=0;
}

/* Devuelve el tiempo transcurrido en el timer2 en milisegundos */
unsigned int timer2_leer(void){
  if (rTCNTO2==65535){
	  return ((timer2_num_int-1)*rTCNTB2+(rTCNTB2-rTCNTO2))/32;
  }
  else{
	  return (timer2_num_int*rTCNTB2+(rTCNTB2-rTCNTO2))/32;
  }
}

/* Paraliza el timer2 */
unsigned int timer2_parar(void){
	rTCON &= ~(0x9000);		//bit start (bit 12) con auto-reload (bit 15) a 0
	return timer2_leer();
}




