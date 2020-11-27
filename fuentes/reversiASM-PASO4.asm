//Autores: Daniel Cay (741066) y Alejandro Gutiérrez (735089)


.global patron_volteo_arm_arm
.type patron_volteo_arm_arm,%function

////////////////////////////////////////////////////////////////////////////////
// La funcion patron volteo_arm_arm es una funcion recursiva que busca el patron de volteo
// (n fichas del rival seguidas de una ficha del jugador actual) en una direccion determinada.
// SF y SC son las cantidades a sumar para movernos en la direccion que toque
// color indica el color de la pieza que se acaba de colocar
// la funcion devuelve PATRON_ENCONTRADO (1) si encuentra patron y NO_HAY_PATRON (0) en caso contrario
// FA y CA son la fila y columna a analizar
// longitud es un parametro por referencia. Sirve para saber la longitud del patron que se esta analizando.
// Se usa para saber cuantas fichas habra que voltear


// r0 = direccion tablero
// r1 = direccion longitud
// r2 = FA
// r3 = CA

//					PILA
// r4 (SF) 				-0x40
// r5 (SC)				-0x36
// r6 (color)				-0x32
// r7 (tablero+lsl)			-0x28
// r8 (tablero[f][c])			-0x24
// r9 (posicion_valida)			-0x20
// r10 (casilla)			-0x16
// fp 					-0x12
// sp(ip) 				-0x8
// lr 					-0x4
// pc 					 0x0
// SF 					 0x4
// SC				  	 0x8
// color				 0x12


patron_volteo_arm_arm:
	mov r12, sp
  	stmdb sp!,{r4-r10,fp,r12,lr,pc}
	sub r11, r12, #4

	ldrsb r4,[r11,#4]	//r4=SF
	ldrsb r5,[r11,#8]	//r5=SC
	ldrb r6,[r11,#12]	//r6=color

	add r2,r2,r4		//r2=FA=FA+SF
	add r3,r3,r5		//r3=CA=CA+SC

	mov r9, #0			//posicion_valida
	mov r10, #0			//casilla

   	cmp r2, #7				//f<DIM
   	bhi else

   	cmp r3, #7				//c<DIM
   	bhi else

   	add r7,r0,r2,lsl #3			//r7=tablero+lsl

   	ldrb r8, [r7, r3]   			//r9=tablero[f][c]
   	cmp r8, #0				//tablero[f][c] != CASILLA_VACIA
   	beq else

   	mov r9, #1		//*posicion_valida=1
   	mov r10,r8		//ficha = tablero[f][c]
   	b finficha

else:
   	mov r10, #0		//*posicion_valida = 0;

finficha:
	cmp r9,#1		//posicion_valida == 1
	movne r0,#0		//return NO_HAY_PATRON
	bne FIN
	cmp r10,r6		//casilla != color
	ldr r8,[r1]		//r8=*longitud
	beq ccigual

	add r8,r8,#1	//*longitud = *longitud + 1;
	str r8,[r1]

	PUSH {r4-r6}

	bl patron_volteo_arm_arm		//r0=resultado, return patron

	add sp,sp,#12

	b FIN

ccigual:
	cmpeq r8,#0		//*longitud > 0
	movgt r0, #1	//return PATRON_ENCONTRADO
   	movle r0, #0	//return NO_HAY_PATRON
FIN:
	ldmdb fp, {r4-r10, fp, sp, pc}




