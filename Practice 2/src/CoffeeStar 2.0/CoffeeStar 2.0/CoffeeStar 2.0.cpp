// CoffeeStar 2.0.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;

const int MAX_TABLES = 20;
const int MAX_COFFEES = 8;
const int COFFEE_ADDITION = 5;

// Variable para saber si estamos haciendo una orden en una mesa o en la barra.
bool isTableOrder;
// Hay 20 mesas (filas) y 8 cafés máximo (columnas).
int tables[MAX_TABLES][MAX_COFFEES];

// La barra tiene un máximo de 8 cafés.
int bar[MAX_COFFEES];

//Contadores para el cuadre de caja.
int tablesCashBox[MAX_TABLES][COFFEE_ADDITION + 1];
int barCashBox[COFFEE_ADDITION + 1]; //En cada posición almaceno cuántas
									 //adiciones he vendido en la barra
									 //En la última posicion cuántos cafés

//Arreglo para el cuadre de caja general.
int globalCashBox[COFFEE_ADDITION + 1];

//Variable que contiene el café actual.
int currentCoffee;

//Arreglo global para almacenar un pedido generado por la función
//makeOrder().
int auxOrder[MAX_COFFEES];

//Variables utilizadas para calcular posición de un café.
//dada una mesa y el número del café, para luego acceder.
//en la matriz tables.
int tableNumber, coffeeNumber;

//Variable a modificar de acuerdo a la adición seleccionada.
int toppingSelected;

//Variables que indican adiciones (posición en la máscara).
const int WHIPPED_CREAM = 1 << 0;
const int CINNAMON		= 1 << 1;
const int CHOCOLATE		= 1 << 2;
const int AMARETTO		= 1 << 3;
const int IRISH_WHISKEY = 1 << 4;

//Variables que indican adiciones (posición en la máscara)
//almacenado en arreglo
const int toppingBits[COFFEE_ADDITION] = {WHIPPED_CREAM,
										  CINNAMON,
										  CHOCOLATE,
										  AMARETTO,
										  IRISH_WHISKEY};

//Arreglo para definir los nombres de las adiciones
const string toppingNames[COFFEE_ADDITION] = {"Whipped Cream",
											  "Cinnamon",
											  "Chocolate",
											  "Amaretto",
											  "Irish Whiskey"};

//Arreglo para definir los precios de las adiciones de los cafés
const string toppingPrices[COFFEE_ADDITION] = {"$0.89",
											   "$0.25",
											   "$0.59",
											   "$1.50",
											   "$1.75"};

//Arreglo para definir los valores de los precios de las adiciones de los cafés
const double toppingPricesValues[COFFEE_ADDITION] = {0.89,
													 0.25,
													 0.59,
													 1.50,
													 1.75};

//Flag para indicar si en el pedido que se está haciendo hay al menos
//un café
bool isOrderEmpty = true;

//Flag para saber si una orden fue cancelada cuando se estaban modificando
//las adiciones de un café
bool orderCanceled = false;

//Declaración de funciones
bool makeOrder();
void cancelOrder();
void addBarToCashbox();

//Permite confirmar y agregar al pedido el café que está en currentCoffee
void 
confirmCoffee() {

	int position;
	
	__asm{   
		MOV ECX, MAX_COFFEES      //Empieza el contador en MAX_COFFES (8).
		JMP insertCoffeeLoop

insertCoffeeLoop:
		MOV EAX, MAX_COFFEES
		SUB EAX, ECX		      //Se convierte ese valor de manera 
								  //que se itere ascendentemente.
								  // 8 - ECX (Número de iteración)
		MOV position, EAX
		MOV EAX, 4	              //Movemos el tamaño 4 Bytes (int) a EAX
		IMUL position			  //Multiplicamos los 4 Bytes por la
								  //posición candidata
		MOV EDX, EAX
		CMP auxOrder+([EDX]), -1  //Se comprueba si en el arreglo 
							      //auxiliar hay espacio disponible para 
								  //el café.
		JNE iterate				  //Si en esa posición no se puede
								  //almacenar, continúo iterando.
		JE insertCoffeeInPosition //Encontré espacio libre para poner

iterate:	
		LOOP insertCoffeeLoop
		JMP endInsertion

insertCoffeeInPosition:
		MOV EAX, currentCoffee
		MOV auxOrder+([EDX]), EAX //Se almacena el café en la 
								  //posición disponible
		JMP endInsertion

endInsertion:
		//Simplemente se usa para abandonar la inserción de cafés.
		MOV currentCoffee, 0
		NOP
	}
}

