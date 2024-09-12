#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>
    
#include "I2C.h"                                        // include header files
#include "I2C_Support.h"                                //
#include "Interrupt.h"                                  //
#include "Fan_Support.h"                                //
#include "Main.h"                                       //
#include "ST7735_TFT.h"                                 //
#include "utils.h"                                      //

#pragma config OSC = INTIO67                            // configurations
#pragma config BOREN =OFF                               // mode off
#pragma config WDT=OFF                                  // mode off
#pragma config LVP=OFF                                  // mpde off

#define KEY_PRESSED PORTCbits.RC4                       // Definition for KEY_PRESSED as PORTC RC4

void test_alarm();                                      // declare test alarm
char second = 0x00;                                     // clearing all the time
char minute = 0x00;                                     // minute
char hour = 0x00;                                       // hour
char dow = 0x00;                                        // dow
char day = 0x00;                                        // day
char month = 0x00;                                      // month
char year = 0x00;                                       // year
char found;                                             // found variable
char tempSecond = 0xff;                                 // Initializing tempSecond to 0xff
signed int DS1621_tempC, DS1621_tempF;                  // Variables to store temperature in Celsius and Fahrenheit from DS1621 sensor
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year; // Variables for setting up time
char alarm_second, alarm_minute, alarm_hour, alarm_date;// Variables for setting up alarm time
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour; // Variables for setting up alarm time during setup
unsigned char fan_set_temp = 75;                        // Initialize set temperature for the fan
unsigned char Nec_state = 0;                            // NEC protocol state variable
float volt;                                             // Variable to store voltage
char INT1_flag, INT2_flag;                              // Interrupt flags
short nec_ok = 0;                                       // NEC protocol flag
char Nec_Button;                                        // NEC button variable
char FAN;                                               // Fan variable
char duty_cycle;                                        // Duty cycle variable
int rps;                                                // Revolutions per second
int rpm;                                                // Revolutions per minute
int ALARMEN;                                            // Alarm enable flag
int alarm_mode, MATCHED, color;                         // Variables related to alarm mode and matching
void test_alarm();                                      // Function declaration for testing the alarm
                                                        // Strings and text storage
char buffer[31] = " ECE3301L Fall'23 Final\0";          //
char *nbr;                                              //
char *txt;                                              //

                                                        // Text representations
char tempC[] = "+25";                                   //
char tempF[] = "+77";                                   //
char time[] = "00:00:00";                               //
char date[] = "00/00/00";                               //
char alarm_time[] = "00:00:00";                         //
char Alarm_SW_Txt[] = "OFF";                            // Text representation for Alarm Switch
char Fan_Set_Temp_Txt[] = "075F";                       // Text representation for Fan Set Temperature
char Fan_SW_Txt[] = "OFF";                              // Text representation for Fan Switch

                                                        // Array of characters
char array1[21] = {0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};
                                                        // Text representations
char DC_Txt[] = "000";                                  //
char Volt_Txt[] = "0.00V";                              //
char RTC_ALARM_Txt[] = "0";                             //
char RPM_Txt[] = "0000";                                //
char setup_time[] = "00:00:00";                         //
char setup_date[] = "01/01/00";                         //
char setup_alarm_time[] = "00:00:00";                   //
char setup_fan_set_text[] = "075F";                     //

void putch (char c)                                     // putch function
{       
    while (!TRMT);                                      // if not TRMT
    TXREG = c;                                          // set TXREG to c
}

void init_UART()                                        //Initializing  UART function
{   
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;                                      // 8Mhz
}

void Do_Init()                                          // Initialize the ports 
{ 
    init_UART();                                        // Initialize the UART
    Init_ADC();                                         // Initialize ADC
    OSCCON=0x70;                                        // Set oscillator to 8 MHz 
    
    ADCON1=0x0E;                                        // Set ADCON1 to 0x0E
    TRISA = 0x11;                                       // Set PORT A bits 0 and 4 as inputs(rest as outputs)
    TRISB = 0x07;                                       // Set PORT B bits 3 as input rest output
    TRISC = 0x01;                                       // Set PORT C last 7 bits as output rest as input
    TRISD = 0x00;                                       // Set PORT E 
    TRISE = 0x00;                                       // set PORT E as output 
    PORTE = 0x00;                                       // Clear PORT E

    FAN = 0;                                            // set fan to 0
    RBPU = 0;                                           // set RBPU to 0

    TMR3L = 0x00;                                       // clear timer
    T3CON = 0x03;                                       // set T3CON to 0x03
    I2C_Init(100000);                                   // initialize with that value

    DS1621_Init();                                      // Initialize DS1621
    init_INTERRUPT();                                   // Initialize interrupt
    Turn_Off_Fan();                                     // turn off fan
    fan_set_temp = 75;                                  // set initial set temp to 75
    ALARMEN = 0;                                        // set alarm enable to 0
    Set_RGB_Color(0);                                   // set RGB to 0
}

