#SimuProc 1.4.2.0
JMP 020
MSG **************************
MSG * Coffee Star CashierApp *
MSG **************************
MSG * 1) Make order          *
MSG * 2) Check cashbox       *
MSG * 3) Quit                *
MSG **************************
LDT     ;Recibir dato para menú
CMP 101 ;Comparar con posición 101 (1)
JEQ 110 ;Si son iguales, saltar a 110
CMP 102 ;Comparar con posición 102 (2)
JEQ 4E0 ;Si son iguales, saltar a 200
CMP 103 ;Comparar con posición 103 (3)
JEQ 800 ;Si son iguales, saltar a 800
JMP 0   ;Si no es igual a ninguno, volver al menú

#020
CLA        ;Limpio el contenido de AX
MOV 3F0,AX ;Limpio el contenido del pedido
LDF 420    ;Cargo en AX y BX el valor de un café (2.0)
STF 280    ;Pongo ese valor en la cuenta del pedido
MOV 286,100;Limpio valor del TOTAL del café más sig.
MOV 287,100;Limpio valor del TOTAL del café menos sig.
MOV 288,100;Limpio valor del TOTAL de la factura sin IVA más sig.
MOV 289,100;Limpio valor del TOTAL de la factura sin IVA menos sig.
MOV 28A,100;Limpio valor del TOTAL + IVA de la factura más sig.
MOV 28B,100;Limpio valor del TOTAL + IVA de la factura menos sig.
MOV AX,3F1 ;Movemos a AX el valor centinela
PUSH AX    ;Ingresamos a la pila un número con todos los bits en 1 (centinela)
JMP 001    ;Vuelvo al inicio

#100
0000000000000000 ;0
0000000000000001 ;1
0000000000000010 ;2
0000000000000011 ;3
0000000000000100 ;4
0000000000000101 ;5
0000000000000110 ;6
0000000000000111 ;7
0000000000001000 ;8
0000000000001001 ;9
0000000000001010 ;10
0000000000000000 ;0 FLOAT más sig.
0000000000000000 ;0 FLOAT menos sig.

#110
MSG *******************************************
MSG * Select topping to add it or remove it   *
MSG *         from the current coffee         *
MSG *******************************************
MSG *      Topping     *  Price  *   Added    *
MSG *******************************************
JMP 170    ; (Ref: 116) Salto a imprimir información de Crema Batida
JMP 180    ; (ref: 117) Salto a imprimir información de Canela
JMP 190    ; (ref: 118) Salto a imprimir información de Chocolate
JMP 1A0    ; (ref: 119) Salto a imprimir información de Amaretto
JMP 1B0    ; (ref: 11A) Salto a imprimir información de Whisky
MSG *******************************************
MSG * 0) Continue                             *
MSG * 8) Cancel coffee                        *
MSG * 9) Cancel order                         *
MSG *******************************************
LDT
CMP 100 ;Comparo lo que ingresé con 0
JEQ 1C0 ;Si es 0 entonces voy a la 1C0
CMP 101 ;Comparo lo que ingresé con 1
JEQ 14A ;Si es 1 entonces voy a la 14A
CMP 102 ;Comparo lo que ingresé con 2
JEQ 150 ;Si es 2 entonces voy a la 150
CMP 103 ;Comparo lo que ingresé con 3
JEQ 15A ;Si es 3 entonces voy a la 15A
CMP 104 ;Comparo lo que ingresé con 4
JEQ 160 ;Si es 4 entonces voy a la 160
CMP 105 ;Comparo lo que ingresé con 5
JEQ 16A ;Si es 5 entonces voy a la 16A
CMP 108 ;Comparo lo que ingresé con 8
JEQ 200 ;Si es 9 entonces voy a la 135
CMP 109 ;Comparo lo que ingresé con 9
JEQ 135 ;Si es 9 entonces voy a la 135
MSG Invalid option!
JMP 110

#135
CLA        ;Limpio el auxiliar
MOV 3F0,AX ;Limpio el pedido
MSG Order has been canceled
JMP 0      ;Voy al inicio

