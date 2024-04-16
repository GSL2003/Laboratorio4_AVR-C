//******************************************************************************************************************
//UNIVERSIDAD DEL VALLE DE GUATEMALA
//IE2023: PROGRAMACIÓN DE MICROCONTROLADORES
//Archivo: Prelab_3
//AUTOR: Guillermo José Schwartz López
//Laboratorio#3
//HARDWARE: ATMEGA328P
//CREADO: 8/04/2024
//ÚLTIMA MODIFICACIÓN: 9/04/2024 - 13:02
//******************************************************************************************************************

//******************************************************************************************************************
//LIBRERIAS
//******************************************************************************************************************
#define F_CPU 16000000		// Definir CPU con el que trabajara el microcontrolador -	Debe estar al inicio de la progra

#include <avr/io.h>			// Se incluye la libreria para poder utilizar los puertos del microcontrolador
#include <util/delay.h>		// Se incluye la libreria que implementa los delay
#include <avr/interrupt.h>	// Se incluye la libreria que permite trabajar con interrupciones

//******************************************************************************************************************
//CONFIGURACIÓN
//******************************************************************************************************************

// -> Declarar las funciones que se implementaran a lo largo de la programación

void setup(void);			//Función que contiene la configuración de entradas y salidas
void b_inter(void);


// -> Se definen las variables

int Bi_contador = 1;					// Variable para el contador binario

unsigned char Boton1_LS1 = 1;			//Unsigned char -> Número entero entre el invervalo [0:255]
unsigned char Boton1_S1 = 1;
unsigned char Boton2_LS2 = 2;
unsigned char Boton2_S2 = 1;

int main(void)
{
	setup();
	b_inter();
	
	sei();
	
	while(1)
	{
		if (Bi_contador > 255){
			Bi_contador = 0;
		}
		if (Bi_contador < 0){
			Bi_contador = 255;
		}
			
		PORTB = Bi_contador;
			
		// -> Configuración del PD7 y PC2
		
		if ((Bi_contador > 63)&&(Bi_contador < 128)){
			PORTC |= (1<<PC2);		//Se incluye "|" para indicar el PIN especifico del puerto C que se esta empelando 
		} else {
			PORTC &= ~(1<<PC2);		//Se incluye "&" para indicar el PIN especifico del puerto C que se esta apagando
		}

		if (Bi_contador > 127){
			PORTD |= (1<<PD7);		//Se incluye "|" para indicar el PIN especifico del puerto D que se esta empelando
		} else {
			PORTD &= ~(1<<PD7);		//Se incluye "&" para indicar el PIN especifico del puerto D que se esta apagando
		}
		
		if (Bi_contador > 191){
			PORTC |= (1<<PC2);		//Se incluye "|" para indicar el PIN especifico del puerto C que se esta empelando
			} else {
			PORTC &= ~(1<<PC2);		//Se incluye "&" para indicar el PIN especifico del puerto C que se esta apagando
		}
		
		if (Bi_contador < 0){
			PORTD &= ~(1<<PD7);
			PORTC &= ~(1<<PC2);
		}
		
	}
}


//******************************************************************************************************************
//FUNCIONES
//******************************************************************************************************************

void setup(void){
	//Configuración de salidas//
	
	// - Pines del puerto D
	DDRD = 0xFF;
	PORTD = 0;
	// Configurar Pines RX y TX para usarlas comos Salidas (PD0 - PD1)
	UCSR0B = 0;
	
	// - Pines del puerto B
	DDRB = 0b00111111;
	PORTB = 0;
	
	//Configurar PC2, PC3 y PC4 como salidas - No afectar las entradas de PC0 y PC1
	DDRC |= (1<<PC2)|(1<<PC3)|(1<<PC4);
	
	// Configuración de las entradas//

	// Configurar PC0 y PC1 como entradas
	DDRC &= ~((1 << PC0) | (1 << PC1));

	// Habilitar resistencias de PULL-UP para PC0 y PC1
	PORTC |= (1 << PC0) | (1 << PC1);
	
	/* Otra forma de declarar las entradas
	DDRC &= ~(1<<0);					//Configuración de los PULL-UP
	PORTC |= (1<<PC0);
	DDRC &= ~(1<<1);					//Configuración de los PULL-UP
	PORTC |= (1<<PC1);
	*/
	
}

void b_inter(void){
	// INTERRUPCION PARA PUERTO C
	PCICR |= (1<<PCIE1);					//PCIE1 -  PCINT[14:8] -> equivale de PC0 a PC6
	// HABILITAR PARA PCINT1
	PCMSK1 |= ((1<<PCINT8)|(1<<PCINT9));	//Al trabajar en PCIE1 se debe usar PCMSK1
}

ISR(PCINT1_vect){
	Boton1_S1 = PINC&(1<<PINC0);
	Boton2_S2 = PINC&(1<<PINC1);
	
	if ((Boton1_S1 == 0)&&(Boton1_LS1 == 1)){
		Boton1_LS1 = 0;
		Bi_contador ++;
	}
	
	if (Boton1_S1 == 1){
		Boton1_LS1 = 1;
	}
	
	if ((Boton2_S2 == 0)&&(Boton2_LS2 == 2)){
		Boton2_LS2 = 0;
		Bi_contador --;
	}
	
	if (Boton2_S2 == 2){
		Boton2_LS2 = 2;
	}
}