//Imprime las adiciones asociadas a un café, recibe el café representado.
void
printCoffeeDetails(int coffee) {
	bool printDetails;
	__asm {
		MOV printDetails, 0 //Suponemos que no se debe imprimir nada
		MOV EAX, coffee
		CMP EAX, -1         //Comparamos el café con -1
		JE endPrintDetails  //Si es -1 dejamos el printDetails en 0
		MOV printDetails, 1

endPrintDetails:
		NOP
	}
	if (printDetails) {
		int toppingAdded[COFFEE_ADDITION]; //Para almacenar si una adición
			    						   //fue agregada o no al pedido
		int position;
		__asm {
			MOV position, 0
			MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
										  //el arreglo
			IMUL position
			MOV EDX, EAX
			MOV EAX, coffee     
			AND EAX, WHIPPED_CREAM		  //Hacemos AND entre el current coffee
										  //y bit de Wipped Cream
			MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

			MOV position, 1
			MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
										  //el arreglo
			IMUL position
			MOV EDX, EAX
			MOV EAX, coffee     
			AND EAX, CINNAMON		      //Hacemos AND entre el current coffee
										  //y bit de Cinnamon
			MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

			MOV position, 2
			MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
										  //el arreglo
			IMUL position
			MOV EDX, EAX
			MOV EAX, coffee     
			AND EAX, CHOCOLATE		      //Hacemos AND entre el current coffee
										  //y bit de CHOCOLATE
			MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

			MOV position, 3
			MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
										  //el arreglo
			IMUL position
			MOV EDX, EAX
			MOV EAX, coffee     
			AND EAX, AMARETTO		      //Hacemos AND entre el current coffee
										  //y bit de AMARETTO
			MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

			MOV position, 4
			MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
										  //el arreglo
			IMUL position
			MOV EDX, EAX
			MOV EAX, coffee     
			AND EAX, IRISH_WHISKEY		  //Hacemos AND entre el current coffee
										  //y bit de IRISH_WHISKEY
			MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo
		}

		cout << "\tCoffee ($2.00)" << endl;
		for (int i = 0; i < COFFEE_ADDITION; i++) {
			if (toppingAdded[i]) {
				cout << "\t >> " << toppingNames[i] << " - (" 
					 << toppingPrices[i] << ")" << endl;
			}
		}
	}
}

//Retorna y puede imprimir el subtotal de un café de acuerdo a sus adiciones.
double
computeCoffeePrice(int coffee, bool printPrice) {
	double coffeePrice = 2.0;
	int positionDouble;
	int positionInt;
	int multFactor = 2;				//Usado para convertir de positionInt
									//a positionDouble, por ejemplo
									//positionInt debería ser 0, 4, 8, 12...
									//y positionDouble 0, 8, 16, 24...
	__asm {
		MOV ECX, COFFEE_ADDITION
		FLD coffeePrice
		JMP iterateToppings

iterateToppings:
		MOV positionInt, ECX
		DEC positionInt
		MOV EAX, 4					 //Movemos 4 Bytes a EAX para recorrer
		IMUL positionInt
		MOV positionInt, EAX		 //Almacenamos positonInt
		IMUL multFactor				 //Multiplicamos la posición por dos
									 //debido a que cada double ocupa 8
									 //Bytes.
		MOV positionDouble, EAX     //Guardamos el valor de positionDouble
		MOV EDX, positionInt		 //Recuperamos el valor de positionInt
									 //almacenandolo en EDX
		MOV EAX, coffee				 //Movemos el café actual a EAX

		AND EAX, toppingBits+([EDX]) //Hacemos and entre café actual
									 //y bits correspondientes a la
									 //adición
		CMP EAX, 0					 //Comparamos con 0 para saber si
									 //el AND dio diferente de 0,
									 //indicando que sí tiene la adición
		JNE addToppingPrice			 //Si tiene la adición vamos a sumar
		LOOP iterateToppings		 //Iteramos en las adiciones
		JMP endAddingCoffeePrices

addToppingPrice:
		MOV EDX, positionDouble      //Traemos el valor de positionDouble
									 //y lo almacenamos en EDX
									 //Sumamos el precio de la adición
		FADD toppingPricesValues+([EDX])
		LOOP iterateToppings
		JMP endAddingCoffeePrices

endAddingCoffeePrices:
		FSTP coffeePrice			 //Almacenamos el resultado en coffeePrice
	}
	if (printPrice) {
		cout << "======================" << endl;
		cout << "CoffeePrice: $" << coffeePrice << endl;
		cout << "======================" << endl;
	}
	return coffeePrice;
}

//Función para imprimir el subtotal, IVA y total de los cafés que hay
//actualmente en el arreglo auxiliar
double
printTotals() {
	double subtotalPrice = 0.0;
	double IVA = 0.1;
	double totalPrice = 0.0;
	__asm {
		FLD subtotalPrice
	}
	for (int i = 0; i < MAX_COFFEES; i++) {
		if (auxOrder[i] != -1) {
			//Mandamos false porque no queremos que imprima los precios,
			//sólo queremos obtener su valor
			double coffeePrice = computeCoffeePrice(auxOrder[i], false);
			__asm {
				FADD coffeePrice
			}
		}
	}
	__asm {
		FST subtotalPrice //Almacenamos el valor del subtotal
		FMUL IVA		  //Multiplicamos el subtotal por IVA
		FST IVA			  //Almacenamos valor del IVA
		FADD subtotalPrice//Sumamos el subtotal al IVA, para que me quede
						  //el total
		FSTP totalPrice
	}
	printf("El subtotal es: %.2f\n", subtotalPrice);
	printf("El IVA es: %.2f\n", IVA);
	printf("El total es: %.2f\n", totalPrice);
	return totalPrice;
}