#14A
MOV AX,3F0  ;Muevo a AX el pedido actual
XOR 3F0,400 ;Hago XOR del pedido con Crema Batida (Flip al bit Crema Batida)
JMP 110     ;Especie de return NOTA: Si cambia código revisar ref.

#150
MOV AX,3F0  ;Muevo a AX el pedido actual
XOR 3F0,401 ;Hago XOR del pedido con Canela (Flip al bit Canela)
JMP 110     ;Especie de return NOTA: Si cambia código revisar ref.

#15A
MOV AX,3F0  ;Muevo a AX el pedido actual
XOR 3F0,402 ;Hago XOR del pedido con Chocolate (Flip al bit Chocolate)
JMP 110     ;Especie de return NOTA: Si cambia código revisar ref.

#160
MOV AX,3F0  ;Muevo a AX el pedido actual
XOR 3F0,403 ;Hago XOR del pedido con Amaretto (Flip al bit Amaretto)
JMP 110     ;Especie de return NOTA: Si cambia código revisar ref.

#16A
MOV AX,3F0  ;Muevo a AX el pedido actual
XOR 3F0,404 ;Hago XOR del pedido con Whisky (Flip al bit Whisky)
JMP 110     ;Especie de return NOTA: Si cambia código revisar ref.

#170
CLA        ;Limpia AX
MOV AX,3F0 ;Pongo en AX el pedido actual (3F0)
AND AX,400 ;Hago AND entre pedido actual y Crema Batida (400)
CMP 100    ;Comparo ese AND con 0 (Ref. 100)
JEQ 177    ;Si me dio igual salto al no, sino imprimo que si lo tengo
MSG * 1) Whipped Cream *  $0.89  *     YES    * ;175
JMP 178    ;Si imprimí que si lo tenía, salto para no volver a imprimir
MSG * 1) Whipped Cream *  $0.89  *     NO     * ;177
JMP 117    ;Esto es como un return. NOTA: Si se cambia codigo revisar ref.

#180
CLA        ;Limpia AX
MOV AX,3F0 ;Pongo en AX el pedido actual (3F0)
AND AX,401 ;Hago AND entre pedido actual y Canela (401)
CMP 100    ;Comparo ese AND con 0 (Ref. 100)
JEQ 187    ;Si me dio igual salto, sino imprimo que ya lo tengo
MSG * 2) Cinnamon      *  $0.25  *     YES    * ;185
JMP 188
MSG * 2) Cinnamon      *  $0.25  *     NO     * ;187
JMP 118    ;Esto es como un return. NOTA: Si se cambia codigo revisar ref.

#190
CLA        ;Limpia AX
MOV AX,3F0 ;Pongo en AX el pedido actual (3F0)
AND AX,402 ;Hago AND entre pedido actual y Chocolate (402)
CMP 100    ;Comparo ese AND con 0 (Ref. 100)
JEQ 197    ;Si me dio igual salto, sino imprimo que ya lo tengo
MSG * 3) Chocolate     *  $0.59  *     YES    *
JMP 198
MSG * 3) Chocolate     *  $0.59  *     NO     *
JMP 119    ;Esto es como un return. NOTA: Si se cambia codigo revisar ref.

#1A0
CLA        ;Limpia AX
MOV AX,3F0 ;Pongo en AX el pedido actual (3F0)
AND AX,403 ;Hago AND entre pedido actual y Amaretto (403)
CMP 100    ;Comparo ese AND con 0 (Ref. 100)
JEQ 1A7    ;Si me dio igual salto, sino imprimo que ya lo tengo
MSG * 4) Amaretto      *  $1.50  *     YES    *
JMP 1A8
MSG * 4) Amaretto      *  $1.50  *     NO     *
JMP 11A    ;Esto es como un return. NOTA: Si se cambia codigo revisar ref.

