//Autores: Daniel Cay (741066) y Alejandro Gutiérrez (735089)


#include "stdint.h";

// Tamaño del tablero
enum { DIM=8 };

// Valores que puede devolver la función patron_volteo())
enum {
	NO_HAY_PATRON = 0,
  PATRON_ENCONTRADO = 1
};

// Estados de las casillas del tablero
enum {
CASILLA_VACIA = 0,
FICHA_BLANCA = 1,
FICHA_NEGRA = 2
};

typedef estado_button;
enum estado_button {button_none, button_iz, button_dr};

// candidatas: indica las posiciones a explorar
// Se usa para no explorar todo el tablero innecesariamente
// Sus posibles valores son NO, SI, CASILLA_OCUPADA
const signed char  NO              = 0;
const signed char  SI              = 1;
const signed char  CASILLA_OCUPADA = 2;

extern int patron_volteo_arm_c(signed char tablero[][DIM], int *longitud, signed char FA, signed char CA, signed char SF, signed char SC, signed char color);
extern int patron_volteo_arm_arm(signed char tablero[][DIM], int *longitud, signed char FA, signed char CA, signed char SF, signed char SC, signed char color);

#define limite 0xC7FF000-8*10
#define inicio 0xC7FF000

void limpiarPila(){
	uint32_t *ini=0xc7FF000;
	while(ini!=limite){
		*ini=0;
		ini=ini-1;
	}
}

void push_debug(uint8_t ID_evento, uint32_t auxData){
	int time=timer2_leer();
	int resultado,ID_evento2;
	static uint32_t *dirM=0xc7FF000;
	
	ID_evento2 = ID_evento << 24;
	auxData = auxData & 0x00FFFFFF;
	resultado=ID_evento2+auxData;
	if(dirM==limite){
		dirM=inicio;
	}
	dirM=dirM-1;
	*dirM=resultado;
	dirM=dirM-1;
	*dirM=time;
}



/////////////////////////////////////////////////////////////////////////////
// TABLAS AUXILIARES
// declaramos las siguientes tablas como globales para que sean más fáciles visualizarlas en el simulador
// __attribute__ ((aligned (8))): specifies a minimum alignment for the variable or structure field, measured in bytes, in this case 8 bytes

static const signed char __attribute__ ((aligned (8))) tabla_valor[DIM][DIM] =
{
    {8,2,7,3,3,7,2,8},
    {2,1,4,4,4,4,1,2},
    {7,4,6,5,5,6,4,7},
    {3,4,5,0,0,5,4,3},
    {3,4,5,0,0,5,4,3},
    {7,4,6,5,5,6,4,7},
    {2,1,4,4,4,4,1,2},
    {8,2,7,3,3,7,2,8}
};


// Tabla de direcciones. Contiene los desplazamientos de las 8 direcciones posibles
const signed char vSF[DIM] = {-1,-1, 0, 1, 1, 1, 0,-1};
const signed char vSC[DIM] = { 0, 1, 1, 1, 0,-1,-1,-1};

//////////////////////////////////////////////////////////////////////////////////////
// Variables globales que no deberían serlo
// tablero, fila, columna y ready son varibles que se deberían definir como locales dentro de reversi8.
// Sin embargo, las hemos definido como globales para que sea más fácil visualizar el tablero y las variables en la memoria
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Tablero sin inicializar
////////////////////////////////////////////////////////////////////
signed char __attribute__ ((aligned (8))) tablero[DIM][DIM] = {
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA}
	    };

  ////////////////////////////////////////////////////////////////////
     // VARIABLES PARA INTERACCIONAR CON LA ENTRADA SALIDA
     // Pregunta: ¿hay que hacer algo con ellas para que esto funcione bien?
     // (por ejemplo añadir alguna palabra clave para garantizar que la sincronización a través de esa variable funcione)
  int tabla_tiempo[100];
  int tabla_tiempo_ARM_C[100];
  int tabla_tiempo_ARM_ARM[100];
  int i=0;

  signed char __attribute__ ((aligned (8))) candidatas[DIM][DIM] =
  		    {
  		        {0,0,0,0,0,0,0,0},
  		        {0,0,0,0,0,0,0,0},
  		        {0,0,0,0,0,0,0,0},
  		        {0,0,0,0,0,0,0,0},
  		        {0,0,0,0,0,0,0,0},
  		        {0,0,0,0,0,0,0,0},
  		        {0,0,0,0,0,0,0,0},
  		        {0,0,0,0,0,0,0,0}
  		    };