//Método para imprimir cada uno de los cafés que hay en la orden,
//es decir, en el arreglo auxOrder.
void
printCoffeesInOrder() {
	for (int i = 0; i < MAX_COFFEES; i++) {
		int coffeeToPrint;
		int position;
		__asm {
			MOV EAX, i
			MOV position, EAX
			MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
										  //el arreglo
			IMUL position
			MOV EDX, EAX                  //Guardo la posición del café a
										  //a buscar en el arreglo
			MOV EAX, auxOrder+([EDX])     //Obtenemos el café asociado al
										  //índice i
			MOV coffeeToPrint, EAX        //Guardamos el café a imprimir en
									      //coffeeToPrint
		}
		bool printDetails;
		__asm {
			MOV printDetails, 0 //Suponemos que no se debe imprimir nada
			MOV EAX, coffeeToPrint
			CMP EAX, -1         //Comparamos el café con -1
			JE endPrintDetails  //Si es -1 dejamos el printDetails en 0
			MOV printDetails, 1

endPrintDetails:
			NOP
		}
		if (printDetails) { //Si el café no existe, no lo imprima
			cout << "Coffee #" << (i+1) << endl;
			cout << "=================" << endl;
			printCoffeeDetails(coffeeToPrint);
			//Enviamos true porque quiero que me imprima el precio
			//de una vez
			computeCoffeePrice(coffeeToPrint, true);
		}
	}
}

//De acuerdo al arreglo auxiliar permite al usuario ingresar un índice
//de un café para cancelarlo o editarlo.
int
selectCoffeeToCancelOrEdit (string cancelOrEditText) {
	printCoffeesInOrder();
	//Llamamos para imprimir el subtotal, IVA y total.
	printTotals();
	int option;
	do {
		cout << "Please, select coffee to " << cancelOrEditText 
			 << " OR press 0 to go back" << endl;
		cin >> option;
	} while (option < 0 || option > MAX_COFFEES || auxOrder[option - 1] == -1);
	return option - 1;
}

//Esta función se llamará al seleccionar la opción de cancelar el café actual.
void
cancelCurrentCoffee() {
	__asm {
		MOV currentCoffee, 0 //Se resetea el valor de currentCoffee a 0
	}
	//Esto ya debería volver al menú de agregar otro café, pagar o 
	//cancelar orden.
}

//En el menú después de confirmar un café, puedo llamar esta función,
//la cual permite cancelar cualquier café que se haya hecho en el pedido,
//esto es, recibiendo por teclado la posición del café que se desea
//cancelar.
void
cancelAnyCoffee(int coffeeSelected) {
	__asm {
		MOV EAX, 4	              //Movemos el tamaño 4 Bytes (int) a EAX
		IMUL coffeeSelected		  //Multiplicamos los 4 Bytes por la
								  //posición a cancelar recibida
		MOV auxOrder+([EAX]), -1
	}
	//Esto ya debería volver al menú de agregar otro café, pagar o 
	//cancelar orden.
}

void
flipTopping(int bitTopping) {
	__asm {
		MOV EAX, currentCoffee
		XOR EAX, bitTopping       //Se invierte el bit correspondiente
							      //a la adición
		MOV currentCoffee, EAX
	}
}

// Función para mostrar los detalles del café actual y permitir al usuario
// confirmarlo, cancelarlo o editarlo.
int
showCurrentCoffeeDetails() {
	cout << "Current coffee" << endl;
	cout << "==============" << endl;
	printCoffeeDetails(currentCoffee);
	//Enviamos true porque quiero que me imprima el precio
	//de una vez
	computeCoffeePrice(currentCoffee, true);
	cout << "--" << endl;
	cout << "0. Confirm coffee" << endl;
	cout << "1. Edit current coffee" << endl;
	cout << "8. Cancel current coffee" << endl;
	cout << "9. Cancel order" << endl;
	int option;
	cin >> option; 
	switch (option) {
		case 0: //Confirm coffee
			return 0;
			break;
		case 1: //Edit current coffee
			return 1;
		case 8: //Cancel current coffee
			return 8;
			break;
		case 9: //Cancel order
			return 9;
			break;
		default:
			return showCurrentCoffeeDetails();
			break;
	}
}