#1B0
CLA        ;Limpia AX
MOV AX,3F0 ;Pongo en AX el pedido actual (3F0)
AND AX,404 ;Hago AND entre pedido actual y Whisky (404)
CMP 100    ;Comparo ese AND con 0 (Ref. 100)
JEQ 1B7    ;Si me dio igual salto, sino imprimo que ya lo tengo
MSG * 5) Irish Whiskey *  $1.75  *     YES    *
JMP 1B8
MSG * 5) Irish Whiskey *  $1.75  *     NO     *
JMP 11B    ;Esto es como un return. NOTA: Si se cambia codigo revisar ref.

#1C0
MSG ***************************
MSG *     CURRENT COFFEE      *
MSG ***************************
MSG * Coffee          ($2.00) *
JMP 220
MSG ***************************
MSG * Coffee Price (+ IVA):   *
LDF 280   ;Movemos el valor de la cuenta a AX y BX
MULF 432
OUT 1,AX  ;Imprimimos el valor
MSG ***************************
MSG ***************************
MSG * Accumulated Total:      *
LDF 280   ;Cargamos el valor (SIN IVA) del café actual
ADDF 288  ;Sumamos el valor total de la factura (SIN IVA)
MULF 432  ;Multiplicamos por 1.10 para obtener el acumulado + IVA
OUT 1,AX  ;Mostramos el acumulado TOTAL + IVA
MSG ***************************
MSG * 0) Confirm coffee       *   ;Esto ya implica GUARDAR en la pila.
MSG * 1) Edit coffee          *
MSG * 8) Cancel coffee        *
MSG * 9) Cancel order         *
MSG ***************************
LDT     ;Recibir dato para menú
CMP 100 ;Comparar con posición 100 (0)
JEQ 360 ;Si son iguales, saltar a 360
CMP 101 ;Comparar con posición 101 (1)
JEQ 110 ;Si son iguales, saltar a 110
CMP 108 ;Comparar con posición 108 (8)
JEQ 200 ;Si son iguales, saltar al 200
CMP 109 ;Comparar con posición 109 (9)
JEQ 0   ;Si son iguales, saltar al inicio
JMP 1C0 ;Si no es igual a ninguno, volver al menú

#200
LDF 288     ;Se carga el valor actual del pedido
SUBF 10B    ;Se compara con 0 FLOAT para saber si ya se adicionó algún café
JEQ 0       ;Si no se ha adicionado nada se comienza desde el principio
JMP 390     ;Si es mayor que cero se debe dirigir a la opción de pagar

#220
LDF 420         ;Movemos el precio del café (2.0) a AX y BX
STF 280         ;Llevamos ese valor al total del café actual (reseteamos valor)
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,400      ;Hacemos el AND pedido actual y Crema Batida
CMP 100         ;Comparamos el resultado con 0
JEQ 22A         ;Si es igual a 0, saltamos a la otra adicion
LDF 280         ;Cargo el valor que lleva la cuenta
ADDF 422        ;Sumo el valor de la adición
STF 280         ;Guardo el resultado en el acumulador
MSG * > Whipped Cream ($0.89) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,401      ;Hacemos el AND pedido actual y Canela
CMP 100         ;Comparamos el resultado con 0
JEQ 232         ;Si es igual a 0, saltamos a la otra adicion
LDF 280         ;Cargo el valor que lleva la cuenta
ADDF 424        ;Sumo el valor de la adición
STF 280         ;Guardo el resultado en el acumulador
MSG * > Cinnamon      ($0.25) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,402      ;Hacemos el AND pedido actual y
CMP 100         ;Comparamos el resultado con 0
JEQ 23A         ;Si es igual a 0, saltamos a la otra adicion
LDF 280         ;Cargo el valor que lleva la cuenta
ADDF 426        ;Sumo el valor de la adición
STF 280         ;Guardo el resultado en el acumulador
MSG * > Chocolate     ($0.59) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,403      ;Hacemos el AND pedido actual y Amaretto
CMP 100         ;Comparamos el resultado con 0
JEQ 242         ;Si es igual a 0, saltamos a la otra adicion
LDF 280         ;Cargo el valor que lleva la cuenta
ADDF 428        ;Sumo el valor de la adición
STF 280         ;Guardo el resultado en el acumulador
MSG * > Amaretto      ($1.50) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,404      ;Hacemos el AND pedido actual y Whisky
CMP 100         ;Comparamos el resultado con 0
JEQ 24A         ;Si es igual a 0, saltamos a la otra adicion
LDF 280         ;Cargo el valor que lleva la cuenta
ADDF 42A        ;Sumo el valor de la adición
STF 280         ;Guardo el resultado en el acumulador
MSG * > Irish Whiskey ($1.75) *
JMP 1C5         ;Retorno a donde me llamé

