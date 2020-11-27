/*********************************************************************************************
* Fichero:	botones_antirebote.c
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

//Guarda estado del boton
typedef estado_button;
enum estado_button {button_none, button_iz, button_dr};
typedef void(function_ptr)(estado_button);
static estado_button boton_pulsado;
typedef estado;
enum estado {inicial,rsubida,encuestar,rbajada,reiniciar};
static volatile estado estadoActual=inicial;
int ahora=0;

/* inicializa el button */
void inicializar(){
	button_iniciar();
}

/* Funcion de callback que almacena el estado del boton */
void callback(estado_button boton){
	ahora=timer0_leer();
	estadoActual=encuestar;
	boton_pulsado=boton;
}

/* Inicializa la funcion callback */
void button_empezar(){
	void (*ptr_func_1)(estado_button)=&callback;
}

/* Gestiona los rebotes del boton */
void gestion(estado_button *boton){
	static int cambiar=0;
	static int esperar;
	*boton=button_none;
	switch(estadoActual){
		case encuestar:
			if((timer0_leer()-ahora)>=50000){
				if (cambiar==0){
					esperar=timer2_leer();
					cambiar=1;
				}
				if((timer2_leer()-esperar)>=20){
						cambiar=0;
						if(button_estado()!=boton_pulsado){
								estadoActual=rbajada;
								ahora=timer0_leer();
								*boton=boton_pulsado;
						}
				}
			}

		break;

		case rbajada:
			if((timer0_leer()-ahora)>=50000){
				estadoActual=reiniciar;
			}
		break;

		case reiniciar:
			estadoActual=inicial;
			activarInterrupciones();
		break;
	}
}