bool
showToppings() {
	__asm {
		MOV orderCanceled, 0 //Limpiar el flag de orden cancelada
	}

	int toppingAdded[COFFEE_ADDITION]; //Para almacenar si una adición
										//fue agregada o no al pedido
	int position;

	__asm {
		MOV position, 0
		MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
									  //el arreglo
		IMUL position
		MOV EDX, EAX
		MOV EAX, currentCoffee     
		AND EAX, WHIPPED_CREAM		  //Hacemos AND entre el current coffee
									  //y bit de Wipped Cream
		MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

		MOV position, 1
		MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
									  //el arreglo
		IMUL position
		MOV EDX, EAX
		MOV EAX, currentCoffee     
		AND EAX, CINNAMON		      //Hacemos AND entre el current coffee
									  //y bit de Cinnamon
		MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

		MOV position, 2
		MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
									  //el arreglo
		IMUL position
		MOV EDX, EAX
		MOV EAX, currentCoffee     
		AND EAX, CHOCOLATE		      //Hacemos AND entre el current coffee
									  //y bit de CHOCOLATE
		MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

		MOV position, 3
		MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
									  //el arreglo
		IMUL position
		MOV EDX, EAX
		MOV EAX, currentCoffee     
		AND EAX, AMARETTO		      //Hacemos AND entre el current coffee
									  //y bit de AMARETTO
		MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

		MOV position, 4
		MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
									  //el arreglo
		IMUL position
		MOV EDX, EAX
		MOV EAX, currentCoffee     
		AND EAX, IRISH_WHISKEY		  //Hacemos AND entre el current coffee
									  //y bit de IRISH_WHISKEY
		MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo
	}
	//Leer la adición que se quiere agregar
	int option;
	for (int i = 0; i < COFFEE_ADDITION; i++) {
		cout << (i+1) << ". " << toppingNames[i]
			 << " (" << toppingPrices[i] << ")"
			 << (toppingAdded[i] ? " Yes" : " No") << endl;
	}
	cout << "--" << endl;
	cout << "0. Continue" << endl;
	cout << "7. Cancel current coffee" << endl;
	cout << "8. Cancel order" << endl;
	//if (!isOrderEmpty) cout << "9. Cancel any coffee" << endl;
	cin >> option;

	bool showToppingsAgain;
	int optionSelectedWhenContinue;
	switch (option) {
		case 0:
			optionSelectedWhenContinue = showCurrentCoffeeDetails();
			if (optionSelectedWhenContinue == 0) {
				//showCurrentCoffeeDetails me devuelve que quiere confirmar
				//el café
				confirmCoffee();
				__asm {
					MOV orderCanceled, 0     //Apago flag de orden cancelado
					MOV showToppingsAgain, 0 //Apago flag de mostrar adiciones
				}
			}
			else if (optionSelectedWhenContinue == 1) {
				//showCurrentCoffeeDetails me devuelve que quiere editar
				//el café actual
				__asm {
					MOV orderCanceled, 0     //Apago flag de orden cancelado
					MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
				}
			}
			else if (optionSelectedWhenContinue == 8) {
				//showCurrentCoffeeDetails me devuelve que quiere cancelar
				//el café actual
				cancelCurrentCoffee();
				__asm {
					MOV orderCanceled, 0     //Apago flag de orden cancelado
					MOV showToppingsAgain, 0 //Apago flag de mostrar adiciones
				}
			}
			else if (optionSelectedWhenContinue == 9) {
				//showCurrentCoffeeDetails me devuelve que quiere cancelar la
				//orden
				cancelOrder();
				__asm {
					MOV orderCanceled, 1     //Prendo flag de orden cancelado
					MOV showToppingsAgain, 0 //Apago flag de mostrar adiciones
				}
			}
			break;
		case 1: //WHIPPED_CREAM
			flipTopping(WHIPPED_CREAM);
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
				MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
			}
			break;
		case 2: //CINNAMON
			flipTopping(CINNAMON);
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
				MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
			}
			break;
		case 3: //CHOCOLATE
			flipTopping(CHOCOLATE);
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
				MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
			}
			break;
		case 4: //AMARETTO
			flipTopping(AMARETTO);
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
				MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
			}
			break;
		case 5: //IRISH_WHISKEY
			flipTopping(IRISH_WHISKEY);
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
				MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
			}
			break;
		case 7: //Cancel current coffee
			cancelCurrentCoffee();
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
				MOV showToppingsAgain, 0 //Apago flag de mostrar adiciones
			}
			break;
		case 8: //Cancel order
			cancelOrder();
			__asm {
				MOV orderCanceled, 1     //Prendo flag de orden cancelado
				MOV showToppingsAgain, 0 //Apago flag de mostrar adiciones
			}
			break;
		default:
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
				MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
			}
			break;
	}
	return showToppingsAgain;
}

//Puede ser llamado en cualquiera de los menús al momento de estar haciendo
//la orden
void
cancelOrder() {
	int position;
	__asm {
		MOV currentCoffee, 0	  //Limpiar el café actual
		MOV ECX, MAX_COFFEES	  //Inicializar el contador para el ciclo
		JMP clearLoop

clearLoop:
		MOV EAX, MAX_COFFEES
		SUB EAX, ECX		      //Se convierte ese valor de manera 
								  //que se itere ascendentemente.
								  // 8 - ECX (Número de iteración)
		MOV position, EAX
		MOV EAX, 4	              //Movemos el tamaño 4 Bytes (int) a EAX
		IMUL position			  //Multiplicamos los 4 Bytes por la
								  //posición candidata
		MOV auxOrder+([EAX]), -1 //Limpiar la posición actual del vector
	}
}