// extern int patron_volteo(signed char tablero[][8], int *longitud,signed char f, signed char c, signed char SF, signed char SC, signed char color);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 0 indica CASILLA_VACIA, 1 indica FICHA_BLANCA y 2 indica FICHA_NEGRA
// pone el tablero a cero y luego coloca las fichas centrales.
void init_table(signed char tablero[][DIM], signed char candidatas[][DIM])
{
    int i, j;

    for (i = 0; i < DIM; i++)
    {
        for (j = 0; j < DIM; j++)
            tablero[i][j] = CASILLA_VACIA;
    }
#if 0
    for (i = 3; i < 5; ++i) {
	for(j = 3; j < 5; ++j) {
	    tablero[i][j] = i == j ? FICHA_BLANCA : FICHA_NEGRA;
	}
    }

    for (i = 2; i < 6; ++i) {
	for (j = 2; j < 6; ++j) {
	    if((i>=3) && (i < 5) && (j>=3) && (j<5)) {
		candidatas[i][j] = CASILLA_OCUPADA;
	    } else {
		candidatas[i][j] = SI; //CASILLA_LIBRE;
	    }
	}
    }
#endif
    // arriba hay versión alternativa
    tablero[3][3] = FICHA_BLANCA;
    tablero[4][4] = FICHA_BLANCA;
    tablero[3][4] = FICHA_NEGRA;
    tablero[4][3] = FICHA_NEGRA;

    candidatas[3][3] = CASILLA_OCUPADA;
    candidatas[4][4] = CASILLA_OCUPADA;
    candidatas[3][4] = CASILLA_OCUPADA;
    candidatas[4][3] = CASILLA_OCUPADA;

    // casillas a explorar:
    candidatas[2][2] = SI;
    candidatas[2][3] = SI;
    candidatas[2][4] = SI;
    candidatas[2][5] = SI;
    candidatas[3][2] = SI;
    candidatas[3][5] = SI;
    candidatas[4][2] = SI;
    candidatas[4][5] = SI;
    candidatas[5][2] = SI;
    candidatas[5][3] = SI;
    candidatas[5][4] = SI;
    candidatas[5][5] = SI;
}

////////////////////////////////////////////////////////////////////////////////
// Espera a que ready valga 1.
// CUIDADO: si el compilador coloca esta variable en un registro, no funcionará.
// Hay que definirla como "volatile" para forzar a que antes de cada uso la cargue de memoria

void esperar_mov(signed char *ready)
{
    while (*ready == 0) {};  // bucle de espera de respuestas hasta que el se modifique el valor de ready (hay que hacerlo manualmente)

    *ready = 0;  //una vez que pasemos el bucle volvemos a fijar ready a 0;
}

