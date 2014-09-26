// CoffeeStar 2.0.cpp: define el punto de entrada de la aplicaci�n de consola.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <sstream>

using namespace std;

const int MAX_TABLES = 20;
const int MAX_COFFEES = 8;
const int COFFEE_ADDITION = 5;

// Variable para saber si estamos haciendo una orden en una mesa o en la barra.
bool isTableOrder;
// Hay 20 mesas (filas) y 8 caf�s m�ximo (columnas).
int tables[MAX_TABLES][MAX_COFFEES];
bool tablesStatus[MAX_TABLES];

// La barra tiene un m�ximo de 8 caf�s.
int bar[MAX_COFFEES];

//Contadores para el cuadre de caja.
int tablesCashBox[MAX_TABLES][COFFEE_ADDITION + 1];
int barCashBox[COFFEE_ADDITION + 1]; //En cada posici�n almaceno cu�ntas
                                     //adiciones he vendido en la barra
                                     //En la �ltima posicion cu�ntos caf�s

//Arreglo para el cuadre de caja general.
int globalCashBox[COFFEE_ADDITION + 1];

//Variable que contiene el caf� actual.
int currentCoffee;

//Variable para almacenar el id de la mesa que se est� atendiendo actualmente
int currentTableId;

//Arreglo global para almacenar un pedido generado por la funci�n
//makeOrder().
int auxOrder[MAX_COFFEES];

//Variables utilizadas para calcular posici�n de un caf�.
//dada una mesa y el n�mero del caf�, para luego acceder.
//en la matriz tables.
int tableNumber, coffeeNumber;

//Variable a modificar de acuerdo a la adici�n seleccionada.
int toppingSelected;

//Variables que indican adiciones (posici�n en la m�scara).
const int WHIPPED_CREAM = 1 << 0;
const int CINNAMON      = 1 << 1;
const int CHOCOLATE     = 1 << 2;
const int AMARETTO      = 1 << 3;
const int IRISH_WHISKEY = 1 << 4;

//Variables que indican adiciones (posici�n en la m�scara)
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

//Arreglo para definir los precios de las adiciones de los caf�s
const string toppingPrices[COFFEE_ADDITION] = {"$0.89",
                                               "$0.25",
                                               "$0.59",
                                               "$1.50",
                                               "$1.75"};

//Arreglo para definir los valores de los precios de las adiciones de los caf�s
const double toppingPricesValues[COFFEE_ADDITION] = {0.89,
                                                     0.25,
                                                     0.59,
                                                     1.50,
                                                     1.75};

//Flag para indicar si en el pedido que se est� haciendo hay al menos
//un caf�
bool isOrderEmpty = true;
bool isOrderFull = false;

//Flag para saber si una orden fue cancelada cuando se estaban modificando
//las adiciones de un caf�
bool orderCanceled = false;

//En esta variable almacenamos el valor de la propina
double tipValue;
bool tipIncluded = false;
bool tipAsked = false;

//Declaraci�n de funciones
bool makeOrder();
void cancelOrder();
void addBarToCashbox();
void addTableToCashBox(int tableIndex);

//Enumeraci�n para seleccionar los colores de impresi�n
enum color {GRAY, BLUE, GREEN, CYAN, RED, PURPLE, YELLOW, WHITE};

//Funci�n para cambiar el color de impresi�n en la consola de Windows
void
setConsoleColor(int colorId) {
    colorId += 8; //Para quedar en el id del color correcto
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorId);
}

//Template para convertir lo recibido a entero
template <class T> int toInt(const T &x) {
    stringstream s;
    s << x;
    int r;
    s >> r;
    return r;
}

//Template para convertir lo recibido a double
template <class T> double toDouble(const T &x) {
    stringstream s;
    s << x;
    double r;
    s >> r;
    return r;
}

//Funci�n para comprobar que un string recibido sea un n�mero entero
bool
isANum(string s) {
    for (int i = 0; i < s.size(); i++) if (!isdigit(s[i])) return false;
    return true;
}

//Funci�n para comprobar que un string recibido sea un n�mero de punto flotante
bool
isDouble(string s) {
    istringstream ss(s);
    double d;
    return (ss >> d) && (ss >> std::ws).eof();
}

//Permite confirmar y agregar al pedido el caf� que est� en currentCoffee
void 
confirmCoffee() {

    int position;
    
    __asm{   
        MOV ECX, MAX_COFFEES      //Empieza el contador en MAX_COFFES (8).
        JMP insertCoffeeLoop

insertCoffeeLoop:
        MOV EAX, MAX_COFFEES
        SUB EAX, ECX              //Se convierte ese valor de manera 
                                  //que se itere ascendentemente.
                                  // 8 - ECX (N�mero de iteraci�n)
        MOV position, EAX
        MOV EAX, 4                //Movemos el tama�o 4 Bytes (int) a EAX
        IMUL position             //Multiplicamos los 4 Bytes por la
                                  //posici�n candidata
        MOV EDX, EAX
        CMP auxOrder+([EDX]), -1  //Se comprueba si en el arreglo 
                                  //auxiliar hay espacio disponible para 
                                  //el caf�.
        JNE iterate               //Si en esa posici�n no se puede
                                  //almacenar, contin�o iterando.
        JE insertCoffeeInPosition //Encontr� espacio libre para poner

iterate:    
        LOOP insertCoffeeLoop
        JMP endInsertion

insertCoffeeInPosition:
        MOV EAX, currentCoffee
        MOV auxOrder+([EDX]), EAX //Se almacena el caf� en la 
                                  //posici�n disponible
        JMP endInsertion

endInsertion:
        //Simplemente se usa para abandonar la inserci�n de caf�s.
        MOV currentCoffee, 0
        NOP
    }
}

//Imprime las adiciones asociadas a un caf�, recibe el caf� representado.
void
printCoffeeDetails(int coffee) {
    bool printDetails;
    __asm {
        MOV printDetails, 0 //Suponemos que no se debe imprimir nada
        MOV EAX, coffee
        CMP EAX, -1         //Comparamos el caf� con -1
        JE endPrintDetails  //Si es -1 dejamos el printDetails en 0
        MOV printDetails, 1

endPrintDetails:
        NOP
    }
    if (printDetails) {
        int toppingAdded[COFFEE_ADDITION]; //Para almacenar si una adici�n
                                           //fue agregada o no al pedido
        int position;
        __asm {
            MOV position, 0
            MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                          //el arreglo
            IMUL position
            MOV EDX, EAX
            MOV EAX, coffee     
            AND EAX, WHIPPED_CREAM        //Hacemos AND entre el current coffee
                                          //y bit de Wipped Cream
            MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

            MOV position, 1
            MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                          //el arreglo
            IMUL position
            MOV EDX, EAX
            MOV EAX, coffee     
            AND EAX, CINNAMON             //Hacemos AND entre el current coffee
                                          //y bit de Cinnamon
            MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

            MOV position, 2
            MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                          //el arreglo
            IMUL position
            MOV EDX, EAX
            MOV EAX, coffee     
            AND EAX, CHOCOLATE            //Hacemos AND entre el current coffee
                                          //y bit de CHOCOLATE
            MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

            MOV position, 3
            MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                          //el arreglo
            IMUL position
            MOV EDX, EAX
            MOV EAX, coffee     
            AND EAX, AMARETTO             //Hacemos AND entre el current coffee
                                          //y bit de AMARETTO
            MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

            MOV position, 4
            MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                          //el arreglo
            IMUL position
            MOV EDX, EAX
            MOV EAX, coffee     
            AND EAX, IRISH_WHISKEY        //Hacemos AND entre el current coffee
                                          //y bit de IRISH_WHISKEY
            MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo
        }

        printf("***************************************\n");
        printf("*     Coffee                  ");
        setConsoleColor(GREEN);
        printf("($2.00)");
        setConsoleColor(WHITE);
        printf(" *\n");
        for (int i = 0; i < COFFEE_ADDITION; i++) {
            if (toppingAdded[i]) {
                printf("*       >> %-19s", toppingNames[i].c_str());
                setConsoleColor(GREEN);
                printf("(%s)", toppingPrices[i].c_str());
                setConsoleColor(WHITE);
                printf(" *\n");
            }
        }
    }
}

