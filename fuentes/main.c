/*********************************************************************************************
* Fichero:	main.c
* Autor:	Daniel Cay (741066) y Alejandro Gutiérrez (735089)
* Descrip:	punto de entrada de C
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "44blib.h"
#include "44b.h"
#include "tp.h"
#include "lcd.h"
#include "Bmp.h"
#include <stdio.h>

//Guarda estado de la pantalla
typedef estado_pantalla;
enum estado_pantalla {pulsada, no_pulsada};
//Guarda estado del boton
typedef estado_button;
enum estado_button {button_none, button_iz, button_dr};
estado_button boton=button_none;
//Guarda estado del programa
typedef estado_programa;
enum estado_programa {inicio, elegirMovimiento, esperar_2s, reversi};
estado_programa estadoPrograma=inicio;

int primera=0;
int fin=0;

int xpos=0;
int ypos=0; //posicion ficha gris

int tiempoParpadeo=0;
int tiempo2s=0;
int tiempoPatron=0;
int cambio=1;
int estadoPartida=0;

volatile int tX=0;			//Indica la posición X de la pulsación de la pantalla
volatile int tY=0;			//Indica la posición Y de la pulsación de la pantalla
volatile int X_MIN;		//Valor mínimo de la X en el panel táctil para el tablero
volatile int Y_MIN;		//Valor mínimo de la Y en el panel táctil para el tablero
volatile int X_MAX;		//Valor máximo de la X en el panel táctil para el tablero
volatile int Y_MAX;		//Valor máximo de la Y en el panel táctil para el tablero

/*--- codigo de funciones ---*/

/* Funcion de calibracion de la pantalla */
void calibrar(){
	Lcd_Clr();				//Limpiamos la pantalla
	volatile int minX=0;	//Valor mínimo de la X en el panel táctil para el tablero
	volatile int minY=0;	//Valor mínimo de la Y en el panel táctil para el tablero
	volatile int maxX=0;	//Valor máximo de la X en el panel táctil para el tablero
	volatile int maxY=0;	//Valor máximo de la Y en el panel táctil para el tablero
	int i;					//Variable que utilizará para comprobar n veces cada esquina del tablero
	int tiempo;
	cambiarEstado(no_pulsada);
	for (i=0; i<1; i++){								//Hacemos 5 medidas
		Lcd_Clr();
		Lcd_DspAscII8x16(25,25,BLACK,"Superior izquierda");		//Indica la esquina a pulsar
		Lcd_DspAscII8x16(125,125,BLACK,"CALIBRAR");
		Lcd_Dma_Trans();
		cambiarEstado(no_pulsada);
		while(estadoActual()==no_pulsada){}						//Esperamos que se pulse la pantalla tactil
		cambiarEstado(no_pulsada);
		minX+=tX;
		maxY+=tY;
		tiempo=timer2_leer();
		while(timer2_leer()-tiempo<=1000000){}

		Lcd_Clr();
		Lcd_DspAscII8x16(150,25,BLACK,"Superior derecha");	//Indica la esquina a pulsar
		Lcd_DspAscII8x16(125,125,BLACK,"CALIBRAR");
		Lcd_Dma_Trans();
		cambiarEstado(no_pulsada);
		while(estadoActual()==no_pulsada){}							//Esperamos que se pulse la pantalla tactil
		cambiarEstado(no_pulsada);

		maxX+=tX;
		maxY+=tY;

		tiempo=timer2_leer();
		while(timer2_leer()-tiempo<=1000000){}

		Lcd_Clr();
		Lcd_DspAscII8x16(25,200,BLACK,"Inferior izquierda");		//Indica la esquina a pulsar
		Lcd_DspAscII8x16(125,125,BLACK,"CALIBRAR");
		Lcd_Dma_Trans();
		cambiarEstado(no_pulsada);
		while(estadoActual()==no_pulsada){}							//Esperamos que se pulse la pantalla tactil
		cambiarEstado(no_pulsada);

		minX+=tX;
		minY+=tY;

		tiempo=timer2_leer();
		while(timer2_leer()-tiempo<=1000000){}


		Lcd_Clr();
		Lcd_DspAscII8x16(150,200,BLACK,"Inferior derecha");			//Indica la esquina a pulsar
		Lcd_DspAscII8x16(125,125,BLACK,"CALIBRAR");
		Lcd_Dma_Trans();
		cambiarEstado(no_pulsada);
		while(estadoActual()==no_pulsada){}							//Esperamos que se pulse la pantalla tactil
		cambiarEstado(no_pulsada);

		maxX+=tX;
		minY+=tY;

		tiempo=timer2_leer();
		while(timer2_leer()-tiempo<=1000000){}

	}

	//Dado que se han hecho, en total, 10 mediciones de cada punto, dividimos entre 10 para obtener la media de cada punto
	X_MIN= minX/2;
	Y_MIN= minY/2;
	X_MAX= maxX/2;
	Y_MAX= maxY/2;
}

