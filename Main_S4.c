#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "I2C.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"
#include "utils.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config CCP2MX = PORTBE

#define KEY_PRESSED PORTCbits.RC4                       // Definition for KEY_PRESSED as PORTC RC4

void test_alarm();
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char fan_set_temp = 75;
unsigned char Nec_state = 0;
float volt;
char INT1_flag, INT2_flag;

short nec_ok = 0;
char Nec_code;
char Nec_Button;
char FAN;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;
int alarm_mode, MATCHED,color;



char buffer[31]     = " ECE3301L Sp'23 Final\0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Fan_Set_Temp_Txt[] = "075F";
char Fan_SW_Txt[]   = "OFF";                // text storage for Heater Mode
char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};

    
char DC_Txt[]       = "000";                // text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";
char RTC_ALARM_Txt[]= "0";                      //
char RPM_Txt[]      = "0000";               // text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";
char setup_alarm_time[] = "00:00:00"; 
char setup_fan_set_text[]   = "075F";


void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    Init_ADC();
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1=0x0E;
    TRISA = 0x11;
    TRISB = 0x07;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;
    PORTE = 0x00;

    FAN = 0;
    RBPU=0;

    TMR3L = 0x00;                   
    T3CON = 0x03;
    I2C_Init(100000); 

    DS1621_Init();
    init_INTERRUPT();
    Turn_Off_Fan();
    fan_set_temp = 75;
    ALARMEN = 0;
    Set_RGB_Color(0);
}


void main() 
{
    Do_Init();                                                  // Initialization  
    Initialize_Screen();  
    DS3231_Turn_Off_Alarm();                                    
    DS3231_Read_Alarm_Time();                                   // Read alarm time for the first time

    tempSecond = 0xff;
    while (1)
    {
        DS3231_Read_Time();

        if(tempSecond != second)
        {
            tempSecond = second;
            rpm = get_RPM();
            volt = read_volt();
            DS1621_tempC = DS1621_Read_Temp();
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;

            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
            printf ("alarm = %d match = %d", RTC_ALARM_NOT, MATCHED);
            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
            
            Monitor_Fan();
            test_alarm();
            Set_RPM_RGB(rpm);
            Update_Screen();
        }
        
        found = 0xff;
        for (int j=0; j< 21; j++)
        {
            if (Nec_Button == array1[j]) 
            {
                found = j;
                j = 21;
            }
        }
            
        if (found == 0xff) 
        {
            printf ("Cannot find button \r\n");
            return;
        }
        else
        {
            printf ("Key Location = %d \r\n\n", found+1);// Print the key location of button
            KEY_PRESSED = 1;                        // Key pressed LED will turn on
            Activate_Buzzer();                      // Activate the buzzer
            Wait_One_Second();                      // wait one second
            Deactivate_Buzzer();                    // Then deactiate buzzer
            KEY_PRESSED = 0;                        // Now Key pressed LED will be off
		
            nec_ok = 0;
            switch (found)
            {
                case Setup_Time_Key:        
                    Do_Beep_Good();
                    Do_Setup_Time();
                    break;
                
                case Setup_Alarm_Key:           
                    Do_Beep_Good();
                    Do_Setup_Alarm_Time();
                    break;
                    
                case Setup_Fan_Temp_Key:
                    Do_Beep_Good();
                    Setup_Temp_Fan();            
                    break;
                    
                case Toggle_Fan_Key:
                    Do_Beep_Good();
                    Toggle_Fan();
                    break;           
        
                default:
                    Do_Beep_Bad();
                    break;
            }
        } 
		
        if (INT1_flag == 1)
        {
            INT1_flag = 0;
			Toggle_Fan();
        }
		
		if (INT2_flag == 1)
        {
            INT2_flag = 0;
            if (ALARMEN == 0) ALARMEN = 1;
            else ALARMEN = 0;			
           
        }
        
    }
}

void test_alarm()
{
    
    
     if (alarm_mode == 0 && ALARMEN == 1)                        // turn on alarm if alarm mode is off but enable is flagged
    {
        DS3231_Turn_On_Alarm();
        alarm_mode = 1;
        MATCHED = 0;
    }
    else if (alarm_mode == 1 && ALARMEN == 0)                   // turn off alarm if alarm mode is on but enable is clear
    {
        DS3231_Turn_Off_Alarm();
        alarm_mode = 0;
        Set_RGB_Color(0);
        Deactivate_Buzzer();
        MATCHED = 0;
    }
    else if (alarm_mode == 1 && ALARMEN == 1)                   // alarm check if alarm mode and enable is 1
    {
        if (RTC_ALARM_NOT == 0)
        {
            MATCHED = 1;
            Activate_Buzzer_4KHz();
        }
        if (MATCHED == 1)
        {
            Set_RGB_Color(color);
            color++;
            if (color > 7) color = 0;
            if (volt > 3.0)
            {
                MATCHED = 0;
                alarm_mode = 0;
                Deactivate_Buzzer();
                Set_RGB_Color(0);
                do_update_pwm(duty_cycle);
            }
        }
    }
}