#280
0100000000000000 ;Valor inicial del pedido más sig.
0000000000000000 ;Valor inicial del pedido menos sig.
0000000000000000 ;Valor IVA de la factura TOTAL más sig.
0000000000000000 ;Valor IVA de la factura TOTAL menos sig.
0000000000000000 ;Valor IVA café actual más sig.
0000000000000000 ;Valor IVA café actual menos sig.
0000000000000000 ;Variable para almacenar el TOTAL más sig.
0000000000000000 ;Variable para almacenar el TOTAL menos sig.
0000000000000000 ;Variable para almacenar el TOTAL de la factura final sin IVA.
0000000000000000 ;Variable para almacenar el TOTAL de la factura final sin IVA.
0000000000000000 ;Variable para almacenar el TOTAL + IVA de la factura.
0000000000000000 ;Variable para almacenar el TOTAL + IVA de la factura.



#2A0
MSG ****************************
MSG *    VALUE TO PAY:         *
LDF 288  ;Movemos el valor total de la factura a AX y BX
MULF 432 ;Multiplicamos el valor por 1.10 para sacarle el IVA
STF 28A  ;Almacenamos el valor TOTAL de la factura + IVA
OUT 1,AX ;Imprimimos el valor total
MSG ****************************
MSG * Enter amount paid:       *
IN AX,1  ;Leemos el valor con el que va a pagar el cliente
STF 42E  ;Guardamos el valor con el que va a pagar el cliente
MSG ****************************
;OUT 1,AX ;Imprimimos el valor con el que va a pagar el cliente
SUBF 28A ;Resta del valor usuario - total
SUBF 100 ;Compararmos con 0
JME 310  ;Si la diferencia es menor que 0 el pago es insuficiente
JMP 2C0  ;Si el valor es correcto saltamos a la factura

#2B1
MSG ***************************
MSG *      COFFEE ADDED       *
MSG ***************************
MSG * 0) Finish and pay order *
MSG * 1) Add another coffee   *
MSG * 9) Cancel order         *
MSG ***************************
LDT
CMP 100
JEQ 2A0     ;El usuario decide terminar su pedido y pagar
CMP 101
JEQ 380     ;Saltamos para limpiar variables y hacer otro café
CMP 109
JEQ 0
JMP 2B1     ;No se preocupe que no se overlapan

#2C0
MSG
MSG **********************************
MSG **********************************
MSG **********************************
MSG **********************************
MSG **** >>    BILL OF SALE    << ****
MSG **********************************
MSG * Organization: Coffee Star      *
MSG **********************************
JMP 410
MSG **********************************
MSG * Subtotal:                      *
LDF 288   ;Movemos el valor de la factura a AX y BX
OUT 1,AX  ;Imprimimos el valor
MSG * + IVA (10 %):                  *
MULF 42C  ;Multiplicamos el valor por 0.10
OUT 1,AX  ;Imprimimos el IVA
STF 282   ;Almacenamos el IVA en la variable
LDF 288   ;Movemos nuevamente el valor total factura a AX y BX
ADDF 282  ;Sumamos el IVA
MSG **********************************
MSG *             TOTAL:             *
MSG **********************************
OUT 1,AX  ;Imprimimos el TOTAL de la factura
STF 28A   ;Almacenamos el valor TOTAL + IVA de la factura
MSG **********************************
MSG * Amount paid:                   *
LDF 42E   ;Movemos el valor pagado por el cliente a AX y BX
OUT 1,AX  ;Imprimimos el valor
MSG * Change:                        *
SUBF 28A  ;Le restamos el valor TOTAL + IVA de la factura para saber el cambio
OUT 1,AX
MSG **********************************
MSG *    Thanks for visiting us      *
MSG **********************************
MSG
MSG  Press any number to print bill of sale and continue...
LDT       ;Recibir cualquier dato para ir al menú
JMP 0