////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// IMPORTANTE: AL SUSTITUIR FICHA_VALIDA() Y PATRON_VOLTEO()
// POR RUTINAS EN ENSAMBLADOR HAY QUE RESPETAR LA MODULARIDAD.
// DEBEN SEGUIR SIENDO LLAMADAS A FUNCIONES Y DEBEN CUMPLIR CON EL ATPCS
// (VER TRANSPARENCIAS Y MATERIAL DE PRACTICAS):
//  - DEBEN PASAR LOS PARAMETROS POR LOS REGISTROS CORRESPONDIENTES
//  - GUARDAR EN PILA SOLO LOS REGISTROS QUE TOCAN
//  - CREAR UN MARCO DE PILA TAL Y COMO MUESTRAN LAS TRANSPARENCIAS
//    DE LA ASIGNATURA (CON EL PC, FP, LR,....)
//  - EN EL CASO DE LAS VARIABLES LOCALES, SOLO HAY QUE APILARLAS
//    SI NO SE PUEDEN COLOCAR EN UN REGISTRO.
//    SI SE COLOCAN EN UN REGISTRO NO HACE FALTA
//    NI GUARDARLAS EN PILA NI RESERVAR UN ESPACIO EN LA PILA PARA ELLAS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////////////////////////////////////////////////////////////////////////////////
// Devuelve el contenido de la posición indicadas por la fila y columna actual.
// Además informa si la posición es válida y contiene alguna ficha.
// Esto lo hace por referencia (en *posicion_valida)
// Si devuelve un 0 no es válida o está vacia.

signed char ficha_valida(signed char tablero[][DIM], signed char f, signed char c, int *posicion_valida)
{
    signed char ficha;

    // ficha = tablero[f][c];
    // no puede accederse a tablero[f][c]
    // ya que algún índice puede ser negativo

    if ((f < DIM) && (f >= 0) && (c < DIM) && (c >= 0) && (tablero[f][c] != CASILLA_VACIA))
    {
        *posicion_valida = 1;
        ficha = tablero[f][c];
    }
    else
    {
        *posicion_valida = 0;
        ficha = CASILLA_VACIA;
    }
    return ficha;
}

////////////////////////////////////////////////////////////////////////////////
// La función patrón volteo es una función recursiva que busca el patrón de volteo
// (n fichas del rival seguidas de una ficha del jugador actual) en una dirección determinada
// SF y SC son las cantidades a sumar para movernos en la dirección que toque
// color indica el color de la pieza que se acaba de colocar
// la función devuelve PATRON_ENCONTRADO (1) si encuentra patrón y NO_HAY_PATRON (0) en caso contrario
// FA y CA son la fila y columna a analizar
// longitud es un parámetro por referencia. Sirve para saber la longitud del patrón que se está analizando. Se usa para saber cuantas fichas habría que voltear