//Recibe un índice para editar, el café que haya allí se mueve a currentCoffee,
//se espera que luego de este método se llame a showToppings()
void
editCoffee(int indexToEdit) {
	__asm {
		MOV EAX, 4	              //Movemos el tamaño 4 Bytes (int) a EAX
		IMUL indexToEdit	      //Multiplicamos los 4 Bytes por la
								  //posición a editar
		MOV EDX, EAX
		MOV EAX, auxOrder+([EDX])
		MOV currentCoffee, EAX	  //Limpiar la posición actual del
								  //vector
		MOV auxOrder+([EDX]), -1  //Limpiamos el café en esa posición
	}
}

//Método para terminar el pedido, mostrar resumen del mismo y leer
//el valor pagado por el cliente. Retornará true si se seleccionó back,
//de lo contrario, si se paga satisfactoriamente retorna false.
bool
finishAndPayOrder() {
	printCoffeesInOrder();
	//Almaceno el valor del total de la orden en valueToPay
	double valueToPay = printTotals();
	double amountPaid;
	do {
		cout << "Please, enter amount paid OR press 0 to go back" << endl;
		cin >> amountPaid;
	} while (amountPaid < 0);
	if (amountPaid == 0) return true; //Seleccioné back, entonces
									  //retorno que quiero volver
									  //a mostrar el menú anterior


	/* Generally, most programs test the condition code bits immediately
	 * after a comparison. Unfortunately, there are no conditional jump
	 * instructions that branch based on the FPU condition codes. Instead,
	 * you can use the fstsw instruction to copy the floating point status
	 * register (see "The FPU Status Register" on page 785) into the ax
	 * register; then you can use the sahf instruction to copy the ah
	 * register into the 80x86's condition code bits. After doing this,
	 * you can can use the conditional jump instructions to test some
	 * condition. This technique copies C0 into the carry flag, C2 into
	 * the parity flag, and C3 into the zero flag. The sahf instruction
	 * does not copy C1 into any of the 80x86's flag bits.

	 * Since the sahf instruction does not copy any 80x87 processor
	 * status bits into the sign or overflow flags, you cannot use the
	 * jg, jl, jge, or jle instructions. Instead, use the ja, jae, jb,
	 * jbe, je, and jz instructions when testing the results of a
	 * floating point comparison.
	 */
	int isAmountPaidEnough;
	double change;
	__asm {
		FLD valueToPay
		FCOMP amountPaid
		FSTSW AX
		SAHF
		JA notEnoughMoney		 //Si valueToPay > amountPaid entonces debe
								 //ingresar un valor mayor
		MOV isAmountPaidEnough, 1//Sí fue suficiente el valor pagado
		FSTP change				 //Limpiamos contenido de pila de flotantes
		FLD amountPaid			 //Guardamos en la pila el valor pagado
		FSUB valueToPay			 //Restamos el valor del pedido para hallar
								 //para hallar la diferencia
		FSTP change
		JMP endPayment

notEnoughMoney:
		MOV isAmountPaidEnough, 0
		JMP endPayment

endPayment:
		NOP
	}
	if (!isAmountPaidEnough) finishAndPayOrder(); //Vuelvo a mostrar el resumen
												  //del pedido y pido el valor
												  //a pagar.
	else {
		cout << "****************************" << endl;
		cout <<"*********COFFEE STAR********" << endl;
		cout << "****************************" << endl;
		cout <<"*********BILL OF SALE*******" << endl;
		cout << "****************************" << endl;
		cout << "============================" << endl;
		if (isTableOrder) cout << "Type of sale: Table service." << endl;
		else cout << "Type of sale: Bar service." << endl;
		cout << "============================" << endl;
		cout << "============================" << endl;
		printCoffeesInOrder();
		printTotals();
		cout << "============================" << endl;
		cout << "============================" << endl;
		printf("Amount paid: %.2f\n", amountPaid);
		printf("Change: %.2f\n", change);
		cout << "============================" << endl;
		cout << "Thanks for visiting us" << endl;
		cout << "We hope to see you soon" << endl;
	}
	//TODO Llamar método que me compute los datos para el cashbox
	addBarToCashbox();
	cancelOrder(); //Llamamos el método cancelar orden para limpiar el
				   //arreglo auxiliar.
	return false; //Retornamos false porque no queremos volver
				  //a mostrar el menú.
}