#2F0
MSG * Coffee                 ($2.00) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,400      ;Hacemos el AND pedido actual y Crema Batida
CMP 100         ;Comparamos el resultado con 0
JEQ 2F6         ;Si es igual a 0, saltamos a la otra adicion
MSG * > Whipped Cream        ($0.89) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,401      ;Hacemos el AND pedido actual y Canela
CMP 100         ;Comparamos el resultado con 0
JEQ 2FB         ;Si es igual a 0, saltamos a la otra adicion
MSG * > Cinnamon             ($0.25) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,402      ;Hacemos el AND pedido actual y Chocolate
CMP 100         ;Comparamos el resultado con 0
JEQ 300         ;Si es igual a 0, saltamos a la otra adicion
MSG * > Chocolate            ($0.59) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,403      ;Hacemos el AND pedido actual y Amaretto
CMP 100         ;Comparamos el resultado con 0
JEQ 305         ;Si es igual a 0, saltamos a la otra adicion
MSG * > Amaretto             ($1.50) *
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,404      ;Hacemos el AND pedido actual y Whisky
CMP 100         ;Comparamos el resultado con 0
JEQ 30A         ;Si es igual a 0, saltamos a la otra adicion
MSG * > Irish Whiskey        ($1.75) *
MSG *                                *
JMP 415         ;Retorno para incrementar variables para cierre de caja

#310
MSG * Amount paid is not enough*
JMP 2A0

#360
LDF 288   ;Cargue el valor TOTAL de la factura en AX y BX
ADDF 280  ;Sumamos el valor del café (SIN IVA) actual al TOTAL de la factura
STF 288   ;Almacemanos el valor total de la factura nuevamente
MOV AX,3F0;Muevo a AX el pedido actual confirmado
PUSH AX   ;Meto a la pila el pedido realizado
JMP 2B1   ;Saltar a preguntar entre pagar o pedir otro café.

#380
MOV 3F0,100       ;Enviar al pedido actual un número con todos los bits en cero
JMP 110

#390
MSG ***************************
MSG *     COFFEE CANCELED     *
MSG ***************************
JMP 2B4

#3F0
0000000000000000 ;Pedido actual
0111111111111111 ;Valor centinela que representa el final de la pila

#400
0000000000000001 ;Crema batida
0000000000000010 ;Canela
0000000000000100 ;Chocolate
0000000000001000 ;Amaretto
0000000000010000 ;Whisky irlandes

#410
POP AX           ;Sacamos el último registro de la pila
CMP 3F1          ;Se compara ese valor con el valor centinela
JEQ 2CA          ;Si el valor retirado es el centinela, reiniciamos desde cero
MOV 3F0,AX       ;Se mueve a 3F0 lo que hay en AX
JMP 2F0          ;Se muestra la descripción por cada café
JMP 480          ;Se incrementan los contadores de adiciones y café para cierre
JMP 410          ;Saca registros mientras no encuentre el centinela (ciclo)

#420
0100000000000000 ;Café más sig.           420
0000000000000000 ;Café menos sig.
0011111101100011 ;Crema Batida más sig.   422
1101011100001010 ;Crema Batida menos sig.
0011111010000000 ;Canela más sig.         424
0000000000000000 ;Canela menos sig.
0011111100010111 ;Chocolate más sig.      426
0000101000111101 ;Chocolate menos sig.
0011111111000000 ;Amaretto más sig.       428
0000000000000000 ;Amaretto menos sig.
0011111111100000 ;Whisky más sig.         42A
0000000000000000 ;Whisky menos sig.
0011110111001100 ;Porcentaje IVA más sig. 42C
1100110011001000 ;Porcentaje IVA menos sig.
0000000000000000 ;Valor con el que paga el cliente más sig. 42E
0000000000000000 ;Valor con el que paga el cliente menos sig.
0000000000000000 ;Cambio cliente más sig. 430
0000000000000000 ;Cambio cliente menos sig.
0011111110001100 ;Variable para sumar el IVA (1.10) 432
1100110011001100 ;Variable para sumar el IVA (1.10)

