#define  F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int size = 0;
int mode = 0;
int cnt, max, min, Dweight;
int adc[8];
int error, sensorRef, sensorData;

void Timer_Init(void);

ISR(INT0_vect)
{
	size = 1;		// 최대, 최소 받기
}

ISR(INT1_vect)
{
	mode = 1;			// 주행 시작
}

ISR(TIMER0_OVF_vect)
{
	ADMUX = 0x40;
	ADCSRA = 0x87;
	ADCSRA |= (1<<ADSC);
	
	while(!(ADCSRA & (1<<ADIF)));
	ADC = adc[8];
	
	if(size == 1)		// 최대, 최소 받기
	{
		for(int i = 0; i < 7; i++)
		{
			if (adc[i] < adc[i+1])
				max = adc[i+1];
			else 
				max = adc[i];
		}
		
		for(int j = 0; j < 7; j++)
		{
			if (adc[j] > adc[j+1])
				min = adc[j+1];
			else
				min = adc[j];
		}
	}
	
	if(mode == 1)		//주행시작
	{
		double ndata = (adc[8] - min) / (max - min) * 100;
		cnt++;
		TCNT0 = 100;
		int totalweight;
		TCCR1A = (1<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(0<<COM1B0)|(1<<WGM11);
		TCCR1B = (1<<WGM13)|(1<<WGM12)|(0<<CS02)|(0<<CS01)|(1<<CS00);
		ICR1 = 799;
		
		if(cnt == 1)
		{
			PORTE = 0x0A;
			int weight[8] = {-4, -3, -2, -1, 1, 2, 3, 4};
			if (PORTF = 0b11000000) 
				{totalweight = ndata * (weight[0] + weight[1]);}
			else if (PORTF = 0b01100000)
				{totalweight = ndata * (weight[1] + weight[2]);}
			else if (PORTF = 0b00110000)
				{totalweight = ndata * (weight[2] + weight[3]);}
			else if (PORTF = 0b00011000)
				{totalweight = ndata * (weight[3] + weight[4]);}
			else if (PORTF = 0b00001100)
				{totalweight = ndata * (weight[4] + weight[5]);}
			else if (PORTF = 0b00000110)
				{totalweight = ndata * (weight[5] + weight[6]);}
			else if (PORTF = 0b00000011)
				{totalweight = ndata * (weight[6] + weight[7]);}
			else 
				{OCR1A == 0, OCR1B == 0;}
				
			OCR1A = 159 + totalweight;
			OCR1B = 159 - totalweight;
			cnt = 0;
		}
	}
}

int main(void)
{
	DDRB = 0xFF;
	DDRD = 0xFF;
	DDRE = 0x0F;
	DDRF = 0x00;
	TCCR0 = (0<<WGM01)|(0<<WGM00)|(0<<COM01)|(0<<COM00)|(1<<CS02)|(1<<CS01)|(1<<CS00);
	TIMSK = (1<<TOIE0);	
	EIMSK= (1<<INT0)|(1<<INT1);
	EICRA= (1<<ISC11)|(0<<ISC10)|(1<<ISC01)|(0<<ISC00);
	sei();
	while(1);
}






