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
int barCashBox[COFFEE_ADDITION + 1];

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

//Flag para indicar si en el pedido que se está haciendo hay al menos
//un café
bool isOrderEmpty = true;

//Flag para saber si una orden fue cancelada cuando se estaban modificando
//las adiciones de un café
bool orderCanceled = false;

//Declaración de funciones
bool makeOrder();
void cancelOrder();

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

//De acuerdo al arreglo auxiliar permite al usuario ingresar un índice
//de un café para cancelarlo.
int
selectCoffeeToCancel () {
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
		if (printDetails) {
			cout << "Coffee #" << (i+1) << endl;
			cout << "=================" << endl;
			printCoffeeDetails(coffeeToPrint);
		}
	}
	int option;
	do {
		cout << "Please, select coffee to cancel:" << endl;
		cin >> option;
	} while (option <= 0 || option > MAX_COFFEES || auxOrder[option - 1] == -1);
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
		cout << (i+1) << ". " << toppingNames[i] << 
			(toppingAdded[i] ? " Yes" : " No") << endl;
	}
	cout << "--" << endl;
	cout << "0. Confirm coffee" << endl;
	cout << "7. Cancel current coffee" << endl;
	cout << "8. Cancel order" << endl;
	if (!isOrderEmpty) cout << "9. Cancel any coffee" << endl;
	cin >> option;

	bool showToppingsAgain;
	int indexToCancel;
	switch (option) {
		case 0:
			confirmCoffee();
			cout << "Auxiliar" << endl;
			for (int i = 0; i < MAX_COFFEES; i++) {
				cout << auxOrder[i] << " | ";
			}
			cout << endl;
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
				MOV showToppingsAgain, 0 //Apago flag de mostrar adiciones
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
		case 9: //Cancel any coffee
			indexToCancel = selectCoffeeToCancel();
			cancelAnyCoffee(indexToCancel);
			__asm {
				MOV orderCanceled, 0     //Apago flag de orden cancelado
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
	do{
		cout << "1. Add Coffee" << endl;
		cout << "2. Cancel Order" << endl;
		if (!isOrderEmpty) {
			cout << "3. Remove Coffee" << endl;
		}
		cin  >> option;
	}
	while (option <= 0 || option > (3- isOrderEmpty));
	
	bool continueMakingOrder;
	int auxCancelOrder;
	int indexToCancel;
	switch (option) {
		case 1:
			while (continueMakingOrder = showToppings());
			auxCancelOrder = orderCanceled;
			__asm {
				MOV continueMakingOrder, 1 //Suponemos que debo mostrar el menú
										   //de hacer orden
				MOV EAX, auxCancelOrder
				CMP EAX, 0                //Revisamos si cancelé la order
				JE endContinueMakingOrder //Se confirmó un café y no se
										   //canceló la orden
				MOV continueMakingOrder, 0 //Cancelé la orden entonces debo
									       //retornar al menú inicial

endContinueMakingOrder:
				NOP
			}
			break;
		case 2:
			cancelOrder();
			__asm {
				MOV continueMakingOrder, 0 //Se canceló la orden y se debe
										   //volver al menú inicial
			}
			break;
		case 3:
			indexToCancel = selectCoffeeToCancel();
			cancelAnyCoffee(indexToCancel);
			continueMakingOrder = true;
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
	do {
		cout << "1. Table" << endl;
		cout << "2. Bar" << endl;
		cin >> option;
	}
	while (option <= 0 || option > 2);
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
	}
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
clearBar() {
	for (int i = 0; i < MAX_COFFEES; i++) bar[i] = 0;
}

bool
menu() {
	int option;
	do {
		cout << "1. Manage Order"<< endl;
		cout << "2. Check Cashbox" << endl;
		cout << "3. Quit" << endl;
		cin >> option;
	}
	while (option <= 0 || option > 3);
	switch (option) {
		case 1:
			manageOrder();
			return true;
			break;
		case 2:
			// cashbox
			return true;
			break;
		case 3:
			cout << "See you tomorrow" << endl;
			return false;
			break;
	}
}

int
_tmain(int argc, _TCHAR* argv[]) {
	clearTables(true, -1); //Limpiamos todas las mesas.
	clearBar(); //Limpiamos la barra.

	while (menu());

	//system("pause");
	return 0;
}

