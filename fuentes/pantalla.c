/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "44blib.h"
#include "44b.h"
#include "tp.h"
#include "def.h"
#include "lcd.h"
#include "Bmp.h"

//Guarda estado de la pantalla
typedef estado_pantalla;
enum estado_pantalla {pulsada, no_pulsada};

/* Muestra el inicio del juego en el LCD */
void mostrarInicio(){
	Lcd_DspAscII8x16(25,25,BLACK,"Toque la pantalla para jugar");
	Lcd_DspAscII8x16(25,40,BLACK,"----------------------------");
	Lcd_DspAscII8x16(25,55,BLACK,"	     INSTRUCCIONES");
	Lcd_DspAscII8x16(25,70,BLACK,"----------------------------");
	Lcd_DspAscII8x16(25,85,BLACK,"Boton izq: ->");
	Lcd_DspAscII8x16(25,100,BLACK,"Boton dr: |");
	Lcd_DspAscII8x16(25,112,BLACK,"          v");
	Lcd_DspAscII8x16(25,130,BLACK,"Pulsar pantalla: Fijar ficha");
	Lcd_Dma_Trans();
}

/* Muestra la fila en el LCD */
void mostrarFila(int xpos){
	xpos+=1;
	char xpos2=xpos + '0';
	Lcd_DspAscII8x16(220,60,BLACK,"Fila:");
	Lcd_DspAscII8x16(260,60,BLACK,&xpos2);
	Lcd_Dma_Trans();
}

/* Muestra la columna en el LCD */
void mostrarColumna(int ypos){
	ypos+=1;
	char ypos2=ypos + '0';
	Lcd_DspAscII8x16(220,80,BLACK,"Columna:");
	Lcd_DspAscII8x16(285,80,BLACK,&ypos2);
	Lcd_Dma_Trans();
}

void auxMT(char t, int pos){
	Lcd_DspAscII8x16(220,20,BLACK,"Time(s):");
	Lcd_DspAscII8x16(pos,20,BLACK,&t);
}

/* Muestra el tiempo real del juego en el LCD */
void mostrarTiempo(int tiempo){
	tiempo=tiempo/1000000;
	int t=0;
	int auxT;
	char tiempo2;
	int pos=290;
	int i=0;
	int vec[10];
	while (tiempo!=0){
		auxT=tiempo%10;
		tiempo=tiempo/10;
		tiempo2=auxT + '0';
		vec[i]=tiempo2;
		i++;
	}
	while(i>0){
		auxMT(vec[i-1],pos);
		pos+=8;
		i--;
	}
	Lcd_Dma_Trans();
}

void auxPatron(char t, int pos){
	Lcd_DspAscII8x16(220,40,BLACK,"P_V(ms):");
	Lcd_DspAscII8x16(pos,40,BLACK,&t);
}

/* Muestra el tiempo de la funcion patrón volteo en el LCD */
void mostrarTiempoPatron(int tiempo){
	if(tiempo==0){
		char tiempo2=tiempo + '0';
		auxPatron(tiempo2,285);
	}
	else{
		int t=0;
		int auxT;
		char tiempo2;
		int pos=290;
		int i=0;
		int vec[10];
		while (tiempo!=0){
			auxT=tiempo%10;
			tiempo=tiempo/10;
			tiempo2=auxT + '0';
			vec[i]=tiempo2;
			i++;
		}
		while(i>0){
			auxPatron(vec[i-1],pos);
			pos+=8;
			i--;
		}
	}
	Lcd_Dma_Trans();
}

/* Muestra el tablero vacio en el LCD */
void mostrarTablero(){
	Lcd_DspAscII6x8(205,0,BLACK,"1");
	Lcd_DspAscII6x8(205,25,BLACK,"2");
	Lcd_DspAscII6x8(205,50,BLACK,"3");
	Lcd_DspAscII6x8(205,75,BLACK,"4");
	Lcd_DspAscII6x8(205,100,BLACK,"5");
	Lcd_DspAscII6x8(205,125,BLACK,"6");
	Lcd_DspAscII6x8(205,150,BLACK,"7");
	Lcd_DspAscII6x8(205,175,BLACK,"8");

	Lcd_DspAscII6x8(0,205,BLACK,"1");
	Lcd_DspAscII6x8(25,205,BLACK,"2");
	Lcd_DspAscII6x8(50,205,BLACK,"3");
	Lcd_DspAscII6x8(75,205,BLACK,"4");
	Lcd_DspAscII6x8(100,205,BLACK,"5");
	Lcd_DspAscII6x8(125,205,BLACK,"6");
	Lcd_DspAscII6x8(150,205,BLACK,"7");
	Lcd_DspAscII6x8(175,205,BLACK,"8");
	/* draw rectangle pattern */
	int x=0;
	while(x<200){
		Lcd_Draw_Box(0,x,25,x+25,15);
		Lcd_Draw_Box(25,x,50,x+25,15);
		Lcd_Draw_Box(50,x,75,x+25,15);
		Lcd_Draw_Box(75,x,100,x+25,15);
		Lcd_Draw_Box(100,x,125,x+25,15);
		Lcd_Draw_Box(125,x,150,x+25,15);
		Lcd_Draw_Box(150,x,175,x+25,15);
		Lcd_Draw_Box(175,x,200,x+25,15);
		x=x+25;
	}
	Lcd_Dma_Trans();
}