/* Devuelve si la pantalla ha sido pulsada en el centro */
int pulsaCentro(){
	if(X_MIN+tX>X_MIN+50 && X_MAX-tX<X_MAX-50 && Y_MIN+tY>Y_MIN+50 && Y_MAX-tY<Y_MAX-50){
		return 1;
	}
	else{
		return 0;
	}
}



void Main(void)
{
	/* Inicializa controladores */
	sys_init();         			// Inicializacion de la placa, interrupciones y puertos
	timer_init();	    			// Inicializacion del temporizador 0
	D8Led_init();       			// inicializamos el 8led
	inicializar();					// inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led
	button_empezar();
	exception_inicializar();		// Inicializacion de excepciones
	timer2_inicializar();			// Inicializacion del temporizador 2
	leds_off();
	limpiarPila();
	reversi_main();
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

	TS_init();

	timer2_empezar();

	//Pasamos a modo USUARIO
	asm(".equ MODEMASK, 0x1f");
	asm(".equ USERMODE, 0x10");
	asm("mrs r0,cpsr");
	asm("bic r0,r0,#MODEMASK");
	asm("orr r1,r0,#USERMODE");
	asm("msr cpsr_cxsf,r1");
	asm("ldr sp, =0xc7ff000");

	calibrar();

	while(1){
		switch(estadoPrograma){
			//Estado inicial
			case inicio:
				//Se muestra el menu de inicio
				if(primera==0){
					Lcd_Clr();
					cambiarBoton(button_none);
					cambiarEstado(no_pulsada);
					mostrarInicio();
					primera=1;
				}
				//Se cambia de estado si es pulsada la pantalla o un boton
				if(estadoActual()==pulsada || buttonEstado()==button_iz || buttonEstado()==button_dr){
					Lcd_Clr();
					cambiarBoton(button_none);
					cambiarEstado(no_pulsada);
					timer2_empezar();
					mostrarTiempo(timer2_leer());
					mostrarTiempoPatron(tiempoPatron);
					mostrarFila(xpos);
					mostrarColumna(ypos);
					mostrarTablero();
					rellenarTablero();
					PulseJugar();
					colocarPosibleFicha(xpos,ypos);
					tiempoParpadeo=timer2_leer();
					estadoPrograma=elegirMovimiento;
				}
				break;
			//Estado elegir movimiento de nuestra ficha
			case elegirMovimiento:
				cambiarEstado(no_pulsada);
				cambiarBoton(button_none);
				//Si pulsa en el centro se cambia a estado de esperar2s
				if(estadoActual()==pulsada){
					cambiarEstado(no_pulsada);
					if(pulsaCentro()==1) {
						estadoPrograma=esperar_2s;
						Lcd_Clr();
						cambiarBoton(button_none);
						mostrarTiempo(timer2_leer());
						mostrarTiempoPatron(tiempoPatron);
						mostrarFila(xpos);
						mostrarColumna(ypos);
						mostrarTablero();
						rellenarTablero();
						PulseCancelar();
						colocarPosibleFicha(xpos,ypos);
						tiempo2s=timer2_leer();
						tiempoParpadeo=timer2_leer();
					}
				}
				//Se gestiona el boton pulsado, eliminando los rebotes
				gestion(&boton);
				//Si el boton es derecho, se mueve verticalmente
				if(boton==button_dr){
					Lcd_Clr();
					cambiarBoton(button_none);
					mostrarTiempo(timer2_leer());
					mostrarTiempoPatron(tiempoPatron);
					mostrarFila(xpos);
					mostrarColumna(ypos);
					mostrarTablero();
					PulseJugar();
					rellenarTablero();
					if(xpos==7) {
						xpos=0;
					}
					else {
						xpos+=1;
					}
					colocarPosibleFicha(xpos,ypos);
				}
				//Si el boton es izquierdo, se mueve horizontalmente
				else if(boton==button_iz){
					Lcd_Clr();
					cambiarBoton(button_none);
					mostrarTiempo(timer2_leer());
					mostrarTiempoPatron(tiempoPatron);
					mostrarFila(xpos);
					mostrarColumna(ypos);
					mostrarTablero();
					PulseJugar();
					rellenarTablero();
					if(ypos==7) {
						ypos=0;
					}
					else {
						ypos+=1;
					}
					colocarPosibleFicha(xpos,ypos);
				}
				//Cada medio segundo, se produce el parpadeo de la ficha
				if(timer2_leer()-tiempoParpadeo>=500000) {
					LcdClrRect(220,15,315,100,WHITE);
					mostrarTiempo(timer2_leer());
					mostrarTiempoPatron(tiempoPatron);
					mostrarFila(xpos);
					mostrarColumna(ypos);
					if (cambio==0){
						cambio=1;//Si el color es blanco
						parpadeo(xpos,ypos,cambio);	//Ponemos la casilla negra
					}
					else{
						cambio=0;						//Color = blanco
						parpadeo(xpos,ypos,cambio);	//Ponemos la casilla blanca
					}
					tiempoParpadeo=timer2_leer();
				}
				break;
			//Estado esperar 2 segundos para aceptar o cancelar movimiento
			case esperar_2s:
				cambiarEstado(no_pulsada);
				//Si pasan los 2 segundos se cambia de estado para realizar el movimiento
				if(timer2_leer()-tiempo2s>=2000000) {
					estadoPrograma=reversi;
				}
				cambiarEstado(no_pulsada);
				//Si es pulsada la pantalla o un boton se vuelve al estado anterior para seguir realizando el movimiento
				if(estadoActual()==pulsada || buttonEstado()==button_iz || buttonEstado()==button_dr) {
					estadoPrograma=elegirMovimiento;
					Lcd_Clr();
					cambiarBoton(button_none);
					cambiarEstado(no_pulsada);
					mostrarTiempo(timer2_leer());
					mostrarTiempoPatron(tiempoPatron);
					mostrarFila(xpos);
					mostrarColumna(ypos);
					mostrarTablero();
					PulseJugar();
					rellenarTablero();
					colocarPosibleFicha(xpos,ypos);
					tiempoParpadeo=timer2_leer();
				}
				//Cada medio segundo, se produce el parpadeo de la ficha
				if(timer2_leer()-tiempoParpadeo>=500000) {
					LcdClrRect(220,15,315,100,WHITE);
					mostrarTiempo(timer2_leer());
					mostrarTiempoPatron(tiempoPatron);
					mostrarFila(xpos);
					mostrarColumna(ypos);
					if (cambio==0){
						cambio=1;//Si el color es blanco
						parpadeo(xpos,ypos,cambio);	//Ponemos la casilla negra
					}
					else{
						cambio=0;						//Color = blanco
						parpadeo(xpos,ypos,cambio);	//Ponemos la casilla blanca
					}
					tiempoParpadeo=timer2_leer();
				}
				break;
			//Estado reversi, se realiza el movimiento seleccionado anteriormente
			case reversi:
				//Si no puede colocar, se pasa turno
				if(fichaPosicionCandidatas(xpos,ypos)!=1 || puedoColocar(xpos,ypos)!=1){
					xpos=8;
					ypos=8;
				}
				estadoPartida=reversi8(xpos,ypos,&tiempoPatron,&fin);
				//Si no se ha terminado todavia se vuelve al estado elegirMovimiento
				if(fin==0){
						Lcd_Clr();
						mostrarTiempo(timer2_leer());
						mostrarTiempoPatron(tiempoPatron);
						mostrarFila(xpos);
						mostrarColumna(ypos);
						mostrarTablero();
						PulseJugar();
						rellenarTablero();
						estadoPrograma=elegirMovimiento;
						xpos=0;
						ypos=0;
						colocarPosibleFicha(xpos,ypos);
				}
				//En caso de fin, se muestra por pantalla el resultado pudiendo empezar otra partida
				else{
					estadoPrograma=inicio;
					Lcd_Clr();
					if(estadoPartida==1){
						BlancasGanan();
					}
					else if(estadoPartida==2){
						NegrasGanan();
					}
					else{
						Empate();
					}
					reversi_main();
					xpos=0;
					ypos=0;
				}
				break;
		}
	}
}