void main()                                             // main function
{
    Do_Init();                                          // Initialization  
    Initialize_Screen();                                // Initialize screen
    DS3231_Turn_Off_Alarm();                            // turn off alarm        
    DS3231_Read_Alarm_Time();                           // Read alarm time for the first time
    tempSecond = 0xff;                                  // Initialize tempSecond to 0xff

while (1)                                               // Infinite loop
{
    DS3231_Read_Time();                                 // Read current time from DS3231
    if (tempSecond != second)                           // Check if a second has passed
    {
        tempSecond = second;                            // equal to second
        rpm = get_RPM();                                // Get revolutions per minute
        volt = read_volt();                             // Read voltage
        DS1621_tempC = DS1621_Read_Temp();              // Read temperature in Celsius from DS1621
        DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;     // Convert temperature to Fahrenheit

                                                        // Print time, temperature, alarm status, RPM, and duty cycle on  TeraTerm
        printf ("%02x:%02x:%02x %02x/%02x/%02x", hour, minute, second, month, day, year);   //
        printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);                        //
        printf ("alarm = %d match = %d ", RTC_ALARM_NOT, MATCHED);                          //
        printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);                                  //

        Monitor_Fan();                                 // call Monitor fan 
        test_alarm();                                  // call Test alarm 
        Set_RPM_RGB(rpm);                              // call Set RGB LED color based on RPM
        Update_Screen();                               // call Update LCD screen
    }

    if (nec_ok == 1)                                   // If NEC is equal to 1
    {
        nec_ok = 0;                                    // Set NEC to 0

        INTCONbits.INT0IE = 1;                         // Enable external interrupt
        INTCON2bits.INTEDG0 = 0;                       // Edge programming for INT0 falling edge
        printf ("Nec_Button = %x\r\n", Nec_Button);    // Print NEC button

        found = 0xff;                                  // Initialize found to 0xff
        for (int j = 0; j < 21; j++)                   // Loop through array1
        {
            if (Nec_Button == array1[j])               // If NEC button matches array1
            {
                found = j;                             // Set found(button) to the loop number
                j = 21;                                // Set j to 21 to exit the loop
            }
        }

        if (found == 0xff)                             // If found is 0xff
        {
            printf ("Cannot find button \r\n");        // Print that the button couldn't be found
            return;                                    // Exit the loop
        }
        else                                           // else
        {
            printf ("Key Location = %d \r\n\n", found + 1);  // Print the key location of the button
            KEY_PRESSED = 1;                           // Key pressed LED will turn on
            Activate_Buzzer();                         // Activate the buzzer
            Wait_One_Sec();                            // Wait for one second
            Deactivate_Buzzer();                       // Deactivate the buzzer
            KEY_PRESSED = 0;                           // Key pressed LED will turn off
            nec_ok = 0;                                // Reset nec_ok

            switch (found)                             // switch statement 
            {
                case Setup_Time_Key:                   // when setup time key pressed
                    Do_Beep_Good();                    // beep good
                    Do_Setup_Time();                   // call setup time function
                    break;                             //

                case Setup_Alarm_Key:                  // When setup alarm key is pressed
                    Do_Beep_Good();                    // beep good
                    Do_Setup_Alarm_Time();             // call setup alarm time
                    break;                             //

                case Setup_Fan_Temp_Key:               // if setup fan temp key pressed 
                    Do_Beep_Good();                    // beep good
                    Setup_Temp_Fan();                  // call setup temp fan
                    break;                             //
                    
                case Toggle_Fan_Key:                   // when toggle fan key pressed
                    Do_Beep_Good();                    // Do beep good
                    Toggle_Fan();                      // Call toggle fan
                    break;                             //

                default:                               // default
                    Do_Beep_Bad();                     // beep bad when unrecognized button pressed
                    break;                             //
            }
        }
    }

    if (INT1_flag == 1)                                // If INT1_flag is 1
    {
        printf("HEATER Toggled!");                     // Print HEATER toggled message
        INT1_flag = 0;                                 // Reset INT1_flag
        Toggle_Fan();                                  // Toggle the fan
    }

    if (INT2_flag == 1)                                // If INT2_flag is 1
    {
        printf("ALARM Toggled!");                      // Print ALARM toggled message
        if (ALARMEN == 0) ALARMEN = 1;                 // Toggle ALARMEN
        else ALARMEN = 0;                              // else alarm enable is 0
        INT2_flag = 0;                                 // Reset INT2_flag
    }
}
}

void test_alarm()                                      // Test alarm function                                  
{
    if (alarm_mode == 0 && ALARMEN == 1)               // Check if alarm mode is off but enable is flagged
    {   
        DS3231_Turn_On_Alarm();                        // Function to enable alarm in DS3231
        alarm_mode = 1;                                // Set alarm mode to on
        MATCHED = 0;                                   // Reset the matched flag
    }
    else if (alarm_mode == 1 && ALARMEN == 0)          // Check if alarm mode is on but enable is clear
    {
        DS3231_Turn_Off_Alarm();                       // Function to disable alarm in DS3231
        alarm_mode = 0;                                // Set alarm mode to off
        Set_RGB_Color(0);                              // Set RGB LED color to 0 (no color)
        Deactivate_Buzzer();                           // Deactivate the buzzer
        MATCHED = 0;                                   // Reset the matched flag
    }
    else if (alarm_mode == 1 && ALARMEN == 1)          // Check if alarm mode and enable are both 1
    {
        if (RTC_ALARM_NOT == 0)                        // Check if RTC_ALARM_NOT signal is low (active)
        {
            MATCHED = 1;                               // Set the matched flag
            Activate_Buzzer_4KHz();                    // Function to activate a 4 kHz buzzer
        }
        
        if (MATCHED == 1)                              // Check if matched flag is set
        {
            Set_RGB_Color(color);                      // Set RGB LED color based on the 'color' variable
            color++;                                   // Increment the color variable
            if (color > 7) color = 0;                  // Reset the color variable if it exceeds 7

            if (volt > 3.0)                            // Check if voltage is greater than 3.0
            {
                MATCHED = 0;                           // Reset the matched flag
                alarm_mode = 0;                        // Set alarm mode to off
                Deactivate_Buzzer();                   // Deactivate the buzzer
                Set_RGB_Color(0);                      // Set RGB LED color to 0 (no color)
                do_update_pwm(duty_cycle);             // Function to update PWM with duty cycle
            }
        }
    }
}