/* Muestra mensaje "Pulse para jugar" en el LCD */
void PulseJugar() {
	Lcd_DspAscII8x16(50,215,BLACK,"Pulse para jugar");
	Lcd_Dma_Trans();
}

/* Muestra mensaje "Pulse para cancelar" en el LCD */
void PulseCancelar() {
	Lcd_DspAscII8x16(50,215,BLACK,"Pulse para cancelar");
	Lcd_Dma_Trans();
}

/* Muestra las fichas existentes en el LCD */
void rellenarTablero(){
	char ficha;
	int i=0; int j=0;
	for(i=0;i<8;i++) {
		for(j=0;j<8;j++) {
			ficha=fichaPosicion(i,j);
			if(ficha==2) {
				LcdClrRect(j*25+5,i*25+5,(j+1)*25-5,(i+1)*25-5,BLACK);		//Escribimos un cuadrado negro
			}
			else if(ficha==1){
				Lcd_Draw_Box(j*25+5,i*25+5,(j+1)*25-5,(i+1)*25-5,BLACK);		//Escribimos un cuadrado gris
			}
		}
	}
	Lcd_Dma_Trans();
}

/* Muestra la ficha a colocar en ese momento en el LCD */
void colocarPosibleFicha(int x, int y) {
	LcdClrRect(y*25+5,x*25+5,(y+1)*25-5,(x+1)*25-5,0xb);
	Lcd_Dma_Trans();
}

/* Muestra la ficha a colocar cambiando de color en el LCD */
void parpadeo(int x, int y, int cambio) {
	if(cambio==0) {
		LcdClrRect(y*25+5,x*25+5,(y+1)*25-5,(x+1)*25-5,WHITE);
	}
	else {
		LcdClrRect(y*25+5,x*25+5,(y+1)*25-5,(x+1)*25-5,BLACK);
	}
	Lcd_Dma_Trans();
}

/* Muestra victoria de fichas blancas e inicio en el LCD */
void BlancasGanan() {
	Lcd_DspAscII8x16(25,0,BLACK,"[BLANCAS HAN GANADO!!]");
	Lcd_DspAscII8x16(25,25,BLACK,"Toque la pantalla para jugar");
	Lcd_DspAscII8x16(25,40,BLACK,"----------------------------");
	Lcd_DspAscII8x16(25,55,BLACK,"	     INSTRUCCIONES");
	Lcd_DspAscII8x16(25,70,BLACK,"----------------------------");
	Lcd_DspAscII8x16(25,85,BLACK,"Boton izq: ->");
	Lcd_DspAscII8x16(25,100,BLACK,"Boton dr: |");
	Lcd_DspAscII8x16(25,112,BLACK,"          v");
	Lcd_DspAscII8x16(25,130,BLACK,"Pulsar pantalla: Fijar ficha");
	Lcd_Dma_Trans();
}

/* Muestra victoria de fichas negras e inicio en el LCD */
void NegrasGanan() {
	Lcd_DspAscII8x16(25,0,BLACK,"[NEGRAS HAN GANADO!!]");
	Lcd_DspAscII8x16(25,25,BLACK,"Toque la pantalla para jugar");
	Lcd_DspAscII8x16(25,40,BLACK,"----------------------------");
	Lcd_DspAscII8x16(25,55,BLACK,"	     INSTRUCCIONES");
	Lcd_DspAscII8x16(25,70,BLACK,"----------------------------");
	Lcd_DspAscII8x16(25,85,BLACK,"Boton izq: ->");
	Lcd_DspAscII8x16(25,100,BLACK,"Boton dr: |");
	Lcd_DspAscII8x16(25,112,BLACK,"          v");
	Lcd_DspAscII8x16(25,130,BLACK,"Pulsar pantalla: Fijar ficha");
	Lcd_Dma_Trans();
}

/* Muestra empate e inicio en el LCD */
void Empate() {
	Lcd_DspAscII8x16(25,0,BLACK,"[Empate!!]");
	Lcd_DspAscII8x16(25,25,BLACK,"Toque la pantalla para jugar");
	Lcd_DspAscII8x16(25,40,BLACK,"----------------------------");
	Lcd_DspAscII8x16(25,55,BLACK,"	     INSTRUCCIONES");
	Lcd_DspAscII8x16(25,70,BLACK,"----------------------------");
	Lcd_DspAscII8x16(25,85,BLACK,"Boton izq: ->");
	Lcd_DspAscII8x16(25,100,BLACK,"Boton dr: |");
	Lcd_DspAscII8x16(25,112,BLACK,"          v");
	Lcd_DspAscII8x16(25,130,BLACK,"Pulsar pantalla: Fijar ficha");
	Lcd_Dma_Trans();
}
