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

/* Maximum Voltage to be measured is 30V, using 10-bit resolution = 30/1024 */
#define f_VOLT_PER_BIT (float) 0.0292968 
 /* #define f_VOLT_PER_BIT (float) 0.00488281*/
/************************************************************************************************************************************************************************************************************/

/************************************************************************************************************************************************************************************************************
***************************************************************************** G L O B A L    V A R I A B L E S **********************************************************************************************
************************************************************************************************************************************************************************************************************/

/* Create an instance of 7segment display, this function also set-s the pins to OUTPUT */
TM1637 tm1637( ub_CLK_PIN, ub_CLK_DIO );


float f_LastMeasuredVoltage = 0;

/************************************************************************************************************************************************************************************************************/

/************************************************************************************************************************************************************************************************************
**************************************************************************** F U N C T I O N    D E C L A R A T I O N S *************************************************************************************
************************************************************************************************************************************************************************************************************/

/* This function will measure a voltage using the ADC and it will return it in float data type . 
   It works correctly when the ADC is set to 10-bit resolution and ALDAR is set to 0. */
float f_MeasureVoltage( void );

/* This function is responsable for displaying the voltage to the HW-069 module, its input is a float */
void v_DisplayVoltage( float f_Voltage );

/* This function will do the setup of the ADC1, with 10-bit resolution. */
void v_ADC_SetUp( void );

/************************************************************************************************************************************************************************************************************/

void setup() 
{
  /* Initialize the object */
  tm1637.init();
  
  /* Set the 7 segment LED-s brightness */
  tm1637.set( BRIGHT_TYPICAL );

  /* Set up the ADC1(using 10-bit resolution) */
  v_ADC_SetUp();
  
}


void loop() 
{
  
  v_DisplayVoltage( f_MeasureVoltage() );
  delay( 50 );
  
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

  f_ReadVoltage *= f_VOLT_PER_BIT;

  float f_RetVal;

  if( ( ( f_ReadVoltage - f_LastMeasuredVoltage ) * ( f_ReadVoltage - f_LastMeasuredVoltage ) ) <= 0.0025 )
  {
    f_RetVal = f_LastMeasuredVoltage;
  }
  else
  {
    f_LastMeasuredVoltage = f_ReadVoltage;
  }

  f_RetVal = f_LastMeasuredVoltage;
  
  return f_RetVal;
}



void v_DisplayVoltage( float f_Voltage )
{
  /* This variable will hold the 4 digits that will be displayed like this: xx.yy [Volt]. */
  uint8_t aub_TimeDisp[4];

  /* This variable holds the whole part of the measured voltage. */
  uint8_t ub_WholePart = (uint8_t)( f_Voltage );

  /* Substract the whole part from the measured voltage. */
  f_Voltage -= ub_WholePart;

  /* Extract the two most significant digits from the remainder. */
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

  for(int i = 0; i<4; i++)
  {
    tm1637.display(i, aub_TimeDisp[i]);
  }
  
}

void v_ADC_SetUp( void )
{
  /* Use internal 2.56V reference with external bypass capacitor at PB0(AREF)pin */
  ADMUX |= ( 1 << REFS0 );
  ADMUX |= ( 1 << REFS1 );
  ADMUX |= ( 1 << REFS2 );
  
  /* Configure ADC1 as input channel */
  ADMUX |= ( 1 << MUX0 );

  /* Left adjust the ADCH and ADCL registers, in this case you can just read the ADCH ang get a 8-bit value */
  /* ADMUX |= ( 1 << ADLAR ); */

  /* The previous 3 lines of code could be set like this:
  ADMUX = 0b10100001
  */

  /* Enable the ADC */
  ADCSRA |= ( 1 << ADEN );

  /* Set ADC division factor to 8 (the chip is set to tun at 1Mhz, so the ADC frequency will be 125kHz)*/
  ADCSRA |= ( 1 << ADPS0 );
  ADCSRA |= ( 1 << ADPS1 );
}

/************************************************************************************************************************************************************************************************************/