//Retorna y puede imprimir el subtotal de un caf� de acuerdo a sus adiciones.
double
computeCoffeePrice(int coffee, bool printPrice) {
    double coffeePrice = 2.0;
    int positionDouble;
    int positionInt;
    int multFactor = 2;             //Usado para convertir de positionInt
                                    //a positionDouble, por ejemplo
                                    //positionInt deber�a ser 0, 4, 8, 12...
                                    //y positionDouble 0, 8, 16, 24...
    __asm {
        MOV ECX, COFFEE_ADDITION
        FLD coffeePrice
        JMP iterateToppings

iterateToppings:
        MOV positionInt, ECX
        DEC positionInt
        MOV EAX, 4                   //Movemos 4 Bytes a EAX para recorrer
        IMUL positionInt
        MOV positionInt, EAX         //Almacenamos positonInt
        IMUL multFactor              //Multiplicamos la posici�n por dos
                                     //debido a que cada double ocupa 8
                                     //Bytes.
        MOV positionDouble, EAX     //Guardamos el valor de positionDouble
        MOV EDX, positionInt         //Recuperamos el valor de positionInt
                                     //almacenandolo en EDX
        MOV EAX, coffee              //Movemos el caf� actual a EAX

        AND EAX, toppingBits+([EDX]) //Hacemos and entre caf� actual
                                     //y bits correspondientes a la
                                     //adici�n
        CMP EAX, 0                   //Comparamos con 0 para saber si
                                     //el AND dio diferente de 0,
                                     //indicando que s� tiene la adici�n
        JNE addToppingPrice          //Si tiene la adici�n vamos a sumar
        LOOP iterateToppings         //Iteramos en las adiciones
        JMP endAddingCoffeePrices

addToppingPrice:
        MOV EDX, positionDouble      //Traemos el valor de positionDouble
                                     //y lo almacenamos en EDX
                                     //Sumamos el precio de la adici�n
        FADD toppingPricesValues+([EDX])
        LOOP iterateToppings
        JMP endAddingCoffeePrices

endAddingCoffeePrices:
        FSTP coffeePrice             //Almacenamos el resultado en coffeePrice
    }
    if (printPrice) {
        printf("***************************************\n");
        printf("* CoffeePrice: $ %.2f                 *\n", coffeePrice);
        printf("***************************************\n");
    }
    return coffeePrice;
}

//Funci�n para imprimir el subtotal, IVA y total de los caf�s que hay
//actualmente en el arreglo auxiliar
double
printTotals() {
    double ivaAndTipPercent = 0.1;
    double subtotalPrice = 0.0;
    double IVA = 0.0;
    double totalPrice = 0.0;
    __asm {
        FLD subtotalPrice
    }
    for (int i = 0; i < MAX_COFFEES; i++) {
        if (auxOrder[i] != -1) {
            //Mandamos false porque no queremos que imprima los precios,
            //s�lo queremos obtener su valor
            double coffeePrice = computeCoffeePrice(auxOrder[i], false);
            __asm {
                FADD coffeePrice
            }
        }
    }
    __asm {
        FST subtotalPrice //Almacenamos el valor del subtotal
        FMUL ivaAndTipPercent//Multiplicamos el subtotal por IVA
        FST IVA           //Almacenamos valor del IVA
        FADD subtotalPrice//Sumamos el subtotal al IVA, para que me quede
                          //el total
        FST totalPrice
        FMUL ivaAndTipPercent
        FSTP tipValue
    }
    printf("* Subtotal                   $%7.2f *\n", subtotalPrice);
    printf("* IVA (10 %%)                 $%7.2f *\n", IVA);
    printf("***************************************\n");
    printf("* ");
    if (!isTableOrder || (isTableOrder && !tipIncluded)) {
        setConsoleColor(YELLOW);
    }
    printf("TOTAL                      $%7.2f ", totalPrice);
    setConsoleColor(WHITE);
    printf("*\n");
    printf("***************************************\n");
    if (isTableOrder) printf("* Voluntary tip %-15s$%5.2f *\n", 
        tipIncluded ? "(Included)" : "(Not included)", tipValue);
    if (isTableOrder) {
        if (tipIncluded) {
            //Si la propina est� inclu�da, entonces la sumamos al total
            __asm {
                FLD totalPrice
                FADD tipValue
                FSTP totalPrice
            }
        }
        printf("***************************************\n");
        printf("* ");
        if (tipIncluded) {
            setConsoleColor(YELLOW);
        }
        printf("TOTAL                      $%7.2f ", totalPrice);
        setConsoleColor(WHITE);
        printf("*\n");
        printf("***************************************\n");
    }
    return totalPrice;
}

//M�todo para imprimir cada uno de los caf�s que hay en la orden,
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
            MOV EDX, EAX                  //Guardo la posici�n del caf� a
                                          //a buscar en el arreglo
            MOV EAX, auxOrder+([EDX])     //Obtenemos el caf� asociado al
                                          //�ndice i
            MOV coffeeToPrint, EAX        //Guardamos el caf� a imprimir en
                                          //coffeeToPrint
        }
        bool printDetails;
        __asm {
            MOV printDetails, 0 //Suponemos que no se debe imprimir nada
            MOV EAX, coffeeToPrint
            CMP EAX, -1         //Comparamos el caf� con -1
            JE endPrintDetails  //Si es -1 dejamos el printDetails en 0
            MOV printDetails, 1

endPrintDetails:
            NOP
        }
        if (printDetails) { //Si el caf� no existe, no lo imprima
            printf("***************************************\n");
            printf("*            ");
            setConsoleColor(PURPLE);
            printf("Coffee # %d               ", (i+1));
            setConsoleColor(WHITE);
            printf("*\n");
            printCoffeeDetails(coffeeToPrint);
            //Enviamos true porque quiero que me imprima el precio
            //de una vez
            computeCoffeePrice(coffeeToPrint, true);
        }
    }
}

//De acuerdo al arreglo auxiliar permite al usuario ingresar un �ndice
//de un caf� para cancelarlo o editarlo.
int
selectCoffeeToCancelOrEdit (string cancelOrEditText) {
    printCoffeesInOrder();
    //Llamamos para imprimir el subtotal, IVA y total.
    printTotals();
    string optionS;
    int option;
    do {
        printf("*****************************************************\n");
        printf("* Please, select coffee to %s OR press 0 to go back *\n", cancelOrEditText.c_str());
        printf("*****************************************************\n");
        cin >> optionS;
        option = toInt(optionS);
    } while (!isANum(optionS) && (option < 0 || option > MAX_COFFEES || auxOrder[option - 1] == -1));
    return option - 1;
}

//Esta funci�n se llamar� al seleccionar la opci�n de cancelar el caf� actual.
void
cancelCurrentCoffee() {
    __asm {
        MOV currentCoffee, 0 //Se resetea el valor de currentCoffee a 0
    }
    //Esto ya deber�a volver al men� de agregar otro caf�, pagar o 
    //cancelar orden.
}

