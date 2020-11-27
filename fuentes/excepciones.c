/*********************************************************************************************
* Fichero:		excepciones.c
* Autor:		Daniel Cay (741066) y Alejandro Gutiérrez (735089)
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "44blib.h"
#include "44b.h"
/*--- variables globales ---*/
static int ERRORES=0;
int resultado, instruccion;

void ISR_Exception(void) __attribute__ ((interrupt ("SWI")));
void ISR_Exception(void) __attribute__ ((interrupt ("UNDEF")));
void ISR_Exception(void) __attribute__ ((interrupt ("ABORT")));
/* Rutina de interrupción */
void ISR_Exception(void)
{
		ERRORES++;
		asm("mrs %[resultado], cpsr" : [resultado]  "=r" (resultado) : );
		asm("sub %[instruccion], r14, #8" : [instruccion] "=r" (instruccion) : );
		resultado &= 0x1F;
		if(resultado==23){ //Data abort
			led1_on();
		}
		else if(resultado==19){	//SWI
			led2_on();
		}
		else if(resultado==27){	//Undef
			leds_on();
		}
		while(1);
}

/* Inicializa las excepciones */
void exception_inicializar(void)
{
	/* Establece la rutina de servicio para las excepciones */
	pISR_SWI = (unsigned) ISR_Exception;
	pISR_DABORT = (unsigned) ISR_Exception;
	pISR_UNDEF = (unsigned) ISR_Exception;

}