int patron_volteo(signed char tablero[][DIM], int *longitud, signed char FA, signed char CA, signed char SF, signed char SC, signed char color)
{
    int posicion_valida; // indica si la posición es valida y contiene una ficha de algún jugador
    int patron; //indica si se ha encontrado un patrón o no
    static int tiempoficha=0;
    static int tiempoficha2=0;
    signed char casilla;   // casilla es la casilla que se lee del tablero
    FA = FA + SF;
    CA = CA + SC;

    //timer2_empezar();
    //tiempoficha=timer2_leer();
    casilla = ficha_valida(tablero, FA, CA, &posicion_valida);
    //tiempoficha2=timer2_leer() - tiempoficha;
    //timer2_parar();

    // mientras la casilla está en el tablero, no está vacía,
    // y es del color rival seguimos buscando el patron de volteo
    if ((posicion_valida == 1) && (casilla != color))
    {
        *longitud = *longitud + 1;
        patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color);
        //printf("longitud: %d \n", *longitud);
        //printf("fila: %d; columna: %d \n", FA, CA);
        return patron;
    }
    // si la ultima posición era válida y la ficha es del jugador actual,
    // entonces hemos encontrado el patrón
    else if ((posicion_valida == 1) && (casilla == color))
    {
        if (*longitud > 0) // longitud indica cuantas fichas hay que voltear
            {
            return PATRON_ENCONTRADO; // si hay que voltear una ficha o más hemos encontrado el patrón
            //printf("PATRON_ENCONTRADO \n");
            }
        else {
            return NO_HAY_PATRON; // si no hay que voltear no hay patrón
            //printf("NO_HAY_PATRON \n");
            }
    }
    // en caso contrario es que no hay patrón
    else
    {
        return NO_HAY_PATRON;
        //printf("NO_HAY_PATRON \n");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Comprueba si, tras realizar un movimiento, tanto patron_volteo, patron_volteo_ARM_C y patron_volteo_ARM_ARM devuelven el mismo resultado, así como
//si el parámetro longitud, pasado por referencia, contiene el mismo valor en las 3 al terminar la ejecución dichas funciones.
//Además, se encarga de medir el tiempo que tarda la ejecución de cada una de ellas.


int patron_volteo_test(signed char tablero[][DIM], int *longitud, signed char f, signed char c, signed char SF, signed char SC, signed char color){
	int longitud_arm_c=*longitud;
	int longitud_arm_arm=*longitud;
	int tiempo1=0;
	int tiempo2=0;
	int tiempoARM_C1=0;
	int tiempoARM_C2=0;
	int tiempoARM_ARM1=0;
	int tiempoARM_ARM2=0;
	
	timer2_empezar();
	tiempo1=timer2_leer();
	int Rpatron_volteo = patron_volteo(tablero, longitud, f, c, SF, SC, color);
	tiempo2=timer2_leer()-tiempo1;
	tabla_tiempo[i]=tiempo2;

	tiempoARM_C1=timer2_leer();
	int Rpatron_volteo_arm_c = patron_volteo_arm_c(tablero, &longitud_arm_c, f, c, SF, SC, color);
	tiempoARM_C2=timer2_leer()-tiempoARM_C1;
	tabla_tiempo_ARM_C[i]=tiempoARM_C2;

	tiempoARM_ARM1=timer2_leer();
	int Rpatron_volteo_arm_arm = patron_volteo_arm_arm(tablero, &longitud_arm_arm, f, c, SF, SC, color);
	tiempoARM_ARM2=timer2_leer()-tiempoARM_ARM1;
	tabla_tiempo_ARM_ARM[i]=tiempoARM_ARM2;

	i++;

	while((Rpatron_volteo!=Rpatron_volteo_arm_c) || (Rpatron_volteo_arm_c!=Rpatron_volteo_arm_arm || *longitud!=longitud_arm_c || longitud_arm_c!=longitud_arm_arm)){}
	return Rpatron_volteo;
}

////////////////////////////////////////////////////////
//Realiza la acción de colocar una ficha automáticamente

/*
void mover() {
  char filasPrueba[] = {2, 5, 4, 1, 2, 6, 1};
  char columnasPrueba[] = {3, 4, 5, 3, 5, 5, 1};
  static int contador = 0;

  fila = filasPrueba[contador];
  columna = columnasPrueba[contador];

  ++contador;
}
*/
////////////////////////////////////////////////////////////////////////////////
// voltea n fichas en la dirección que toque
// SF y SC son las cantidades a sumar para movernos en la dirección que toque
// color indica el color de la pieza que se acaba de colocar
// FA y CA son la fila y columna a analizar
void voltear(signed char tablero[][DIM], signed char FA, signed char CA, signed char SF, signed char SC, int n, signed char color)
{
    int i;

    for (i = 0; i < n; i++)
    {
        FA = FA + SF;
        CA = CA + SC;
        tablero[FA][CA] = color;
    }
}
////////////////////////////////////////////////////////////////////////////////
// comprueba si hay que actualizar alguna ficha
// no comprueba que el movimiento realizado sea válido
// f y c son la fila y columna a analizar
// signed char vSF[DIM] = {-1,-1, 0, 1, 1, 1, 0,-1};
// signed char vSC[DIM] = { 0, 1, 1, 1, 0,-1,-1,-1};
int actualizar_tablero(signed char tablero[][DIM], signed char f, signed char c, signed char color, int *tiempoPatron)
{
    signed char SF, SC; // cantidades a sumar para movernos en la dirección que toque
    int i, flip, patron;
	int tiempo1=0;

    for (i = 0; i < DIM; i++) // 0 es Norte, 1 NE, 2 E ...
    {
        SF = vSF[i];
        SC = vSC[i];
        // flip: numero de fichas a voltear
        flip = 0;
		tiempo1=timer2_leer();
        patron=patron_volteo_arm_arm(tablero, &flip, f, c, SF, SC, color);
		*tiempoPatron=timer2_leer()-tiempo1;
        //printf("Flip: %d \n", flip);
        if (patron == PATRON_ENCONTRADO )
        {
            voltear(tablero, f, c, SF, SC, flip, color);
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////
// Recorre todo el tablero comprobando en cada posición si se puede mover
// En caso afirmativo, consulta la puntuación de la posición y si es la mejor
// que se ha encontrado la guarda
// Al acabar escribe el movimiento seleccionado en f y c

// Candidatas
// NO    0
// SI    1
// CASILLA_OCUPADA 2
int elegir_mov(signed char candidatas[][DIM], signed char tablero[][DIM], signed char *f, signed char *c)
{
    int i, j, k, found;
    int mf = -1; // almacena la fila del mejor movimiento encontrado
    int mc;      // almacena la columna del mejor movimiento encontrado
    signed char mejor = 0; // almacena el mejor valor encontrado
    int patron, longitud;
    signed char SF, SC; // cantidades a sumar para movernos en la dirección que toque

    // Recorremos todo el tablero comprobando dónde podemos mover
    // Comparamos la puntuación de los movimientos encontrados y nos quedamos con el mejor
    for (i=0; i<DIM; i++)
    {
        for (j=0; j<DIM; j++)
        {   // indica en qué casillas quizá se pueda mover
            if (candidatas[i][j] == SI)
            {
                if (tablero[i][j] == CASILLA_VACIA)
                {
                    found = 0;
                    k = 0;

                    // en este bucle comprobamos si es un movimiento válido
                    // (es decir si implica voltear en alguna dirección)
                    while ((found == 0) && (k < DIM))
                    {
                        SF = vSF[k];    // k representa la dirección que miramos
                        SC = vSC[k];    // 1 es norte, 2 NE, 3 E ...

                        // nos dice qué hay que voltear en cada dirección
                        longitud = 0;
                        patron=patron_volteo_arm_arm(tablero, &longitud, i, j, SF, SC, FICHA_BLANCA);
                        //patron = patron_volteo_arm_arm(tablero, &longitud, i, j, SF, SC, FICHA_BLANCA);
                        //  //printf("%d ", patron);
                        if (patron == PATRON_ENCONTRADO)
                        {
                            found = 1;
                            if (tabla_valor[i][j] > mejor)
                            {
                                mf = i;
                                mc = j;
                                mejor = tabla_valor[i][j];
                            }
                        }
                        k++;
                        // si no hemos encontrado nada probamos con la siguiente dirección
                    }
                }
            }
        }
    }
    *f = (signed char) mf;
    *c = (signed char) mc;
    // si no se ha encontrado una posición válida devuelve -1
    return mf;
}
////////////////////////////////////////////////////////////////////////////////
// Cuenta el número de fichas de cada color.
// Los guarda en la dirección b (blancas) y n (negras)
void contar(signed char tablero[][DIM], int *b, int *n)
{
    int i,j;

    *b = 0;
    *n = 0;

    // recorremos todo el tablero contando las fichas de cada color
    for (i=0; i<DIM; i++)
    {
        for (j=0; j<DIM; j++)
        {
            if (tablero[i][j] == FICHA_BLANCA)
            {
                (*b)++;
            }
            else if (tablero[i][j] == FICHA_NEGRA)
            {
                (*n)++;
            }
        }
    }
}

void actualizar_candidatas(signed char candidatas[][DIM], signed char f, signed char c)
{
    // donde ya se ha colocado no se puede volver a colocar
    // En las posiciones alrededor sí
    candidatas[f][c] = CASILLA_OCUPADA;
    if (f > 0)
    {
        if (candidatas[f-1][c] != CASILLA_OCUPADA)
            candidatas[f-1][c] = SI;

        if ((c > 0) && (candidatas[f-1][c-1] != CASILLA_OCUPADA))
            candidatas[f-1][c-1] = SI;

        if ((c < 7) && (candidatas[f-1][c+1] != CASILLA_OCUPADA))
            candidatas[f-1][c+1] = SI;
    }
    if (f < 7)
    {
        if (candidatas[f+1][c] != CASILLA_OCUPADA)
            candidatas[f+1][c] = SI;

        if ((c > 0) && (candidatas[f+1][c-1] != CASILLA_OCUPADA))
            candidatas[f+1][c-1] = SI;

        if ((c < 7) && (candidatas[f+1][c+1] != CASILLA_OCUPADA))
            candidatas[f+1][c+1] = SI;
    }
    if ((c > 0) && (candidatas[f][c-1] != CASILLA_OCUPADA))
        candidatas[f][c-1] = SI;

    if ((c < 7) && (candidatas[f][c+1] != CASILLA_OCUPADA))
        candidatas[f][c+1] = SI;
}




////////////////////////////////////////////////////////////////////////////////
// Proceso principal del juego
// Utiliza el tablero,
// y las direcciones en las que indica el jugador la fila y la columna
// y la señal de ready que indica que se han actualizado fila y columna
// tablero, fila, columna y ready son variables globales aunque deberían ser locales de reversi8,
// la razón es que al meterlas en la pila no las pone juntas, y así jugar es más complicado.
// en esta versión el humano lleva negras y la máquina blancas
// no se comprueba que el humano mueva correctamente.
// Sólo que la máquina realice un movimiento correcto.


/*
 * Devuelve el valor de la posiciï¿½n del tablero indicado mediante la fila y la columna pasadas por parï¿½metro
 */
char fichaPosicion(int fila, int columna){
	return tablero[fila][columna];
}

int puedoColocar(int xpos, int ypos){
	if(tablero[xpos+1][ypos]==1 || tablero[xpos-1][ypos]==1 || tablero[xpos][ypos+1]==1 || tablero[xpos][ypos-1]==1
		|| tablero[xpos+1][ypos+1]==1 ||  tablero[xpos-1][ypos-1] || tablero[xpos+1][ypos-1]==1 ||  tablero[xpos-1][ypos+1]){

		return 1;
	}
	else{
		return 0;
	}
}

int fichaPosicionCandidatas(int fila, int columna){
	return candidatas[fila][columna];
}

int reversi8(char fila, char columna, int *tiempoPatron, int *fin)
{
	int done;     // la máquina ha conseguido mover o no
	int move = 0; // el humano ha conseguido mover o no
	int blancas, negras; // número de fichas de cada color
	*fin = 0;  // fin vale 1 si el humano no ha podido mover
					  // (ha introducido un valor de movimiento con algún 8)
					  // y luego la máquina tampoco puede
	signed char f, c;    // fila y columna elegidas por la máquina para su movimiento
	move = 0;

	if(fila!=8 && columna!=8){	//si puedes mover
		//esperar_mov(&ready);
		// si la fila o columna son 8 asumimos que el jugador no puede mover
			if (((fila) != DIM) && ((columna) != DIM))
			{
				tablero[fila][columna] = FICHA_NEGRA;
				actualizar_tablero(tablero, fila, columna, FICHA_NEGRA, tiempoPatron);
				actualizar_candidatas(candidatas, fila, columna);
				move = 1;
			}
	}


	// escribe el movimiento en las variables globales fila columna
	done = elegir_mov(candidatas, tablero, &f, &c);
	if (done == -1)
	{
		if (move == 0)
			*fin = 1;
	}
	else
	{
		tablero[f][c] = FICHA_BLANCA;
		actualizar_tablero(tablero, f, c, FICHA_BLANCA, tiempoPatron);
		actualizar_candidatas(candidatas, f, c);
		move=0;
	}
	contar(tablero, &blancas, &negras);
	if(blancas>negras){
		return 1;
	}
	else if(negras>blancas){
		return 2;
	}
	else{
		return 3;
	}
}


void reversi_main(){
	init_table(tablero, candidatas);
}