bool
makeOrder() {
	__asm {
		MOV isOrderEmpty, 1 //Supongo que la orden está vacía
	}

	int position;

	__asm {
		MOV ECX, MAX_COFFEES
		JMP checkEmptiness

checkEmptiness:
		MOV EAX, MAX_COFFEES     
		SUB EAX, ECX		        //Se convierte ese valor de manera 
								    //que se itere ascendentemente.
								    // 8 - ECX (Número de iteración)
		MOV position, EAX
		MOV EAX, 4	                //Movemos el tamaño 4 Bytes (int) a EAX
		IMUL position			    //Multiplicamos los 4 Bytes por la
								    //posición candidata
		CMP auxOrder+([EAX]), -1    //Comparamos el contenido del
									//arreglo auxOrder en la posición
									//indicada con -1.
		JE continueChecking			//Mientras las posiciones sean iguales a
									//-1 sigue iterando.
		MOV isOrderEmpty, 0			//Se encontró al menos un café en el
									//pedido.
		JMP endChecking
		

continueChecking:
		LOOP checkEmptiness
		JMP endChecking

endChecking:
		NOP
	}

	int option;
	if (!isOrderEmpty) {
		cout << "0. Finish and pay order" << endl;
	}
	cout << "1. Add Coffee" << endl;
	if (!isOrderEmpty) {
		cout << "2. Edit a coffee" << endl;
		cout << "3. Remove a coffee" << endl;
	}
	cout << "9. Cancel order" << endl;

	cin  >> option;
	
	bool continueMakingOrder;
	int auxCancelOrder;
	int indexToCancelOrEdit;
	switch (option) {
		case 0:
			if (finishAndPayOrder()) {
				__asm {
					MOV continueMakingOrder, 1 //Volvemos a mostrar el menú
				}
			}
			else {
				__asm {
					MOV continueMakingOrder, 0 //No volvemos a mostrar el menú
											   //porque ya se pagó
											   //correctamente.
				}
			}
			break;
		case 1:
			while (continueMakingOrder = showToppings());
			auxCancelOrder = orderCanceled;
			__asm {
				MOV continueMakingOrder, 1 //Suponemos que debo mostrar el menú
										   //de hacer orden
				MOV EAX, auxCancelOrder
				CMP EAX, 0                 //Revisamos si cancelé la order
				JE endAddCoffee            //Se confirmó un café y no se
										   //canceló la orden
				MOV continueMakingOrder, 0 //Cancelé la orden entonces debo
									       //retornar al menú inicial

endAddCoffee:
				NOP
			}
			break;
		case 2:
			indexToCancelOrEdit = selectCoffeeToCancelOrEdit("edit");
			if (indexToCancelOrEdit == -1) {
				//Si seleccioné 'go back' entonces
				//vuelvo a mostrar el menú
				__asm {
					MOV continueMakingOrder, 1 //Volvemos a mostrar el menú
				}
				break;
			}
			editCoffee(indexToCancelOrEdit);
			while (continueMakingOrder = showToppings());
			auxCancelOrder = orderCanceled;
			__asm {
				MOV continueMakingOrder, 1 //Suponemos que debo mostrar el menú
										   //de hacer orden
				MOV EAX, auxCancelOrder
				CMP EAX, 0                 //Revisamos si cancelé la order
				JE endEditCoffee           //Se confirmó un café y no se
										   //canceló la orden
				MOV continueMakingOrder, 0 //Cancelé la orden entonces debo
									       //retornar al menú inicial

endEditCoffee:
				NOP
			}
			break;
		case 3:
			if (isOrderEmpty) {
				__asm {
					MOV continueMakingOrder, 1 //Volvemos a mostrar el menú
				}
				break;
			}
			indexToCancelOrEdit = selectCoffeeToCancelOrEdit("remove");
			if (indexToCancelOrEdit == -1) {
				//Si seleccioné 'go back' entonces
				//vuelvo a mostrar el menú
				__asm {
					MOV continueMakingOrder, 1 //Volvemos a mostrar el menú
				}
				break;
			}
			cancelAnyCoffee(indexToCancelOrEdit);
			continueMakingOrder = true;
			break;
		case 9:
			cancelOrder();
			__asm {
				MOV continueMakingOrder, 0 //Se canceló la orden y se debe
										   //volver al menú inicial
			}
			break;
		default:
			__asm {
				MOV continueMakingOrder, 1 //Volvemos a mostrar el menú
			}
			break;
	}
	return continueMakingOrder;
}

void
handleBar() {
	//Asigna al pedido de la barra lo que hay en el arreglo auxiliar.
	//Se utiiza auxiliar porque se quiere generalizar makeOrder
	int position;
	__asm {
		MOV ECX, MAX_COFFEES
		JMP copyArray

copyArray:
		MOV EAX, MAX_COFFEES     
		SUB EAX, ECX		        //Se convierte ese valor de manera 
								    //que se itere ascendentemente.
								    // 8 - ECX (Número de iteración)
		MOV position, EAX
		MOV EAX, 4	                //Movemos el tamaño 4 Bytes (int) a EAX
		IMUL position			    //Multiplicamos los 4 Bytes por la
								    //posición candidata
		MOV EDX, auxOrder+([EAX])   //Movemos a EDX el contenido del
									//arreglo auxOrder en la posición
									//indicada
		MOV bar+([EAX]), EDX		//Movemos al arreglo de la barra el
									//contenido que había en el auxiliar
		LOOP copyArray
	}

	cout << "Los cafes quedaron:" << endl;
	for (int i = 0; i < MAX_COFFEES; i++) {
		cout << bar[i] << " | ";
	}
	cout << endl;
}

