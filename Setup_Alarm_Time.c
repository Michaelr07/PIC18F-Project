#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "I2C_Support.h"                                        // Include header files
#include "Setup_Alarm_Time.h"                                   //
#include "Setup_Time.h"                                         //
#include "Main_Screen.h"                                        //
#include "Main.h"                                               //
#include "ST7735_TFT.h"                                         //
#include "utils.h"                                              //

extern char setup_alarm_time[];                                 // delcaring external variables
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;        //
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;  //
extern char found;                                              //
extern char *txt;                                               //
extern short nec_ok;                                            //
                                                                // Declaring characters
char Select_Alarm_Field;                                        //
char In_Alarm_Time_Setup;                                       //

void Do_Setup_Alarm_Time(void)                                  // Setup Alarm time function
{
    Select_Alarm_Field = 0;                                     // Set alarm field to 0
    In_Alarm_Time_Setup = 1;                                    // Alarm time setup to 1
    DS3231_Read_Alarm_Time();                                   // Read alarm time
    alarm_second = bcd_2_dec(alarm_second);                     // convert to dec for use
    alarm_minute = bcd_2_dec(alarm_minute);                     // Convert minute to dec
    alarm_hour   = bcd_2_dec(alarm_hour);                       // Convert hour to dec
    setup_alarm_second = alarm_second;                          // Beginning setup alarm time as current set alarm time
    setup_alarm_minute = alarm_minute;                          // setup alarm minute to minute
    setup_alarm_hour = alarm_hour;                              // set setup alarm hour to hour
    Initialize_Setup_Alarm_Time_Screen();                       // Initialize the alarm setup screen
    Update_Setup_Alarm_Time_Screen();                           // Update the alarm screen with latest info
    while (In_Alarm_Time_Setup == 1)                            // While alarm time setup is 1
    {   
         if (check_for_button_input() == 1)                     // Check for button input and if its 1
         {
             if ((found == Prev) || (found == Next) || (found == Ch_Minus) || (found == Ch_Plus) || (found == Minus) || (found == Plus))    // check for certain buttons
             {
                Do_Beep_Good();                                 // Beep good if found
                if (found == Prev) Go_Prev_Alarm_Field();       // If Prev found call function
                if (found == Next) Go_Next_Alarm_Field();       // If Next found call function
                if (found == Ch_Plus) Do_Save_New_Alarm_Time(); // If found is Channel plus save new alarm time function
                if (found == Ch_Minus) Exit_Setup_Alarm_Time(); // If found is channel minus call exit setup funciton
                if (found == Plus) Increase_Alarm_Time();       // If found is plus increase alarm time fucntion
                if (found == Minus) Decrease_Alarm_Time();      // If found is Minus call decrease alarm time function
                found = 0xff;
             }  
             else                                               //
             {  
                 Do_Beep_Bad();                                 // Else beep bad
             }
        }                   
    }
}

void Increase_Alarm_Time()                                      // Increase ALarm time funciton
{
            switch (Select_Alarm_Field)                         // Increment the proper field
            {
                case 0:                                         // case 0
                    setup_alarm_hour++;                         // Setup alarm hour will be increased by 1
                    if (setup_alarm_hour >= 24) setup_alarm_hour  = 0;  // if hour is higher than 24 go back to 0
                    break;                                      //

               case 1:                                          // minute
                    setup_alarm_minute++;                       // increment minute
                    if (setup_alarm_minute >= 60) setup_alarm_minute = 0;   // if minute is 60 go back to 0
                    break;                                      //

                case 2:                                         // second
                    setup_alarm_second++;                       // increment seconds by 1
                    if (setup_alarm_second >= 60) setup_alarm_second = 0;   // if seconds is 60 go back to 0
                    break;                                      //
                    
                default:                                        //
                    break;                                      //
            }    
            Update_Setup_Alarm_Time_Screen();                   // Update alarm screen with latest info
}

void Decrease_Alarm_Time()
{       
            switch (Select_Alarm_Field)                         // Decrement the proper field
            {
                case 0:                                         // hours
                    if (setup_alarm_hour == 0) setup_alarm_hour = 23;   // go back to 23 if under 0
                    else --setup_alarm_hour;                    // decrease hour by 1
                    break;                                      //

                 case 1:                                        // minute
                    if (setup_alarm_minute == 0) setup_alarm_minute = 59;   // if minute is 0 then go to 59
                    else --setup_alarm_minute;                  //
                    break;

                case 2:                                         // second
                    if (setup_alarm_second == 0) setup_alarm_second = 59;   // if alarm second is 0 go to 59
                    else --setup_alarm_second;                  //
                    break;                                      //
                    
                default:                                        //
                break;                                          //
            }                
            Update_Setup_Alarm_Time_Screen();                   // Update alarm screen with latest info
                         
} 

void Go_Next_Alarm_Field()                                      // Go to next field function
{
     
    Select_Alarm_Field++;                                       // Move to next field
    if (Select_Alarm_Field == 3) Select_Alarm_Field = 0;        // Wrap around if necessary
    Update_Setup_Screen_Cursor_Forward(Select_Alarm_Field);     // Update cursor
                                     
}  

void Go_Prev_Alarm_Field()                                      // Go to previous function
{
    if (Select_Alarm_Field == 0) Select_Alarm_Field = 2;
    else Select_Alarm_Field--;                                  // Move to next field
    Update_Setup_Screen_Cursor_Backward(Select_Alarm_Field);    // Update cursor
} 
          
void Exit_Setup_Alarm_Time()                                    // Exit alarm setup function
{
    DS3231_Read_Time();                                         // Read time
    Initialize_Screen();                                        // Initialize the screen before returning  
    In_Alarm_Time_Setup = 0;                                    // Set alarm setup time to 0
}

void Do_Save_New_Alarm_Time()                                   // Do save the new alarm time function
{
    DS3231_Write_Alarm_Time();                                  // Write alarm time
    DS3231_Read_Alarm_Time();                                   // Read alarm time
    DS3231_Read_Time();                                         // Read current time
    Initialize_Screen();                                        // Initialize main screen before returning
    In_Alarm_Time_Setup = 0;                                    // Set the alarm time setup to 0
}
     
void Initialize_Setup_Alarm_Time_Screen(void)                   // initialize the setup alarm time screen function
{ 
    fillScreen(ST7735_BLACK);                                   // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L F23 Final");                          // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);
                                                                // X and Y coordinates of where the text is to be displayed

    strcpy(txt, "Alarm Setup");                                 // Text displayed 
    drawtext(start_x+5 , start_y+20, txt, ST7735_MAGENTA, ST7735_BLACK, TS_2); 
    
    strcpy(txt, "Time");                                        // Text displayed
    drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);
    fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);  //
}

void Update_Setup_Alarm_Time_Screen(void)                       // Update the setup alarm time screen function
{
    printf ("%x:%x:%x\r\n", setup_alarm_hour,setup_alarm_minute,setup_alarm_second);
    setup_alarm_time[0]  = (setup_alarm_hour/10)  + '0';        // ones
    setup_alarm_time[1]  = (setup_alarm_hour%10)  + '0';        // tens
    setup_alarm_time[3]  = (setup_alarm_minute/10)  + '0';      // ones
    setup_alarm_time[4]  = (setup_alarm_minute%10)  + '0';      // tens
    setup_alarm_time[6]  = (setup_alarm_second/10)  + '0';      //ones
    setup_alarm_time[7]  = (setup_alarm_second%10)  + '0';      // tens			
    drawtext(data_time_x, data_time_y, setup_alarm_time, ST7735_CYAN, ST7735_BLACK, TS_2);
}