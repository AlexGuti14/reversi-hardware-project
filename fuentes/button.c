/*********************************************************************************************
* Fichero:	button.c
* Autor:
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "button.h"
#include "8led.h"
#include "44blib.h"
#include "44b.h"
#include "def.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*--- variables globales del módulo ---*/

//Guarda estado del boton
typedef estado_button;
enum estado_button {button_none, button_iz, button_dr};
void (*ptr_func_1)(estado_button)=NULL;

estado_button estadoBoton=button_none;

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de funciones ---*/
/* Rutina de interrupción */
void Eint4567_ISR(void)
{
	rINTMSK |= BIT_EINT4567;
	//push_debug(3,timer0_leer());
	/* Identificar la interrupcion (hay dos pulsadores)*/
	int which_int = rEXTINTPND;
	switch (which_int)
	{
		case 4:
			callback(button_iz);
			estadoBoton=button_iz;
			break;
		case 8:
			callback(button_dr);
			estadoBoton=button_dr;
			break;
		default:
			break;
	}
	/* Finalizar ISR */
	rEXTINTPND = 0xf;				// borra los bits en EXTINTPND
	rI_ISPC   |= BIT_EINT4567;		// borra el bit pendiente en INTPND
}

/* Inicializa el boton */
void button_iniciar()
{
	/* Configuracion del controlador de interrupciones. Estos registros están definidos en 44b.h */
	rI_ISPC    = 0x3ffffff;	// Borra INTPND escribiendo 1s en I_ISPC
	rEXTINTPND = 0xf;       // Borra EXTINTPND escribiendo 1s en el propio registro
	rINTMOD    = 0x0;		// Configura las linas como de tipo IRQ
	rINTCON    = 0x1;	    // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK    &= ~(BIT_EINT4567); // habilitamos interrupcion linea eint4567 en vector de mascaras
	/* Establece la rutina de servicio para Eint4567 */
	pISR_EINT4567 = (int) Eint4567_ISR;
	/* Configuracion del puerto G */
	rPCONG  = 0xffff;        		// Establece la funcion de los pines (EINT0-7)
	rPUPG   = 0x0;                  // Habilita el "pull up" del puerto
	rEXTINT = rEXTINT | 0x22222222;   // Configura las lineas de int. como de flanco de bajada
	/* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
	rEXTINTPND = 0xf;
	rI_ISPC    |= (BIT_EINT4567);
	D8Led_symbol(0 & 0x000f);

}

/* Devuelve el estado del boton en ese momento */
estado_button button_estado(){
	int bit6 = rPDATG & 0x40;
	int bit7 = rPDATG & 0x80;
	if(bit6==0){
		return button_iz;
	}
	else if(bit7==0){
		return button_dr;
	}
	else{
		return button_none;
	}
}

/* Devuelve el estado del boton en ese momento */
estado_button buttonEstado(){
	return estadoBoton;
}

/* Cambia el estado del boton*/
void cambiarBoton(estado_button e){
	estadoBoton=e;
}

/* Activa las interrupciones */
void activarInterrupciones(){
	rEXTINTPND = 0xf;
	rI_ISPC    |= (BIT_EINT4567);
	rINTMSK    &= ~(BIT_EINT4567);
}



