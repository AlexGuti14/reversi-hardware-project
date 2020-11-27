/*********************************************************************************************
* Fichero:		led.c
* Autor:
* Descrip:		funciones de control de los LED de la placa
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "led.h"
#include "44b.h"
#include "44blib.h"

/*--- variables globales del módulo ---*/
static int led_state = 0;		/* estado del LED */
//#define USAR_EMULACION
int on1=0;
int on2=0;

/*--- codigo de las funciones públicas ---*/
void leds_on()
{
	#ifdef USAR_EMULACION
		on1=1;
		on2=1;
	#else
		Led_Display(0x3);
	#endif
}

void leds_off()
{
	#ifdef USAR_EMULACION
		on1=0;
		on2=0;
	#else
		Led_Display(0x0);
	#endif
}

void led1_on()
{
	#ifdef USAR_EMULACION
		on1=1;
	#else
		led_state = led_state | 0x1;
		Led_Display(led_state);
	#endif


}

void led1_off()
{
	#ifdef USAR_EMULACION
		on1=0;
	#else
		led_state = led_state & 0xfe;
		Led_Display(led_state);
	#endif

}

void led2_on()
{
	#ifdef USAR_EMULACION
		on2=1;
	#else
		led_state = led_state | 0x2;
			Led_Display(led_state);
	#endif

}

void led2_off()
{
	#ifdef USAR_EMULACION
		on2=0;
	#else
		led_state = led_state & 0xfd;
		Led_Display(led_state);
	#endif

}

void leds_switch()
{
	#ifdef USAR_EMULACION
		if(on1==1){ on1=0; on2=1; }
		else{ on1=1; on2=0;}

	#else
		led_state ^= 0x03;
		Led_Display(led_state);
	#endif

}

void Led_Display(int LedStatus)
{
	led_state = LedStatus;

	if ((LedStatus & 0x01) == 0x01)
		rPDATB = rPDATB & 0x5ff; /* poner a 0 el bit 9 del puerto B */
	else
		rPDATB = rPDATB | 0x200; /* poner a 1 el bit 9 del puerto B */

	if ((LedStatus & 0x02) == 0x02)
		rPDATB = rPDATB & 0x3ff; /* poner a 0 el bit 10 del puerto B */
	else
		rPDATB = rPDATB | 0x400; /* poner a 1 el bit 10 del puerto B */
}
