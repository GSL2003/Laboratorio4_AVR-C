//******************************************************************************************************************
//UNIVERSIDAD DEL VALLE DE GUATEMALA
//IE2023: PROGRAMACIÓN DE MICROCONTROLADORES
//Archivo: Lab_4
//AUTOR: Guillermo José Schwartz López
//Laboratorio#4
//HARDWARE: ATMEGA328P
//CREADO: 9/04/2024
//ÚLTIMA MODIFICACIÓN: 12/04/2024 - 15:32
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
void b_inter(void);			//Función que contine la configuración inicial de las Interrupciones

void ADC_IN(void);			//Función que contiene la configuración inicial del contador ADC
void dec_hex(uint8_t);		//Función que convierte los decimales a hexadecimales

//******************************************************************************************************************
//DISPLAY DE 7 SEGMETNOS
//******************************************************************************************************************

unsigned char DISP_HEX[16]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
const unsigned int DL_MX = 5;		//Se define la variables para utilizarla como DELAY en los displays

//******************************************************************************************************************

// -> Se definen las variables

int Bi_contador = 1;					// Variable para el contador binario
int ADC_contador = 0;					// Variable para el contador ADC - DIP_7SEG
int LOWB_4 = 0;							// Variable para almacenar los primer 4 digitos
int HIGHB_4 = 0;						// Variable para almacenar los pultimos 4 digitos

//Variables para las interrupciones
unsigned char Boton1_LS1 = 1;			//Unsigned char -> Número entero entre el invervalo [0:255]
unsigned char Boton1_S1 = 1;
unsigned char Boton2_LS2 = 2;
unsigned char Boton2_S2 = 1;

//******************************************************************************************************************
//LOOP
//******************************************************************************************************************

int main(void)
{
	cli();			//Se apagan las interrupciones Globales
	
	//-> Se llaman a las funciones para integrarse a la función principlañ --- LOOP
	ADC_IN();
	setup();
	b_inter();
	
	sei();			//Se activan las interrupciones globales
	
	while(1)
	{
		if (Bi_contador > 255){
			Bi_contador = 0;
		}
		if (Bi_contador < 0){
			Bi_contador = 255;
		}
		
		//-> Configuración de las LEDS
		PORTB = Bi_contador;			//Se muestra el contador en las salidas PB0 a PB5	
			
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
		
		// -> Se inicia la secuencia del ADC
		ADCSRA |= (1<<ADSC);
		
		// Se configura la salida del Primer Display //
		dec_hex(ADC_contador);
		PORTD = DISP_HEX[LOWB_4];
		
		//Configuración de los transistores
		PORTC |= (1<<PC4);						//Se activa el transistor del primer Display
		PORTC &= ~(1<<PC3);						//Se apaga el transistor del segundo Display
		_delay_ms(DL_MX);			
		
		
		// Se configura la salida del Segundo Display //
		
		dec_hex(ADC_contador);
		PORTD = DISP_HEX[HIGHB_4];

		//Configuración de los transistores
		PORTC |= (1<<PC3);					//Se apaga el transistor del primer Display
		PORTC &= ~(1<<PC4);					//Se activa el transistor del segundo Display
		_delay_ms(DL_MX);
		
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
	DDRB = 0x3F;
	PORTB = 0;
	
	//Configurar PC2, PC3, PC4 y PC5 como salidas - No afectar las entradas de PC0 y PC1
	DDRC |= (1<<PC2)|(1<<PC3)|(1<<PC4)|(1<<PC5);
	
	// Configuración de las entradas//

	// Configurar PC0 y PC1 como entradas
	DDRC &= ~((1 << PC0) | (1 << PC1));

	// Habilitar resistencias de PULL-UP para PC0 y PC1
	PORTC |= (1 << PC0) | (1 << PC1);
	
}

void b_inter(void){
	// INTERRUPCION PARA PUERTO C
	PCICR |= (1<<PCIE1);					//PCIE1 -  PCINT[14:8] -> equivale de PC0 a PC6
	// HABILITAR PARA PCINT1
	PCMSK1 |= ((1<<PCINT8)|(1<<PCINT9));	//Al trabajar en PCIE1 se debe usar PCMSK1
}

void ADC_IN(void){
	ADMUX |= 0x07;			// Se habilita la entrada A7 como una entrada analogica (ADC[7])
	
	// Configuración de la referencia AVCC
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	
	// Implementado la justificación a la Izquierda
	ADMUX |= (1<<ADLAR);
	ADCSRA = 0;
	
	// Se enciende el ADC
	ADCSRA |= (1<<ADEN);
	
	// Se habilita ISR ADC
	ADCSRA |= (1<<ADIE);
	
	// Configuración del Prescaler > 16M/128 = 125KHz   -> Es necesario ya que el ADC debe funcionar a partir de 8 bits
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	// Se desabilita la entrada digital PC0
	DIDR0 |= (1<<ADC0D);
}

void dec_hex(uint8_t val){
	HIGHB_4 = val/16;				// Determina los primeros 4 bits
	LOWB_4 = val%16;				// El rresiduo de la división se almnacena en la variable 
}


//***************************************************************************
// ISR
//***************************************************************************
ISR(ADC_vect){
	ADC_contador = ADCH;
	ADCSRA |= (1<<ADIF);			//Al momento de activar el ADC, se debe apagar la bandera
}

ISR(PCINT1_vect){
	Boton1_S1 = PINC&(1<<PINC0);				//Se configura la entrada del boton PC0
	Boton1_S1 = PINC&(1<<PINC0);				//Se configura la entrada del boton PC0
	Boton2_S2 = PINC&(1<<PINC1);
	
	if ((Boton1_S1 == 0)&&(Boton1_LS1 == 1)){			//Si se cumple la condición, el contador aumenta//Si se cumple la condición, el contador aumenta
		Boton1_LS1 = 0;
		Bi_contador ++;
	}
	
	if (Boton1_S1 == 1){
		Boton1_LS1 = 1;
	}
	
	if ((Boton2_S2 == 0)&&(Boton2_LS2 == 2)){			//Si se cumple la condición, el contador aumenta//Si se cumple la condición, el contador disminuye
		Boton2_LS2 = 0;
		Bi_contador --;
	}
	
	if (Boton2_S2 == 2){
		Boton2_LS2 = 2;
	}
}