#450
0000000000000000 ;(450) Cantidad de cafés vendidos en el día
0000000000000000 ;(451) Cantidad de Crema Batida vendida en el día
0000000000000000 ;(452) Cantidad de Canela vendida en el día
0000000000000000 ;(453) Cantidad de Chocolate vendido en el día
0000000000000000 ;(454) Cantidad de Amaretto vendido en el día
0000000000000000 ;(455) Cantidad de Whisky vendido en el día
0000000000000000 ;(456) Precio total vendido al cierre sin IVA más sig.
0000000000000000 ;(457) Precio total vendido al cierre sin IVA menos sig.

#480
INC 450         ;Se incrementa la cantidad de cafés vendidos
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,400      ;Hacemos el AND pedido actual y Crema Batida
CMP 100         ;Comparamos el resultado con 0
JEQ 486         ;Si es igual a 0, saltamos a la otra adicion
INC 451         ;Incrementamos cantidad de Cremas Batidas vendidas
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,401      ;Hacemos el AND pedido actual y Canela
CMP 100         ;Comparamos el resultado con 0
JEQ 48B         ;Si es igual a 0, saltamos a la otra adicion
INC 452         ;Incrementamos cantidad de Canelas vendidas
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,402      ;Hacemos el AND pedido actual y Chocolate
CMP 100         ;Comparamos el resultado con 0
JEQ 490         ;Si es igual a 0, saltamos a la otra adicion
INC 453         ;Incrementamos cantidad de Chocolate vendido
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,403      ;Hacemos el AND pedido actual y Amaretto
CMP 100         ;Comparamos el resultado con 0
JEQ 495         ;Si es igual a 0, saltamos a la otra adicion
INC 454         ;Incrementamos cantidad de Amarrettos vendidos
MOV AX,3F0      ;Movemos el pedido actual a AX
AND AX,404      ;Hacemos el AND pedido actual y Whisky
CMP 100         ;Comparamos el resultado con 0
JEQ 49A         ;Si es igual a 0, saltamos a la otra adicion
INC 455         ;Incrementamos cantidad de Whisky vendido
JMP 416         ;Retorno para seguir sacando cafés de la pila