int 
selectTable(){
	int option;
	do{
		cout << "Select Table" << endl;
		cin >> option;
	}
	while (option <= 0 || option > 20);
	return option - 1;

}

void
barMenu(){
	//Se limpia el arreglo auxiliar para pedidos.
	for (int i = 0; i < MAX_COFFEES; i++) {
		auxOrder[i] = -1; //Se hace para luego saber qué cafés existen
						  //en el pedido, y cuáles no.
	}

	while (makeOrder());
}

void 
manageOrder() {
	int option;
	cout << "1. Table" << endl;
	cout << "2. Bar" << endl;
	cout << "9. Back" << endl;

	cin >> option;
	int tableSelected;
	switch (option) {
		case 1:
			isTableOrder = true;
			tableSelected = selectTable(); // Se sabe que mesa se desea atender.
			// tableMenu(tableSelected);
			break;
		case 2:
			isTableOrder = false;
			barMenu();
			break;
		case 9:
			break;
		default:
			manageOrder();
			break;

	}
}

//Toma el arreglo auxiliar y lo suma al cuadre de caja de la barra
void addBarToCashbox() {
	for (int i = 0; i < MAX_COFFEES; i++) {
		int coffee;
		int analyzeCoffee;
		__asm {
			MOV EAX, 4
			IMUL i
			MOV EDX, EAX
			
			MOV EAX, auxOrder+([EDX])
			MOV coffee, EAX
			CMP EAX, -1
			JE dontAnalyze
			MOV analyzeCoffee, 1
			JMP endAnalyzing

dontAnalyze:
			MOV analyzeCoffee, 0
			JMP endAnalyzing

endAnalyzing:
			NOP
		}
		if (analyzeCoffee) {
			int position;
			int coffeesCountPosition = 5 * 4;
			__asm {
				MOV ECX, COFFEE_ADDITION
				JMP iterateToppings2

iterateToppings2:
				MOV position, ECX
				DEC position
				MOV EAX, 4					//Movemos 4 Bytes a EAX para recorrer
				IMUL position
				MOV position, EAX			//Almacenamos positonInt
				MOV EDX, position		 //Recuperamos el valor de positionInt
											 //almacenandolo en EDX
				MOV EAX, coffee				 //Movemos el café actual a EAX

				AND EAX, toppingBits+([EDX]) //Hacemos and entre café actual
											 //y bits correspondientes a la
											 //adición
				CMP EAX, 0					 //Comparamos con 0 para saber si
											 //el AND dio diferente de 0,
											 //indicando que sí tiene la adición
				JNE addCashBoxCount			 //Si tiene la adición vamos a sumar
				LOOP iterateToppings2		 //Iteramos en las adiciones
				JMP endAddingCashBoxCount

addCashBoxCount:
				MOV EAX, barCashBox+([EDX]) //Aumentamos contador de adiciones
				INC EAX
				MOV barCashBox+([EDX]), EAX
				LOOP iterateToppings2
				JMP endAddingCashBoxCount

endAddingCashBoxCount:
				MOV EDX, coffeesCountPosition 
				MOV EAX, barCashBox+([EDX])  //Aumentamos uno al contador de
											 //cafés
				INC EAX
				MOV barCashBox+([EDX]), EAX
				NOP
			}
		}
	}
}

void
checkCashboxTable() {
	double tableCoffeesPrice = 0;
	double coffeePrice = 2.00;
	int coffeesQuantity;

	cout << "===========================" << endl;
	cout << "      Table service" << endl;
	for (int k = 0; k < MAX_TABLES; k++) {
		__asm {
			MOV EAX, 4					//Se calcula posición por la fila
			IMUL k						//Multiplicamos por la fila actual
			MOV EDX, EAX				//Almacenamos resultado en EDX
			MOV EAX, 4					//Volvemos a mover 4 Bytes para ya
										//hacer las columnas
			IMUL COFFEE_ADDITION		//multiplicamos la última columna
										//que es donde están los cafés que
										//se vendieron en la mesa k
			ADD EAX, EDX				//sumamos el resultado al que ya
										//teníamos para obtener el resultado
										//exacto de la posición en el arreglo
			MOV EDX, EAX
			MOV EAX, tablesCashBox+([EDX])
			MOV coffeesQuantity, EAX
			FILD coffeesQuantity
			FMUL coffeePrice
			FSTP tableCoffeesPrice
		}
		cout << "-------------------------" << endl;
		cout << "      Table # " << (k+1) << endl;
		cout << "------------------------" << endl;
		cout << "* Coffees sold: " << coffeesQuantity << endl;
		cout << "* > Unit price: $2.00" << endl;
		//cout << "* > Total price: " << barCoffeesPrice << endl;
		printf("* > Total price: $%.2f\n", tableCoffeesPrice);
		for (int i = 0; i < COFFEE_ADDITION; i++) {
			double toppingTotalPrice;
			int toppingsQuantity;
			int toppingsPricePosition;
			__asm {
				MOV EAX, 4					//Se calcula posición por la fila
				IMUL k						//Multiplicamos por la fila actual
				MOV EDX, EAX				//Almacenamos resultado en EDX
				MOV EAX, 4
				IMUL i
				MOV toppingsPricePosition, EAX //Almacenamos la posición de la
											//columna para ir por el precio
											//después
				ADD EAX, EDX				//sumamos el resultado al que ya
											//teníamos para obtener el resultado
											//exacto de la posición en el arreglo
				MOV EDX, EAX				//Movemos el resultado
				MOV EAX, tablesCashBox+([EDX])
				MOV toppingsQuantity, EAX
				FILD toppingsQuantity
				MOV EAX, 2
				IMUL toppingsPricePosition
				MOV EDX, EAX
				FMUL toppingPricesValues+([EDX])
				FSTP toppingTotalPrice
			}
			printf("------------------------\n");
			printf("* %s units sold: %d\n", toppingNames[i].c_str(), toppingsQuantity);
			printf("* > Unit price: %s\n", toppingPrices[i].c_str());
			printf("* > Total price: $%.2f\n", toppingTotalPrice);
		}
		cout << endl;
	}
}