//En el men� despu�s de confirmar un caf�, puedo llamar esta funci�n,
//la cual permite cancelar cualquier caf� que se haya hecho en el pedido,
//esto es, recibiendo por teclado la posici�n del caf� que se desea
//cancelar.
void
cancelAnyCoffee(int coffeeSelected) {
    __asm {
        MOV EAX, 4                //Movemos el tama�o 4 Bytes (int) a EAX
        IMUL coffeeSelected       //Multiplicamos los 4 Bytes por la
                                  //posici�n a cancelar recibida
        MOV auxOrder+([EAX]), -1
    }
    //Esto ya deber�a volver al men� de agregar otro caf�, pagar o 
    //cancelar orden.
}

void
flipTopping(int bitTopping) {
    __asm {
        MOV EAX, currentCoffee
        XOR EAX, bitTopping       //Se invierte el bit correspondiente
                                  //a la adici�n
        MOV currentCoffee, EAX
    }
}

// Funci�n para mostrar los detalles del caf� actual y permitir al usuario
// confirmarlo, cancelarlo o editarlo.
int
showCurrentCoffeeDetails() {
    printf("***************************************\n");
    printf("*           CURRENT COFFEE            *\n");
    printCoffeeDetails(currentCoffee);
    //Enviamos true porque quiero que me imprima el precio
    //de una vez
    computeCoffeePrice(currentCoffee, true);
    printf("* 0. Confirm coffee                   *\n");
    printf("* 1. Edit current coffee              *\n");
    printf("* 8. Cancel current coffee            *\n");
    printf("* 9. Cancel order                     *\n");
    printf("***************************************\n");
    string optionS;
    cin >> optionS;
    int option = toInt(optionS);
    //Si no se ingres� un n�mero entonces vuelva y muestre el men�
    if (!isANum(optionS)) return showCurrentCoffeeDetails();
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

    int toppingAdded[COFFEE_ADDITION]; //Para almacenar si una adici�n
                                        //fue agregada o no al pedido
    int position;

    __asm {
        MOV position, 0
        MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                      //el arreglo
        IMUL position
        MOV EDX, EAX
        MOV EAX, currentCoffee     
        AND EAX, WHIPPED_CREAM        //Hacemos AND entre el current coffee
                                      //y bit de Wipped Cream
        MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

        MOV position, 1
        MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                      //el arreglo
        IMUL position
        MOV EDX, EAX
        MOV EAX, currentCoffee     
        AND EAX, CINNAMON             //Hacemos AND entre el current coffee
                                      //y bit de Cinnamon
        MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

        MOV position, 2
        MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                      //el arreglo
        IMUL position
        MOV EDX, EAX
        MOV EAX, currentCoffee     
        AND EAX, CHOCOLATE            //Hacemos AND entre el current coffee
                                      //y bit de CHOCOLATE
        MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

        MOV position, 3
        MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                      //el arreglo
        IMUL position
        MOV EDX, EAX
        MOV EAX, currentCoffee     
        AND EAX, AMARETTO             //Hacemos AND entre el current coffee
                                      //y bit de AMARETTO
        MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo

        MOV position, 4
        MOV EAX, 4                    //Movemos 4 Bytes a EAX para recorrer
                                      //el arreglo
        IMUL position
        MOV EDX, EAX
        MOV EAX, currentCoffee     
        AND EAX, IRISH_WHISKEY        //Hacemos AND entre el current coffee
                                      //y bit de IRISH_WHISKEY
        MOV toppingAdded+([EDX]), EAX //Movemos el resultado del AND al arreglo
    }
    //Leer la adici�n que se quiere agregar
    printf("******************************************\n");
    printf("*            CURRENT COFFEE              *\n");
    printf("******************************************\n");
    printf("* ");
    setConsoleColor(YELLOW);
    printf("ID ");
    setConsoleColor(WHITE);
    printf("*     ");
    setConsoleColor(YELLOW);
    printf("Topping     ");
    setConsoleColor(WHITE);
    printf("*  ");
    setConsoleColor(YELLOW);
    printf("Price  ");
    setConsoleColor(WHITE);
    printf("* ");
    setConsoleColor(YELLOW);
    printf("Added ");
    setConsoleColor(WHITE);
    printf("*\n");
    printf("******************************************\n");
    string optionS;
    for (int i = 0; i < COFFEE_ADDITION; i++) {
        printf("* %d. * %-15s *  %5s  * ", (i+1), 
                                        toppingNames[i].c_str(),
                                        toppingPrices[i].c_str());
        if (toppingAdded[i]) {
            setConsoleColor(GREEN);
            printf("%4s  ", "YES");
        }
        else {
            setConsoleColor(RED);
            printf("%4s  ", "NO");
        }
        setConsoleColor(WHITE);
        printf("*\n");
    }
    printf("******************************************\n");
    printf("* 0. Continue                            *\n");
    printf("* 8. Cancel current coffee               *\n");
    printf("* 9. Cancel order                        *\n");
    printf("******************************************\n");
    cin >> optionS;
    int option = toInt(optionS);
    bool showToppingsAgain;
    //Si lo ingresado no es un n�mero, muestre el men� de nuevo
    if (!isANum(optionS)) {
        __asm {
            MOV orderCanceled, 0     //Apago flag de orden cancelado
            MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
        }
        return showToppingsAgain;
    }
    int optionSelectedWhenContinue;
    switch (option) {
        case 0:
            optionSelectedWhenContinue = showCurrentCoffeeDetails();
            if (optionSelectedWhenContinue == 0) {
                //showCurrentCoffeeDetails me devuelve que quiere confirmar
                //el caf�
                confirmCoffee();
                __asm {
                    MOV orderCanceled, 0     //Apago flag de orden cancelado
                    MOV showToppingsAgain, 0 //Apago flag de mostrar adiciones
                }
            }
            else if (optionSelectedWhenContinue == 1) {
                //showCurrentCoffeeDetails me devuelve que quiere editar
                //el caf� actual
                __asm {
                    MOV orderCanceled, 0     //Apago flag de orden cancelado
                    MOV showToppingsAgain, 1 //Prendo flag de mostrar adiciones
                }
            }
            else if (optionSelectedWhenContinue == 8) {
                //showCurrentCoffeeDetails me devuelve que quiere cancelar
                //el caf� actual
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
        case 8: //Cancel current coffee
            cancelCurrentCoffee();
            __asm {
                MOV orderCanceled, 0     //Apago flag de orden cancelado
                MOV showToppingsAgain, 0 //Apago flag de mostrar adiciones
            }
            break;
        case 9: //Cancel order
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

//Puede ser llamado en cualquiera de los men�s al momento de estar haciendo
//la orden
void
cancelOrder() {
    if (isTableOrder) {
        __asm {
            MOV EAX, currentTableId
            MOV EDX, EAX
            MOV tablesStatus+([EDX]), 0//Se mueve un cero al estado de esa mesa
                                       //para indicar que ya est� disponible
        }
    }
    __asm { 
        MOV currentCoffee, 0        //Limpiar el caf� actual
        MOV ECX, MAX_COFFEES        //Inicializar el contador para el ciclo
        JMP clearLoopCancelOrder

clearLoopCancelOrder:
        MOV EDX, ECX
        DEC EDX
        MOV EAX, 4              //Movemos el tama�o 4 Bytes (int) a EAX
        IMUL EDX                //Multiplicamos los 4 Bytes por la
                                //posici�n candidata
        MOV EDX, EAX
        MOV auxOrder+([EDX]), -1//Limpiar la posici�n actual del vector
        LOOP clearLoopCancelOrder
    }
}

//Recibe un �ndice para editar, el caf� que haya all� se mueve a currentCoffee,
//se espera que luego de este m�todo se llame a showToppings()
void
editCoffee(int indexToEdit) {
    __asm {
        MOV EAX, 4                //Movemos el tama�o 4 Bytes (int) a EAX
        IMUL indexToEdit          //Multiplicamos los 4 Bytes por la
                                  //posici�n a editar
        MOV EDX, EAX
        MOV EAX, auxOrder+([EDX])
        MOV currentCoffee, EAX    //Limpiar la posici�n actual del
                                  //vector
        MOV auxOrder+([EDX]), -1  //Limpiamos el caf� en esa posici�n
    }
}

//M�todo para terminar el pedido, mostrar resumen del mismo y leer
//el valor pagado por el cliente. Retornar� true si se seleccion� back,
//de lo contrario, si se paga satisfactoriamente retorna false.
bool
finishAndPayOrder() {
    printCoffeesInOrder();
    //Almaceno el valor del total de la orden en valueToPay
    double valueToPay = printTotals();
    double amountPaid;
    string amountPaidS;
    if (!tipAsked && isTableOrder) {
        string answer;
        do {
            printf("***************************************************\n");
            printf("* Do you want to include the voluntary tip? (Y/N) *\n");
            printf("***************************************************\n");
            cin >> answer;
        } while (answer.size() > 1 || (tolower(answer[0]) != 'y'
                                   && tolower(answer[0]) != 'n'));
        if (tolower(answer[0]) == 'y') {
            __asm {
                MOV tipIncluded, 1
                MOV tipAsked, 1
            }
        }
        else {
            __asm {
                MOV tipAsked, 1
                MOV tipIncluded, 0
            }
        }
        return finishAndPayOrder();
    }
    do {
        printf("***************************************************\n");
        printf("* Please, enter amount paid OR press 0 to go back *\n");
        printf("***************************************************\n");
        cin >> amountPaidS;
        amountPaid = toDouble(amountPaidS);
    } while (!isDouble(amountPaidS) || amountPaid < 0);
    if (amountPaid == 0) {
        __asm {
            MOV tipAsked, 0
            MOV tipIncluded, 0
        }
        return true; //Seleccion� back, entonces
                     //retorno que quiero volver
                     //a mostrar el men� anterior
    }


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
    double zero = 0;
    __asm {
        FLD valueToPay
        FCOMP amountPaid
        FSTSW AX
        SAHF
        JA notEnoughMoney        //Si valueToPay > amountPaid entonces debe
                                 //ingresar un valor mayor
        MOV isAmountPaidEnough, 1//S� fue suficiente el valor pagado
        FSTP change              //Limpiamos contenido de pila de flotantes
        FLD amountPaid           //Guardamos en la pila el valor pagado
        FSUB valueToPay          //Restamos el valor del pedido para hallar
                                 //para hallar la diferencia
        FSTP change
        JMP endPayment

        /* AYUDA DE MONITOR CAUSABA RESULTADOS INESPERADOS
        FLD valueToPay           //Cargar el valor a pagar en la pila
        FLD amountPaid           //Cargamos tambi�n el valor pagado
        FSUB                     //Restamos ambos valores
        FCOM zero                
        FSTSW AX                 //Pasar los flags flotantes a ax
        SAHF                     //Cargar los flags desde ax
        JA notEnoughMoney        //Si valueToPay > amountPaid entonces debe
                                 //ingresar un valor mayor
        MOV isAmountPaidEnough, 1//S� fue suficiente el valor pagado
        FSTP change              //Limpiamos contenido de pila de flotantes
        FLD amountPaid           //Guardamos en la pila el valor pagado
        FSUB valueToPay          //Restamos el valor del pedido para hallar
                                 //para hallar la diferencia
        FSTP change
        JMP endPayment*/

notEnoughMoney:
        MOV isAmountPaidEnough, 0
        JMP endPayment

endPayment:
        NOP
    }
    if (!isAmountPaidEnough) {
        return finishAndPayOrder(); //Vuelvo a mostrar el resumen
                             //del pedido y pido el valor
                             //a pagar.
    }
    else {
        setConsoleColor(YELLOW);
        printf("***************************************\n");
        printf("***************************************\n");
        setConsoleColor(BLUE);
        printf("***************************************\n");
        setConsoleColor(RED);
        printf("***************************************\n");
        setConsoleColor(WHITE);
        printf("*            COFFEE STAR              *\n");
        printf("***************************************\n");
        printf("*            BILL OF SALE             *\n");
        printf("***************************************\n");  
        if (isTableOrder)  {
            printf("***************************************\n");  
            printf("* Type of sale: Table service         *\n");
            printf("* Table number: %2d                    *\n",
                   currentTableId + 1);
        }
        else printf("* Type of sale: Bar service           *\n");
        printf("***************************************\n"); 
        printf("***************************************\n");  
        printCoffeesInOrder();
        printTotals();
        printf("***************************************\n");
        printf("***************************************\n");
        printf("* Amount paid:               $%7.2f *\n", amountPaid);
        printf("* Change:                    $%7.2f *\n", change);
        printf("***************************************\n");
        printf("*       Thanks for visiting us        *\n");
        printf("*       We hope to see you soon       *\n");
        printf("***************************************\n");
        __asm {
            MOV tipAsked, 0     //Reseteamos los valores de tipAsked
            MOV tipIncluded, 0  //y tipIncluded para otro pedido nuevo
        }
        if (!isTableOrder) addBarToCashbox();
                                            //Se env�a el ID de la mesa
                                            //que antend�, para indicar
                                            //la posici�n en la que se quiere
                                            //almacenar en el Cashbox
        else if (isTableOrder) addTableToCashBox(currentTableId); 
        cancelOrder(); //Llamamos el m�todo cancelar orden para limpiar el
                       //arreglo auxiliar.
    }
    string pauseLn;
    getline(cin, pauseLn); //Leer fin de l�nea despu�s de la anterior tecla
                           //presionada.
    printf("Press any key to continue...");
    getline(cin, pauseLn);
    return false; //Retornamos false porque no queremos volver
                  //a mostrar el men�.
}

bool
makeOrder() {
    __asm {
        MOV isOrderEmpty, 1 //Supongo que la orden est� vac�a
        MOV isOrderFull, 1  //Suponemos que la orden est� llena
    }

    int position;

    __asm {
        MOV ECX, MAX_COFFEES
        JMP checkEmptiness

checkEmptiness:
        MOV EAX, MAX_COFFEES     
        SUB EAX, ECX                //Se convierte ese valor de manera 
                                    //que se itere ascendentemente.
                                    // 8 - ECX (N�mero de iteraci�n)
        MOV position, EAX
        MOV EAX, 4                  //Movemos el tama�o 4 Bytes (int) a EAX
        IMUL position               //Multiplicamos los 4 Bytes por la
                                    //posici�n candidata
        CMP auxOrder+([EAX]), -1    //Comparamos el contenido del
                                    //arreglo auxOrder en la posici�n
                                    //indicada con -1.
        JE continueChecking         //Mientras las posiciones sean iguales a
                                    //-1 sigue iterando.
        MOV isOrderEmpty, 0         //Se encontr� al menos un caf� en el
                                    //pedido.
        //JMP endChecking
        LOOP checkEmptiness         //Sigo iterando porque necesito ver
                                    //si todo el arreglo est� lleno o no
        JMP endChecking
        

continueChecking:
        MOV isOrderFull, 0          //Encontramos un -1, la orden no est� llena
        LOOP checkEmptiness
        JMP endChecking

endChecking:
        NOP
    }

    string optionS;
    int option;
    printf("***************************************\n");
    if (!isOrderEmpty) {
        printf("* 0. Finish and pay order             *\n");
    }
    if (!isOrderFull) printf("* 1. Add a coffee                     *\n");
    if (!isOrderEmpty) {
        printf("* 2. Edit a coffee                    *\n");
        printf("* 3. Remove a coffee                  *\n");
        if (isTableOrder) {
            printf("* 4. Save order                       *\n");
        }
    }
    printf("* 9. Cancel order                     *\n");
    printf("***************************************\n");
    cin  >> optionS;
    option = toInt(optionS);
    bool continueMakingOrder;
    if (!isANum(optionS)) {
        __asm {
            MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
        }
        return continueMakingOrder;
    }
    int auxCancelOrder;
    int indexToCancelOrEdit;
    switch (option) {
        case 0:
            //Revisar que si haya almenos un caf� para pagar
            if (isOrderEmpty) {
                __asm {
                    MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
                }
                break;
            }
            if (finishAndPayOrder()) {
                __asm {
                    MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
                }
            }
            else {
                __asm {
                    MOV continueMakingOrder, 0 //No volvemos a mostrar el men�
                                               //porque ya se pag�
                                               //correctamente.
                }
            }
            break;
        case 1:
            if (isOrderFull) {
                //No podemos a�adir m�s caf�s
                __asm {
                    MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
                }
                break;
            }
            while (continueMakingOrder = showToppings());
            auxCancelOrder = orderCanceled;
            __asm {
                MOV continueMakingOrder, 1 //Suponemos que debo mostrar el men�
                                           //de hacer orden
                MOV EAX, auxCancelOrder
                CMP EAX, 0                 //Revisamos si cancel� la order
                JE endAddCoffee            //Se confirm� un caf� y no se
                                           //cancel� la orden
                MOV continueMakingOrder, 0 //Cancel� la orden entonces debo
                                           //retornar al men� inicial

endAddCoffee:
                NOP
            }
            break;
        case 2:
            //Revisar que si haya almenos un caf� para editar
            if (isOrderEmpty) {
                __asm {
                    MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
                }
                break;
            }
            indexToCancelOrEdit = selectCoffeeToCancelOrEdit("edit");
            if (indexToCancelOrEdit == -1) {
                //Si seleccion� 'go back' entonces
                //vuelvo a mostrar el men�
                __asm {
                    MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
                }
                break;
            }
            editCoffee(indexToCancelOrEdit);
            while (continueMakingOrder = showToppings());
            auxCancelOrder = orderCanceled;
            __asm {
                MOV continueMakingOrder, 1 //Suponemos que debo mostrar el men�
                                           //de hacer orden
                MOV EAX, auxCancelOrder
                CMP EAX, 0                 //Revisamos si cancel� la order
                JE endEditCoffee           //Se confirm� un caf� y no se
                                           //cancel� la orden
                MOV continueMakingOrder, 0 //Cancel� la orden entonces debo
                                           //retornar al men� inicial

endEditCoffee:
                NOP
            }
            break;
        case 3:
            //Revisar que si haya almenos un caf� para quitar
            if (isOrderEmpty) {
                __asm {
                    MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
                }
                break;
            }
            indexToCancelOrEdit = selectCoffeeToCancelOrEdit("remove");
            if (indexToCancelOrEdit == -1) {
                //Si seleccion� 'go back' entonces
                //vuelvo a mostrar el men�
                __asm {
                    MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
                }
                break;
            }
            cancelAnyCoffee(indexToCancelOrEdit);
            continueMakingOrder = true;
            break;
        case 4:
            if (isOrderEmpty || !isTableOrder) {
                //Revisar que si haya almenos un caf� para guardar y que
                //si sea de una mesa el pedido
                __asm {
                    MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
                }
                break;
            }
            //Lo �nico que se espera es salirnos de este men�, y en el
            //men� anterior copiamos los valores que hay en auxOrder
            //a la matriz de mesas en la posici�n indicada
            __asm {
                MOV continueMakingOrder, 0 //Se guard� la orden y se debe
                                           //volver al men� inicial
                MOV EAX, currentTableId
                MOV EDX, EAX
                MOV tablesStatus+([EDX]), 1 //Se mueve un uno al estado 
                                            //de esa mesa para indicar que ya
                                            //est� ocupada

            }
            break;
        case 9:
            cancelOrder();
            __asm {
                MOV continueMakingOrder, 0 //Se cancel� la orden y se debe
                                           //volver al men� inicial
            }
            break;
        default:
            __asm {
                MOV continueMakingOrder, 1 //Volvemos a mostrar el men�
            }
            break;
    }
    return continueMakingOrder;
}

void
handleTable(int tableIndex) {
    //Asigna el pedido de lo que hay en el arreglo auxiliar
    //a la mesa que recibe como par�metro
    int rowPosition;
    __asm {
        MOV ECX, MAX_COFFEES
        JMP copyArray

copyArray:
        MOV EAX, 4
        IMUL tableIndex             //Ya tenemos los Bytes de la fila
        IMUL MAX_COFFEES            //Eq: fila * (4Bytes * maxcolumnas)
        MOV rowPosition, EAX        //correspondiente a la mesa
        MOV EDX, ECX                //Ahora vamos a procesar los Bytes
        DEC EDX                     //para la columna
        MOV EAX, 4                  
        IMUL EDX
        MOV EDX, EAX
        MOV EAX, auxOrder+([EDX])   //Movemos a EAX el contenido del
                                    //arreglo auxOrder en la posici�n
                                    //indicada
        ADD EDX, rowPosition        //Sumamos las filas a las columnas
        MOV tables+([EDX]), EAX     //Movemos al arreglo de la barra el
                                    //contenido que hab�a en el auxiliar
        LOOP copyArray
    }
}

int 
selectTable(){
    int option;
    string optionS;
    do{
        printf("******************************************************\n");
        printf("* ");
        setConsoleColor(GREEN);
        printf("Available");
        setConsoleColor(WHITE);
        printf("                                          *\n");
        printf("******************************************************\n");
        for (int i = 0; i < MAX_TABLES; i++) {
            if (!tablesStatus[i]) {
                printf("* Table # %2d              ", (i+1));
                printf("                           *\n");
            }
        }
        printf("******************************************************\n");
        printf("* ");
        setConsoleColor(RED);
        printf("Occupied ");
        setConsoleColor(WHITE);
        printf("                                          *\n");
        printf("******************************************************\n");
        for (int i = 0; i < MAX_TABLES; i++) {
            if (tablesStatus[i]) {
                printf("* Table # %2d              ", (i+1));
                printf("                           *\n");
            }
        }
        printf("******************************************************\n");
        printf("* Select table to manage order OR press 0 to go back *\n");
        printf("******************************************************\n");
        cin >> optionS;
        option = toInt(optionS);
    }
    while (!isANum(optionS) && (option < 0 || option > 20));
    return option - 1;
}

void
barMenu(){
    //Se limpia el arreglo auxiliar para pedidos.
    for (int i = 0; i < MAX_COFFEES; i++) {
        auxOrder[i] = -1; //Se hace para luego saber qu� caf�s existen
                          //en el pedido, y cu�les no.
    }
    while (makeOrder());
}

//Mueve todos los caf�s de una mesa seleccionada al arreglo auxiliar
void
tableMenu(int tableSelected) {
    //Traemos el pedido que tiene la mesa seleccionada actualmente
    int rowPosition;
    __asm {
        MOV ECX, MAX_COFFEES
        JMP putTableOrderInAux

putTableOrderInAux:
        MOV EAX, 4            //Movemos 4 Bytes
        IMUL tableSelected    //Multiplicamos por la fila
        IMUL MAX_COFFEES      //Ecuaci�n fila * (4Bytes * MAX_COFFEES)
        MOV rowPosition, EAX  //Almacenamos el valor multiplicado
        MOV EDX, ECX          //Traemos de ECX lo que corresponde a la columna
        DEC EDX               //Decrementamos porque ECX est� base 1
        MOV EAX, 4            //Movemos 4 Bytes
        IMUL EDX              //Multiplicamos por columna
        MOV EDX, EAX         
        ADD EDX, rowPosition  //Sumo lo que hab�a guardado de la fila
        MOV EAX, tables+([EDX])//Recupero el valor del cafe en la mesa
        SUB EDX, rowPosition  //Se resta el valor de la fila a la columna
        MOV auxOrder+([EDX]), EAX //Muevo el valor que recuper� al arreglo
                                  //auxiliar
        LOOP putTableOrderInAux //Itero
    }
    while (makeOrder());
    //Copiar el arreglo auxiliar a la matriz de la mesa en la posici�n
    //correspondiente a la mesa seleccionada
    handleTable(tableSelected);
}

void 
manageOrder() {
    int option;
    string optionS;
    printf("***************************************\n");
    printf("*      Please select an option        *\n");
    printf("***************************************\n");
    printf("* 1. Table                            *\n");
    printf("* 2. Bar                              *\n");
    printf("* 9. Go back                          *\n");
    printf("***************************************\n");
    cin >> optionS;
    option = toInt(optionS);
    if (!isANum(optionS)) {
        manageOrder();
        return;
    }
    int tableSelected;
    switch (option) {
        case 1:
            isTableOrder = true;
            tableSelected = selectTable(); // Se sabe que mesa se desea atender
            if (tableSelected == -1) { //Se seleccion� back
                manageOrder();
                break;
            }
            __asm {
                MOV EAX, tableSelected
                MOV currentTableId, EAX//Guardamos el id de la mesa
                                       //que vamos a atender
            }
            tableMenu(tableSelected);
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
void
addBarToCashbox() {
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
            int coffeesCountPosition = COFFEE_ADDITION * 4;
            __asm {
                MOV ECX, COFFEE_ADDITION
                JMP iterateToppings2

iterateToppings2:
                MOV position, ECX
                DEC position
                MOV EAX, 4                  //Movemos 4 Bytes a EAX para recorrer
                IMUL position
                MOV position, EAX           //Almacenamos positonInt
                MOV EDX, position        //Recuperamos el valor de positionInt
                                             //almacenandolo en EDX
                MOV EAX, coffee              //Movemos el caf� actual a EAX

                AND EAX, toppingBits+([EDX]) //Hacemos and entre caf� actual
                                             //y bits correspondientes a la
                                             //adici�n
                CMP EAX, 0                   //Comparamos con 0 para saber si
                                             //el AND dio diferente de 0,
                                             //indicando que s� tiene la adici�n
                JNE addCashBoxCount          //Si tiene la adici�n vamos a sumar
                LOOP iterateToppings2        //Iteramos en las adiciones
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
                                             //caf�s
                INC EAX
                MOV barCashBox+([EDX]), EAX
                NOP
            }
        }
    }
}

//Toma el arreglo auxiliar y lo suma al cuadre de la mesa recibida
void
addTableToCashBox(int tableIndex) {
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
            JE dontAnalyzeTable
            MOV analyzeCoffee, 1
            JMP endAnalyzingTable

dontAnalyzeTable:
            MOV analyzeCoffee, 0
            JMP endAnalyzingTable

endAnalyzingTable:
            NOP
        }
        if (analyzeCoffee) {
            int position;
            int coffeesCountPosition = COFFEE_ADDITION * 4;
            int columnPosition;

            __asm {
                MOV ECX, COFFEE_ADDITION
                JMP iterateToppings2Table

iterateToppings2Table:
                MOV position, ECX
                DEC position
                MOV EAX, 4                //Movemos 4 Bytes a EAX para recorrer
                IMUL position
                MOV position, EAX            //Almacenamos positonInt
                MOV EDX, position            //Recuperamos el valor de position
                                             //almacenandolo en EDX
                MOV columnPosition, EDX      //Guardamos la posici�n de la
                                             //columna
                MOV EAX, coffee              //Movemos el caf� actual a EAX

                AND EAX, toppingBits+([EDX]) //Hacemos and entre caf� actual
                                             //y bits correspondientes a la
                                             //adici�n
                CMP EAX, 0                   //Comparamos con 0 para saber si
                                             //el AND dio diferente de 0,
                                             //indicando que s� tiene la adici�n
                JNE addCashBoxCountTable     //Si tiene la adici�n vamos a sumar
                LOOP iterateToppings2Table   //Iteramos en las adiciones
                JMP endAddingCashBoxCountTable

addCashBoxCountTable:
                MOV EAX, 4
                MOV EBX, COFFEE_ADDITION
                INC EBX
                IMUL EBX                    //Multiplicamos los 4 Bytes por las
                                            //5 adiciones (+ casilla para caf�)
                IMUL tableIndex             //Eq = fila 
                                            //  * (4Bytes * COFFEE_ADDITION+1)
                MOV EDX, EAX                //Sumamos los Bytes de la fila
                                            //a la columna que ya tengo en EDX
                ADD EDX, columnPosition     //Se suma la columna que corresponde
                                            //a la adici�n
                MOV EAX, tablesCashBox+([EDX])//Aumentamos contador de adiciones
                INC EAX
                MOV tablesCashBox+([EDX]), EAX
                LOOP iterateToppings2Table
                JMP endAddingCashBoxCountTable

endAddingCashBoxCountTable:
                MOV EAX, 4      
                MOV EBX, COFFEE_ADDITION
                INC EBX                     //NOTA: No se guarda el EDX
                                            //anterior, por lo que necesitamos
                                            //recuperar el EDX de la variable
                IMUL EBX                    //Multiplicamos los 4 Bytes por las
                                            //5 adiciones (+ casilla para caf�)
                IMUL tableIndex             //Eq = fila 
                                            //  * (4Bytes * COFFEE_ADDITION+1)
                MOV EDX, EAX                //Sumamos los Bytes de la fila
                ADD EDX, coffeesCountPosition 
                MOV EAX, tablesCashBox+([EDX])  //Aumentamos uno al contador de
                                               //caf�s
                INC EAX
                MOV tablesCashBox+([EDX]), EAX
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
    double totalSoldInTables;

    printf("***************************************\n");
    printf("*           ");
    setConsoleColor(YELLOW);
    printf("TABLE SERVICE             ");
    setConsoleColor(WHITE);
    printf("*\n");
    printf("***************************************\n");
    for (int k = 0; k < MAX_TABLES; k++) {
        if(tablesCashBox[k][COFFEE_ADDITION] == 0) {
            //printf("***************************************\n");
            printf("* ");
            setConsoleColor(CYAN);
            printf("Table # %2d does not record sales", (k+1));
            setConsoleColor(WHITE);
            printf("    *\n");
            printf("***************************************\n");
        }
        else {
            int rowIndex;
            __asm {
                MOV EAX, COFFEE_ADDITION
                MOV rowIndex, EAX           //Obtenemos los Bytes de la fila
                MOV EAX, 4                  //Se calcula posici�n por la fila
                INC rowIndex                    //con la ecuaci�n:
                IMUL rowIndex               //Eq: fila 
                                            //  * (4Bytes * COFFEE_ADDITION)
                IMUL k                      //Multiplicamos por la fila actual
                MOV rowIndex, EAX           //Almacenamos el �ndice de la fila
                MOV EDX, EAX                //Almacenamos resultado en EDX
                MOV EAX, 4                  //Volvemos a mover 4 Bytes para ya
                                            //hacer las columnas
                IMUL COFFEE_ADDITION        //Multiplicamos la �ltima columna
                                            //que es donde est�n los caf�s que
                                            //se vendieron en la mesa k
                MOV EDX, EAX                //Ponemos el resultado al que ya
                                            //ten�amos para obtener el resultado
                                            //exacto de la posici�n en el arreglo
                ADD EDX, rowIndex           //Sumamos los Bytes de la fila a lo
                                            //que ten�amos
                MOV EAX, tablesCashBox+([EDX])
                MOV coffeesQuantity, EAX
                FILD coffeesQuantity
                FMUL coffeePrice
                FST tableCoffeesPrice
                FSTP totalSoldInTables
            }
            //printf("***************************************\n");
            printf("* --------- ");
            setConsoleColor(RED);
            printf("TABLE # %2d", (k+1));
            setConsoleColor(WHITE);
            printf(" -------------- *\n");
            printf("***************************************\n");
            printf("* Coffees sold: %-3d                   *\n", coffeesQuantity);
            printf("* > Unit price:             $    2.00 *\n"); 
            printf("* > Total price:            $%8.2f *\n", tableCoffeesPrice);
            for (int i = 0; i < COFFEE_ADDITION; i++) {
                double toppingTotalPrice;
                int toppingsQuantity;
                int toppingsPricePosition;
                __asm {
                    /*MOV EAX, 4                    //Se calcula posici�n por la fila
                    MOV EBX, COFFEE_ADDITION    //Eq = filas 
                                                //* (4Bytes 
                                                //* (COFFE_ADDITION + 1))
                    INC EBX
                    IMUL EBX
                    IMUL k                      //Multiplicamos por la fila actual
                    MOV EDX, EAX                //Almacenamos resultado en EDX*/
                    MOV EAX, 4
                    IMUL i
                    MOV toppingsPricePosition, EAX //Almacenamos la posici�n de la
                                                //columna para ir por el precio
                                                //despu�s
                    ADD EAX, rowIndex           //Sumamos el resultado al que ya
                                                //ten�amos para obtener el resultado
                                                //exacto de la posici�n en el arreglo
                    MOV EDX, EAX                //Movemos el resultado
                    MOV EAX, tablesCashBox+([EDX])
                    MOV toppingsQuantity, EAX
                    FILD toppingsQuantity
                    MOV EAX, 2                  //Hay que multiplicar la posici�n
                                                //por 2 porque hay que acceder
                                                //al arreglo de los precios que
                                                //es double
                    IMUL toppingsPricePosition
                    MOV EDX, EAX
                    FMUL toppingPricesValues+([EDX])
                    FSTP toppingTotalPrice
                    FLD totalSoldInTables
                    FADD toppingTotalPrice
                    FSTP totalSoldInTables //Se va guardando todo lo que 
                                          //se vendi� para mostrar un total 
                                          //de las mesas
                }
                printf("***************************************\n");
                printf("* ");
                setConsoleColor(PURPLE);
                printf("%-13s                       ", toppingNames[i].c_str());
                setConsoleColor(WHITE);
                printf("*\n");
                printf("* > Units sold:              %8d *\n", toppingsQuantity);
                printf("* > Unit price:             $%8.2f *\n", toppingPricesValues[i]);       
                printf("* > Total price:            $%8.2f *\n", toppingTotalPrice);
            }
            printf("***************************************\n");
            printf("*         ");
            setConsoleColor(BLUE);
            printf("TABLE %2d SERVICE", (k+1));
            setConsoleColor(WHITE);
            printf("            *\n");
            printf("* SUBTOTAL                  $%8.2f *\n", totalSoldInTables);
            double iva = 0.1;
            __asm{
                FLD iva
                FMUL totalSoldInTables
                FSTP iva
            }
            printf("* IVA (10 %%)                $%8.2f *\n", iva);
            printf("***************************************\n");
            __asm{
                FLD totalSoldInTables
                FADD iva
                FSTP totalSoldInTables
            }
            printf("* TOTAL                     $%8.2f *\n", totalSoldInTables);
            printf("***************************************\n");
        }
    }
}

void
checkCashboxBar() {
    double totalSoldInBar;
    if(barCashBox[COFFEE_ADDITION] == 0){
            printf("***************************************\n");
            printf("*  ");
            setConsoleColor(CYAN);
            printf("Bar service does not record sales");
            setConsoleColor(WHITE);
            printf("  *\n");
            printf("***************************************\n");
    }
    else {
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
            FST barCoffeesPrice
            FSTP totalSoldInBar  //Se va guardando todo lo que se vendi� para
                                 //mostrar un total de la barra
        }
        printf("***************************************\n");
        printf("* ----------- ");
        setConsoleColor(RED);
        printf("BAR SERVICE");
        setConsoleColor(WHITE);
        printf(" ----------- *\n");
        printf("***************************************\n");
        printf("* Coffees sold: %-3d                   *\n", coffeesQuantity);
        printf("* > Unit price:             $    2.00 *\n");       
        printf("* > Total price:            $%8.2f *\n", barCoffeesPrice);
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
                FLD totalSoldInBar  
                FADD toppingTotalPrice
                FSTP totalSoldInBar   //Se va guardando todo lo que 
                                      //se vendi� para mostrar un total 
                                      //de la barra
            }
            printf("***************************************\n");
            printf("* ");
            setConsoleColor(PURPLE);
            printf("%-13s                       ", toppingNames[i].c_str());
            setConsoleColor(WHITE);
            printf("*\n");
            printf("* > Units sold:              %8d *\n", toppingsQuantity);
            printf("* > Unit price:             $%8.2f *\n", toppingPricesValues[i]);       
            printf("* > Total price:            $%8.2f *\n", toppingTotalPrice);
        }
        printf("***************************************\n");
        printf("*            ");
        setConsoleColor(BLUE);
        printf("BAR SERVICE");
        setConsoleColor(WHITE);
        printf("              *\n");
        printf("* SUBTOTAL                  $%8.2f *\n", totalSoldInBar);
        double iva = 0.1;
        __asm{
            FLD iva
            FMUL totalSoldInBar
            FSTP iva
        }
        printf("* IVA (10 %%)                $%8.2f *\n", iva);
        printf("***************************************\n");
        __asm{
            FLD totalSoldInBar
            FADD iva
            FSTP totalSoldInBar
        }
        printf("* TOTAL                     $%8.2f *\n", totalSoldInBar);
        printf("***************************************\n");
    }
}

void
checkCashboxTotals() {
    double totalSoldToday;
    double coffeesTotalPrice = 0;
    double coffeePrice = 2.00;
    int coffeesQuantity;
    int rowIndex;
    __asm {
        MOV EAX, 4
        IMUL COFFEE_ADDITION
        MOV EDX, EAX
        MOV EAX, barCashBox+([EDX])
        MOV coffeesQuantity, EAX  //Hasta aqu� se tienen los caf�s de la barra
        MOV ECX, MAX_TABLES
        JMP iterateCoffeesInTables

iterateCoffeesInTables:
        MOV rowIndex, ECX
        DEC rowIndex
        MOV EAX, 4                  //Se calcula posici�n por la fila
        MOV EBX, COFFEE_ADDITION
        INC EBX                     //Eq: fila 
                                    //* (4Bytes * (COFFE_ADDITION + 1))
        IMUL EBX
        IMUL rowIndex                   //Multiplicamos por la fila actual
        MOV rowIndex, EAX           //Almacenamos la fila en Bytes
        MOV EAX, 4                  //Volvemos a mover 4 Bytes para ya
                                    //hacer las columnas
        IMUL COFFEE_ADDITION        //Multiplicamos la �ltima columna
                                    //que es donde est�n los caf�s que
                                    //se vendieron en la mesa k
        ADD EAX, rowIndex           //sumamos el resultado al que ya
                                    //ten�amos para obtener el resultado
                                    //exacto de la posici�n en el arreglo
        MOV EDX, EAX
        MOV EAX, tablesCashBox+([EDX])
        ADD coffeesQuantity, EAX  
        LOOP iterateCoffeesInTables
        JMP getCoffeesTotalPrice

getCoffeesTotalPrice:
        FILD coffeesQuantity
        FMUL coffeePrice
        FST coffeesTotalPrice
        FSTP totalSoldToday  
    }

    printf("***************************************\n");
    printf("* --------- ");
    setConsoleColor(RED);
    printf("GENERAL SERVICE");
    setConsoleColor(WHITE);
    printf(" --------- *\n");
    printf("***************************************\n");
    printf("* Coffees sold: %-3d                   *\n", coffeesQuantity);
    printf("* > Unit price:             $    2.00 *\n");
    printf("* > Total price:            $%8.2f *\n", coffeesTotalPrice);
    for (int i = 0; i < COFFEE_ADDITION; i++) {
        double toppingTotalPrice;
        int toppingsQuantity;
        int toppingPricePos;
        __asm {
            MOV EAX, 4
            IMUL i
            MOV EDX, EAX
            MOV EAX, barCashBox+([EDX])
            MOV toppingsQuantity, EAX
            MOV EAX, 2
            IMUL EDX
            MOV toppingPricePos, EAX
        }
        for(int k = 0; k < MAX_TABLES; k++) {
            __asm{
                MOV EAX, 4                  //Se calcula posici�n por la fila
                MOV EBX, COFFEE_ADDITION
                INC EBX                     //Eq= fila 
                                            //* (4Bytes*(COFFEE_ADDITION + 1))
                IMUL EBX
                IMUL k                      //Multiplicamos por la fila actual
                MOV rowIndex, EAX           //Almaceno la fila
                MOV EAX, 4
                IMUL i
                ADD EAX, rowIndex           //Sumamos el resultado al que ya
                                            //ten�amos para obtener el resultado
                                            //exacto de la posici�n en el arreglo
                MOV EDX, EAX                //Movemos el resultado
                MOV EAX, tablesCashBox+([EDX])
                ADD toppingsQuantity, EAX
            }
        }
        __asm{
            FILD toppingsQuantity           //toppingsQuantity es la suma de 
                                            //las cantidades del topping en
                                            //la barra y la mesa
            MOV EDX, toppingPricePos
            FMUL toppingPricesValues+([EDX])
            FSTP toppingTotalPrice
            FLD totalSoldToday  
            FADD toppingTotalPrice
            FSTP totalSoldToday   //Se va guardando todo lo que 
                                  //se vendi� para mostrar un total 
                                  //general
        }
        printf("***************************************\n");
        printf("* ");
        setConsoleColor(PURPLE);
        printf("%-13s                       ", toppingNames[i].c_str());
        setConsoleColor(WHITE);
        printf("*\n");
        printf("* > Units sold:              %8d *\n", toppingsQuantity);
        printf("* > Unit price:             $%8.2f *\n", toppingPricesValues[i]);       
        printf("* > Total price:            $%8.2f *\n", toppingTotalPrice);
    }
    printf("***************************************\n");
    printf("*         ");
    setConsoleColor(BLUE);
    printf("GENERAL SERVICE");
    setConsoleColor(WHITE);
    printf("            *\n");
    printf("* SUBTOTAL                  $%8.2f *\n", totalSoldToday);
    double iva = 0.1;
    __asm{
        FLD iva
        FMUL totalSoldToday
        FSTP iva
    }
    printf("* IVA (10 %%)                $%8.2f *\n", iva);
    printf("***************************************\n");
    __asm{
        FLD totalSoldToday
        FADD iva
        FSTP totalSoldToday
    }
    printf("* TOTAL                     $%8.2f *\n", totalSoldToday);
    printf("***************************************\n");
}

void
checkCashbox() {
    printf("***************************************\n");
    printf("***************************************\n");
    printf("* ");
    setConsoleColor(GREEN);
    printf("---------- CASHBOX CHECK ----------");
    setConsoleColor(WHITE);
    printf(" *\n");
    printf("***************************************\n");
    checkCashboxBar();
    string pauseLn;
    getline(cin, pauseLn); //Leer primer \n despu�s de �ltima palabra ingresada
    printf("Press any key to continue showing cashbox...");
    getline(cin, pauseLn); //Leer la l�nea que contenga cualquier tecla para 
                           //continuar
    checkCashboxTable();
    printf("Press any key to continue showing cashbox...");
    getline(cin, pauseLn); //Leer la l�nea que contenga cualquier tecla para 
                           //continuar
    checkCashboxTotals();
    printf("Press any key to continue...");
    getline(cin, pauseLn); //Leer la l�nea que contenga cualquier tecla para 
                           //continuar
}

void
clearTables(bool clearAll, int tableNumber) { 
    //Si el booleano est� en true, entonces se procede a limpiar 
    //todas las mesas.
    if (clearAll) {
        for (int i = 0; i < MAX_TABLES; i++) {
            for (int j = 0; j < MAX_COFFEES; j++) {
                tables[i][j] = -1;
            }
            tablesStatus[i] = 0;    //Ponemos la mesa i inicialmente desocupada
        }
    }
    else {
        //Si el booleano est� en false, entonces se procede a limpiar 
        //s�lo la mesa indicada.
        for (int j = 0; j < MAX_COFFEES; j++) {
            tables[tableNumber][j] = -1;
        }
        tablesStatus[tableNumber] = 0;
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
    for (int i = 0; i < MAX_COFFEES; i++) bar[i] = -1;
}

bool
menu() {
    int option;
    string optionS;
    setConsoleColor(WHITE);
    printf("***************************************\n");
    printf("*      ");
    setConsoleColor(BLUE);
    printf("Coffee Star CashierApp 2.0");
    setConsoleColor(WHITE);
    printf("     *\n");
    printf("***************************************\n");
    printf("* 1. Manage Order                     *\n");
    printf("* 2. Check Cashbox                    *\n");
    printf("* 3. Quit                             *\n");
    printf("***************************************\n");
    cin >> optionS;
    option = toInt(optionS);
    if (!isANum(optionS)) return true;
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
            printf("See you tomorrow!\n");
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
    clearCashbox();        //Limpiamos datos de cuadre de caja
    clearBar();            //Limpiamos la barra.

    while (menu());
    system("pause");
    return 0;
}
