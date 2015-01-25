Coffee Star Cashier App
=======================

This repository contains all the practices developed during the Organización de Computadores (ST0254) course at EAFIT University.

##Description
The new CoffeeShop _Coffee Star_ specializes in providing coffee products. The prices of the products are listed below:

| __Product__  | __Price__ |
| ------------- | ------------- |
| Coffee | $2.00 |
| Whipped Cream topping | + $0.89 |
| Cinnamon topping | + $0.25 |
| Chocolate topping | + $0.59 |
| Amaretto topping | + $1.50 |
| Irish Whiskey topping | + $1.75 |

All products are coffee served alone or with one or more additions.

Order bill must include 10% of IVA tax.

##Practice 1:
__Developed in:__ Assembly - SimuProc

__Key words:__ Assembly, SimuProc, CoffeeShop

__Objectives:__
  - Accept the client order, which can be for one or more coffees, with their respective toppings.
  - Add and show in screen the total of the order (including IVA).
  - Read the value paid by the client and calculate the correspoding change.
  - In case the client doesn't have enough money to pay, the order could be modified or calceled.
  - At the end of the day, cashier can check the checkbox, which should show the total number of coffees and toppings sold, and total earning.
  - The option "Quit" allows to close and terminate the program.

__Screenshots:__

Main menu

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/master/Practice%201/images/main.png)

Making an order

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/master/Practice%201/images/makeorder.png)

Current coffee

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/master/Practice%201/images/currentcoffee.png)

Coffee Added

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/master/Practice%201/images/confirmcoffee.png)

Amount to pay

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/master/Practice%201/images/valuetopay.png)

Cashbox

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/master/Practice%201/images/cashbox1.png)

##Practice 2:
__Developed in:__ C++ and embedded Assembly

__Updates from version 1:__
* The visual part of the application was developed in *C++* whereas the business logic was made using embedded *Assembly*.
* Tables were included to the program in order to emulate a more complex coffee shop. That means that table clients can ask for additional coffes at any time and they are allowed to pay when leaving the store.
* Voluntary 10% tips were added for the tables bill of sale.
* Totals appear classified by tables, bar and there is a general total for the entire coffee house.
* A list of tables showing their availability or occupancy is presented to the cashier.

__Screenshots:__

Asking for topping

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/askingForTopping.png)

Bar cashbox

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/barCashBox.png)

Bill of sale

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/billOfSale.png)

Current coffee

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/currentCoffee.png)

Finish and pay order

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/finishAndPayOrder.png)

General cashbox

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/generalCashBox.png)

Lower amount paid

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/lowerAmountPaid.png)

Main

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/main.png)

Make order

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/makeOrder.png)

Tables status

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/showTablesStatus.png)

Show toppings

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/showToppings.png)

Tables cashbox

![alt tag](https://raw.githubusercontent.com/svanegas/coffeestar/bea3f3c7415489a4921b18befadcbf385e9d9f36/Practice%202/images/tablesCashBox.png)