#4E0
LDF 10B    ;Cargamos el valor 0 flotante a AX y BX
STF 456    ;Guardamos ese 0 (limpiamos) el precio total vendido al cierre
MSG ****************************************
MSG *            CASHBOX CHECK             *
MSG ****************************************
MOV AX,450 ;Movemos la cantidad de cafés vendidos a AX
MSG * COFFEES SOLD                         *
MSG ****************************************
EAP *   Units sold:                        *
MSG *   Unit price: $2.00                  *
ITOF       ;Convertimos la cantidad de cafés a flotante
MULF 420   ;Multiplicamos la cantidad de cafés por su precio
MSG *   Total price:                       *
OUT 1,AX   ;Imprimimos el valor total de los cafes
ADDF 456   ;Sumamos el valor total del cierre al valor de los cafés vendidos
STF 456    ;Almacenamos el nuevo valor total del cierre
MSG ****************************************
MSG * TOPPINGS SOLD                        *
MSG ****************************************
MSG * > Whipped Cream                      *
MOV AX,451 ;Movemos la cantidad de adiciones de Crema Batida vendidas a AX
EAP *    Units sold:                       *
MSG *    Unit price: $0.89                 *
MSG *    Total price:                      *
ITOF       ;Convertimos la cantidad de adiciones a flotante
MULF 422   ;Multiplicamos la cantidad de cafés por su precio
OUT 1,AX   ;Imprimimos el valor total de la adición
ADDF 456   ;Sumamos el valor total del cierre al valor de la adición vendida
STF 456    ;Almacenamos el nuevo valor total del cierre
MSG *                                      *
MSG * > Cinnamon                           *
MOV AX,452 ;Movemos la cantidad de adiciones de Canela vendidas a AX
EAP *    Units sold:                       *
MSG *    Unit price: $0.25                 *
MSG *    Total price:                      *
ITOF       ;Convertimos la cantidad de adiciones a flotante
MULF 424   ;Multiplicamos la cantidad de cafés por su precio
OUT 1,AX   ;Imprimimos el valor total de la adición
ADDF 456   ;Sumamos el valor total del cierre al valor de la adición vendida
STF 456    ;Almacenamos el nuevo valor total del cierre
MSG *                                      *
MSG * > Chocolate                          *
MOV AX,453 ;Movemos la cantidad de adiciones de Chocolate vendidas a AX
EAP *    Units sold:                       *
MSG *    Unit price: $0.59                 *
MSG *    Total price:                      *
ITOF       ;Convertimos la cantidad de adiciones a flotante
MULF 426   ;Multiplicamos la cantidad de cafés por su precio
OUT 1,AX   ;Imprimimos el valor total de la adición
ADDF 456   ;Sumamos el valor total del cierre al valor de la adición vendida
STF 456    ;Almacenamos el nuevo valor total del cierre
MSG *                                      *
MSG * > Amaretto                           *
MOV AX,454 ;Movemos la cantidad de adiciones de Crema Batida vendidas a AX
EAP *    Units sold:                       *
MSG *    Unit price: $1.50                 *
MSG *    Total price:                      *
ITOF       ;Convertimos la cantidad de adiciones a flotante
MULF 428   ;Multiplicamos la cantidad de cafés por su precio
OUT 1,AX   ;Imprimimos el valor total de la adición
ADDF 456   ;Sumamos el valor total del cierre al valor de la adición vendida
STF 456    ;Almacenamos el nuevo valor total del cierre
MSG *                                      *
MSG * > Irish Whiskey                      *
MOV AX,455 ;Movemos la cantidad de adiciones de Crema Batida vendidas a AX
EAP *    Units sold:                       *
MSG *    Unit price: $1.75                 *
MSG *    Total price:                      *
ITOF       ;Convertimos la cantidad de adiciones a flotante
MULF 42A   ;Multiplicamos la cantidad de cafés por su precio
OUT 1,AX   ;Imprimimos el valor total de la adición
ADDF 456   ;Sumamos el valor total del cierre al valor de la adición vendida
STF 456    ;Almacenamos el nuevo valor total del cierre
MSG ****************************************
MSG * SUBTOTAL                             *
OUT 1,AX   ;Imprimimos el valor total del cierre que ya estaba en AX y BX
MSG * IVA (10 %)                           *
MULF 42C   ;Multiplicamos el valor por 0.10
OUT 1,AX   ;Imprimimos el IVA
ADDF 456   ;Sumamos el valor total del cierre al IVA
MSG ****************************************
MSG *          TOTAL SOLD TODAY            *
MSG ****************************************
OUT 1,AX  ;Imprimimos el TOTAL del cierre de caja
MSG ****************************************
MSG
MSG  Press any number to save cashbox check and continue...
LDT       ;Recibir cualquier dato para ir al menú
JMP 0

#800
MOV 450,100 ;Resetear el valor de cafés vendidos a 0
MOV 451,100 ;Resetear el valor de Cremas Batidas vendidas a 0
MOV 452,100 ;Resetear el valor de Canela vendidas a 0
MOV 453,100 ;Resetear el valor de Chocolate vendidas a 0
MOV 454,100 ;Resetear el valor de Amaretto vendidas a 0
MOV 455,100 ;Resetear el valor de Whisky vendidas a 0
MSG See you tomorrow...
HLT
