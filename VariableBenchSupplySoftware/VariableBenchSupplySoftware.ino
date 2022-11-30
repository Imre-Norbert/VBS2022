/************************************************************************************************************************************************************************************************************
************************************************************************************** I N C L U D E S ******************************************************************************************************
************************************************************************************************************************************************************************************************************/

#include <TM1637.h>
#include <avr/io.h>

/************************************************************************************************************************************************************************************************************/

/************************************************************************************************************************************************************************************************************
************************************************************************************** D E F I N E S ********************************************************************************************************
************************************************************************************************************************************************************************************************************/

/* This is used to set the output[pin2 on the ic] of the attiny HIGH or LOW, to address the PORTB register
   This pin is connected to the CLK input pin of the HW-069*/
#define ub_CLK_PIN (uint8_t) 3
/* This is used to set the output[pin3 on the ic] of the attiny HIGH or LOW, to address the PORTB register
   This pin is connected to the DIO input pin of the HW-069*/ 
#define ub_CLK_DIO (uint8_t) 4

/* This is used to read the supply voltage, ADC1 [pin7 on the ic]*/
#define ub_VOLTAGE_MEASUREMENT (uint8_t) 2

/* Maximum Voltage to be measured is 30V, using 8-bit resolution = 30/255 */
#define f_VOLT_PER_BIT (float) 0.0293
/************************************************************************************************************************************************************************************************************/

/************************************************************************************************************************************************************************************************************
***************************************************************************** G L O B A L    V A R I A B L E S **********************************************************************************************
************************************************************************************************************************************************************************************************************/

/* Create an instance of 7segment display, this function also set-s the pins to OUTPUT */
TM1637 tm1637( ub_CLK_PIN, ub_CLK_DIO );

/************************************************************************************************************************************************************************************************************/

/************************************************************************************************************************************************************************************************************
**************************************************************************** F U N C T I O N    D E C L A R A T I O N S *************************************************************************************
************************************************************************************************************************************************************************************************************/

/* This function will measure the voltage using an analog input pin, Its input is the pin number, and it will output the measured voltage in [V]*/
float f_MeasureVoltage( void );

/* This function is responsable for displaying the voltage to the HW-069 module, its input is a float */
void v_DisplayVoltage( float f_Voltage );

/************************************************************************************************************************************************************************************************************/

void setup() 
{
  /* Initialize the object */
  tm1637.init();
  
  /* Set the 7 segment LED-s brightness */
  tm1637.set( BRIGHT_TYPICAL );

  /**************************************************/
  /*************** Setting up the ADC ***************/
  /**************************************************/
  
  /* Use internal reference voltage of 1.1V 
     Anything above this will be read as MAXIMUM */
  ADMUX |= ( 1 << REFS1 );

  /* Configure ADC1 as input channel */
  ADMUX |= ( 1 << MUX0 );

  /* Left adjust the ADCH and ADCL registers to select 8-bit resolution */
  //ADMUX |= ( 1 << ADLAR );

  /* The previous 3 lines of code could be set like this:
  ADMUX = 0b10100001
  */

  /* Enable the ADC */
  ADCSRA |= ( 1 << ADEN );

  /* Set ADC division factor to 8 (the chip is set to tun at 1Mhz, so the ADC frequency will be 125kHz)*/
  ADCSRA |= ( 1 << ADPS0 );
  ADCSRA |= ( 1 << ADPS1 );
  
  /**************************************************/
}


void loop() 
{
  
  v_DisplayVoltage( f_MeasureVoltage() );
  delay( 500 );
  
}

/************************************************************************************************************************************************************************************************************
**************************************************************************** F U N C T I O N S **************************************************************************************************************
************************************************************************************************************************************************************************************************************/

float f_MeasureVoltage( void )
{
  /* Start ADC conversion */
  ADCSRA |= ( 1 << ADSC );
  
  /* Store the analog data to a variable 
     In single conversion mode the conversion needs to be started but, when it has collected 
     the data it will turn itself off automatically, the data will remain static untill read out or
     overwritten */
  /* Variable for holding the 2 LSB-s from the 10-bit number(ADC) */
  uint8_t ub_LowADC = ADCL;
  /* Variable for holding the 8 MSB-s from the 10-bit number(ADC) */
  uint16_t uw_HighADC = ( ADCH << 8 ) | ( ub_LowADC );
  
  float f_ReadVoltage = (float)( uw_HighADC );
  return f_ReadVoltage * f_VOLT_PER_BIT;
}



void v_DisplayVoltage( float f_Voltage )
{
  uint8_t aub_TimeDisp[4];
  
  uint8_t ub_WholePart = (uint8_t)( f_Voltage );
  
  f_Voltage -= ub_WholePart;
  
  uint8_t ub_Remainder = (uint8_t)( f_Voltage * 100 );

  if( ub_WholePart >= 10 )
  {
    aub_TimeDisp[ 0 ] = ub_WholePart / 10 ;
    aub_TimeDisp[ 1 ] = ub_WholePart % 10 ;
  }
  else
  {
    aub_TimeDisp[ 0 ] = 0;
    aub_TimeDisp[ 1 ] = ub_WholePart;
  }

  if( ub_Remainder >= 10 )
  {
    aub_TimeDisp[ 2 ] = ub_Remainder / 10 ;
    aub_TimeDisp[ 3 ] = ub_Remainder % 10;
  }
  else
  {
    aub_TimeDisp[ 2 ] = 0;
    aub_TimeDisp[ 3 ] = ub_Remainder;
  }

  tm1637.display(aub_TimeDisp);
  
}

/************************************************************************************************************************************************************************************************************/
