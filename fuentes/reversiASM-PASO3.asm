//Autores: Daniel Cay (741066) y Alejandro Gutiérrez (735089)


.global patron_volteo_arm_c
.type patron_volteo_arm_c,%function

////////////////////////////////////////////////////////////////////////////////
// La funcion patron volteo_arm_c es una funcion recursiva que busca el patron de volteo
// (n fichas del rival seguidas de una ficha del jugador actual) en una direccion determinada.
// SF y SC son las cantidades a sumar para movernos en la direccion que toque
// color indica el color de la pieza que se acaba de colocar
// la funcion devuelve PATRON_ENCONTRADO (1) si encuentra patron y NO_HAY_PATRON (0) en caso contrario
// FA y CA son la fila y columna a analizar
// longitud es un parametro por referencia. Sirve para saber la longitud del patron que se esta analizando.
// Se usa para saber cuantas fichas habra que voltear

//PARAMETROS POR REGISTRO PATRON_VOLTEO
// r0 = direccion tablero
// r1 = direccion longitud
// r2 = FA
// r3 = CA

//PARAMETROS POR REGISTRO FICHA_VALIDA
// r0 = direccion tablero
// r1 = FA
// r2 = CA
// r3 = posicion_valida


//				PILA
// SF 				-0x52
// SC				-0x48
// color			-0x44
// posicion_valida		-0x40
// r4 (SF) 			-0x36
// r5 (SC)			-0x32
// r6 (color)			-0x28
// r7 (casilla)			-0x24
// r8 (longitud)		-0x20
// r9 (posicion_valida)		-0x16
// fp 				-0x12
// sp(ip) 			-0x8
// lr 				-0x4
// pc 				 0x0
// SF 				 0x4
// SC				 0x8
// color			 0x12

patron_volteo_arm_c:
	mov r12, sp
  	stmdb sp!,{r4-r9,fp,r12,lr,pc}
	sub r11, r12, #4
 	sub sp,sp,#4		//espacio para posicion_valida(resultado de ficha_valida)

	ldrsb r4,[r11,#4]	//r4=SF
	ldrsb r5,[r11,#8]	//r5=SC
	ldrb r6,[r11,#12]	//r6=color

	add r2,r2,r4		//r2=FA=FA+SF
	add r3,r3,r5		//r3=CA=CA+SC

	PUSH {r0-r3}		//Salvaguardar registros

	mov r1,r2				//r1=FA
	mov r2,r3				//r2=CA
	sub r3,fp,#40		//r3=longitud

	bl ficha_valida  	//r0=casilla

	mov r7,r0		//r7=casilla

	POP {r0-r3}	//Recuperar registros (r0=tablero r1=longitud r2=FA r3=CA)

	ldr r9,[fp,#-40]	//r9=posicion valida
	cmp r9,#1		//posicion_valida == 1
	movne r0,#0		//return NO_HAY_PATRON
	bne FIN
	cmp r7,r6		// casilla != color
	ldr r8,[r1]		//r8=*longitud
	beq ccigual

	add r8,r8,#1	//*longitud = *longitud + 1;
	str r8,[r1]

	PUSH {r4-r6}
	bl patron_volteo_arm_c		//r0=resultado, return patron
	add sp,sp,#12
	b FIN

ccigual:
	cmpeq r8,#0		//*longitud > 0
	movgt r0, #1	//return PATRON_ENCONTRADO
  	movle r0, #0	//return NO_HAY_PATRON

FIN:
	ldmdb fp, {r4-r9, fp, sp, pc}
