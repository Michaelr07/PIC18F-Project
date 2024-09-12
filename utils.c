#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p18f4620.h>

#include "utils.h"                                  // Include header files
#include "Fan_Support.h"                            // 

#define KEY_PRESSED PORTCbits.RC4                   // Definition for KEY_PRESSED as PORTC RC4

#define OFF         0                               // Setting values for each color
#define RED         1                               //
#define GREEN       2                               //
#define YELLOW      3                               //
#define BLUE        4                               //
#define PURPLE      5                               //
#define CYAN        6                               //
#define WHITE       7                               //

extern char found;                                  // declaring external variables
extern short nec_ok;                                //
extern char Nec_Button;                             //
extern char array1[21];                             //
extern char duty_cycle;                             //

char check_for_button_input()                       // check for button pressed function
{    
    if (nec_ok == 1)                                // if nec ok is 1
    {   
            nec_ok = 0;                             // make it 0
            printf ("NEC_Code = %x\r\n",Nec_Button);// print message

            INTCONbits.INT0IE = 1;                  // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge

            found = 0xff;                           // found set to 0xff
            for (int j=0; j< 21; j++)               // For loop from o to 21
            {
                if (Nec_Button == array1[j])        // if nec matches array 
                {
                    found = j;                      // set found to j
                    j = 21;                         // 
                }   
            }

            if (found == 0xff)                      // check if button is found
            {
                printf ("Cannot find button \r\n"); // cannot find
                return (0);                         // return 0
            }
            else                                    // if not
            {
                printf ("Key Location = %d \r\n\n", found+1);// Print the key location of button
                KEY_PRESSED = 1;                    // Key pressed LED will turn on
                Activate_Buzzer();                  // Activate the buzzer
                Wait_One_Sec();                     // wait one second
                Deactivate_Buzzer();                // Then deactiate buzzer
                KEY_PRESSED = 0;                    // Now Key pressed LED will be off
            	nec_ok=0;                           // nec ok will be 0
                return (1);                         // return a 1
            }
    }
}
                                                    // Function to convert BCD to decimal
char bcd_2_dec (char bcd)                       
{
    int dec;
    dec = ((bcd >> 4) * 10) + (bcd & 0x0f);
    return dec;
}

                                                    // Function to convert decimal to BCD
int dec_2_bcd (char dec)
{
    int bcd;
    bcd = ((dec / 10) << 4) + (dec % 10);
    return bcd;
}

                                                    // Function to produce a beep sound
void Do_Beep()
{
    Activate_Buzzer();
    Wait_One_Sec();
    Deactivate_Buzzer();
    Wait_One_Sec();
    do_update_pwm(duty_cycle);
}

                                                    // Function to produce a good beep sound
void Do_Beep_Good()
{
    Activate_Buzzer_2KHz();
    Wait_One_Sec();
    Deactivate_Buzzer();
    Wait_One_Sec();
    do_update_pwm(duty_cycle);
}

                                                    // Function to produce a bad beep sound
void Do_Beep_Bad()
{
    Activate_Buzzer_500Hz();
    Wait_One_Sec();
    Deactivate_Buzzer();
    Wait_One_Sec();
    do_update_pwm(duty_cycle); 
}

                                                    // Function to wait for one second
void Wait_One_Sec()
{
    for (int k = 0; k < 0xffff; k++);
}

                                                    // Function to activate the buzzer with default frequency
void Activate_Buzzer()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

                                                    // Function to activate the buzzer with a frequency of 500Hz
void Activate_Buzzer_500Hz()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000111 ;
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

                                                    // Function to activate the buzzer with a frequency of 2KHz
void Activate_Buzzer_2KHz()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

                                                    // Function to activate the buzzer with a frequency of 4KHz
void Activate_Buzzer_4KHz()
{
    PR2 = 0b01111100 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b00111110 ;
    CCP2CON = 0b00011100 ;
}

                                                    // Function to deactivate the buzzer
void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
    PORTCbits.RC1 = 0;
}

                                                    // Function to update PWM signal with a specified duty cycle
void do_update_pwm(char duty_cycle) 
{ 
    float dc_f;                                     // float dc
    int dc_I;                                       // DC integer
    PR2 = 0b00000100 ;                              // Set the frequency for 25 kHz 
    T2CON = 0b00000111 ;                            // As given on the website
    dc_f = (4.0 * duty_cycle / 20.0) ;              // Calculate the factor of duty cycle versus a 25 kHz signal
    dc_I = (int) dc_f;                              // Truncate integer
    if (dc_I > duty_cycle) dc_I++;                  // Round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;    // 
    CCPR1L = (dc_I) >> 2;                           // ccpr1l
}

                                                    // Function to set D2 color
void Set_D2_Color(char color)
{
    switch (color)                                                              
    {
    case OFF: D2_RED = 0; D2_GREEN = 0; D2_BLUE = 0; break;                              
    case RED: D2_RED = 1; D2_GREEN = 0; D2_BLUE = 0; break;                              
    case GREEN: D2_RED = 0; D2_GREEN = 1; D2_BLUE = 0; break;                              
    case YELLOW: D2_RED = 1; D2_GREEN = 1; D2_BLUE = 0; break;                              
    case BLUE: D2_RED = 0; D2_GREEN = 0; D2_BLUE = 1; break;                              
    case PURPLE: D2_RED = 1; D2_GREEN = 0; D2_BLUE = 1; break;                              
    case CYAN: D2_RED = 0; D2_GREEN = 1; D2_BLUE = 1; break;                              
    case WHITE: D2_RED = 1; D2_GREEN = 1; D2_BLUE = 1; break;                              
    }
}
                                                    // Function to set RGB color using port D
void Set_RGB_Color(char color)
{
    char D1[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05 , 0x06, 0x07};
    PORTD = (PORTD & 0xF8) | D1[color];
}
                                                    // Function to read voltage using ADC
float read_volt()
{
    int nStep = get_full_ADC();
    float volt = nStep * 5 / 1024.0;
    return (volt);
}
                                                    // Function to get full ADC value
unsigned int get_full_ADC()
{
    unsigned int result;
    ADCON0bits.GO = 1;                     
    while (ADCON0bits.DONE == 1);           
    result = (ADRESH * 0x100) + ADRESL;  
    return result;                 
}
                                                    // Function to initialize ADC
void Init_ADC()
{
    ADCON0 = 0x01;
    ADCON1 = 0x0E; 
    ADCON2 = 0xA9;
}