void
checkCashboxBar() {
	double barCoffeesPrice = 0;
	double coffeePrice = 2.00;
	int coffeesQuantity;

	__asm {
		MOV EAX, 4
		IMUL COFFEE_ADDITION
		MOV EDX, EAX
		MOV EAX, barCashBox+([EDX])
		MOV coffeesQuantity, EAX
		FILD coffeesQuantity
		FMUL coffeePrice
		FSTP barCoffeesPrice
	}
	cout << "------------------------" << endl;
	cout << "Bar service" << endl;
	cout << "------------------------" << endl;
	cout << "* Coffees sold: " << coffeesQuantity << endl;
	cout << "* > Unit price: $2.00" << endl;
	//cout << "* > Total price: " << barCoffeesPrice << endl;
	printf("* > Total price: $%.2f\n", barCoffeesPrice);
	for (int i = 0; i < COFFEE_ADDITION; i++) {
		double toppingTotalPrice;
		int toppingsQuantity;
		__asm {
			MOV EAX, 4
			IMUL i
			MOV EDX, EAX
			MOV EAX, barCashBox+([EDX])
			MOV toppingsQuantity, EAX
			FILD toppingsQuantity
			MOV EAX, 2
			IMUL EDX
			MOV EDX, EAX
			FMUL toppingPricesValues+([EDX])
			FSTP toppingTotalPrice
		}
		printf("------------------------\n");
		printf("* %s units sold: %d\n", toppingNames[i].c_str(), toppingsQuantity);
		printf("* > Unit price: %s\n", toppingPrices[i].c_str());
		printf("* > Total price: $%.2f\n", toppingTotalPrice);
	}
	cout << endl;
}

void
checkCashbox() {
	checkCashboxBar();
	checkCashboxTable();
}

void
clearTables(bool clearAll, int tableNumber) { 
	//Si el booleano está en true, entonces se procede a limpiar 
	//todas las mesas.
	if (clearAll) {
		for (int i = 0; i < MAX_TABLES; i++) {
			for (int j = 0; j < MAX_COFFEES; j++) {
				tables[i][j] = 0;
			}
		}
	}
	else {
		//Si el booleano está en false, entonces se procede a limpiar 
		//sólo la mesa indicada.
		for (int j = 0; j < MAX_COFFEES; j++) {
			tables[tableNumber][j] = 0;
		}
	}
}

void
clearCashbox() {
	for (int i = 0; i < MAX_COFFEES + 1; i++) {
		barCashBox[i] = 0;
	}
	for (int i = 0; i < MAX_TABLES; i++) {
		for (int j = 0; j < MAX_COFFEES + 1; j++) {
			tablesCashBox[i][j] = 0;
		}
	}
}

void
clearBar() {
	for (int i = 0; i < MAX_COFFEES; i++) bar[i] = 0;
}

bool
menu() {
	int option;
	cout << "1. Manage Order"<< endl;
	cout << "2. Check Cashbox" << endl;
	cout << "3. Quit" << endl;
	cin >> option;
	switch (option) {
		case 1:
			manageOrder();
			return true;
			break;
		case 2:
			checkCashbox();
			return true;
			break;
		case 3:
			cout << "See you tomorrow" << endl;
			return false;
			break;
		default:
			return true;
			break;
	}
}

int
_tmain(int argc, _TCHAR* argv[]) {
	clearTables(true, -1); //Limpiamos todas las mesas.
	clearCashbox();		   //Limpiamos datos de cuadre de caja
	clearBar();			   //Limpiamos la barra.

	while (menu());
	system("pause");
	return 0;
}
