#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ST7735_TFT.h"                                         // Include header files
#include "I2C_Support.h"                                        //
#include "Setup_Time.h"                                         //
#include "Main_Screen.h"                                        //
#include "Main.h"                                               //
#include "utils.h"                                              //

extern unsigned char second, minute, hour, dow, day, month, year;// delcaring extern varaibles
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
extern char setup_time[];                                       //
extern char setup_date[];                                       //
extern char buffer[22];                                         //
extern char *nbr;                                               //
extern char *txt;                                               //
extern char tempC[];                                            //
extern char tempF[];                                            //
extern char time[];                                             //
extern char date[];                                             //
extern char found;                                              //
extern short nec_ok;                                            //
char In_Time_Setup;                                             // declaring characters
char Select_Field;                                              //
    
void Do_Setup_Time(void)                                        // Setup time function
{

    Select_Field = 0;                                           // start at field 0

    DS3231_Read_Time();                                         // Read actual time
    setup_second = bcd_2_dec(second);                           // Set setup time as current time
    setup_minute = bcd_2_dec(minute);                           // convert all the bcd to real integer
    setup_hour = bcd_2_dec(hour);                               //
    setup_day = bcd_2_dec(day);                                 // 
    setup_month = bcd_2_dec(month);                             // 
    setup_year = bcd_2_dec(year);                               // 
    
    Initialize_Setup_Time_Screen();                             // Initialize setup screen
    Update_Setup_Time_Screen();                                 // Update screen with latest information
    In_Time_Setup = 1;                                          // set variable to stay in setup mode
    
    while (In_Time_Setup == 1)                                  // While in time setup is true
    {   
         if (check_for_button_input() == 1)                     // check if button pressed
         {

             if ((found == Prev) || (found == Next) || (found == Ch_Minus) || (found == Ch_Plus) || (found == Minus) || (found == Plus)) // check for those buttons
             {
                Do_Beep_Good();                                 // do a beep good
                if (found == Prev) Go_Prev_Field();             // if prev found go previous
                if (found == Next) Go_Next_Field();             // check if 'Next' then use Go_Next_Field(); 
                if (found == Ch_Plus) Do_Save_New_Time();       // check if 'Ch_Plus' then use Do_Save_New_Time();
                if (found == Ch_Minus) Exit_Time_Setup();       // check if 'Ch_Minus' Exit_Time_Setup();                      
                if (found == Minus) Decrease_Time();            // check if 'Minus' then use Decrease_Time();
                if (found == Plus) Increase_Time();             // check if 'Plus' then use Increase_Time();
                found = 0xff;                                   // then found set to 0xff
             }
             else
             {
                 Do_Beep_Bad();                                 // else beep bad
             }
        }                   
    }
}

void Increase_Time()                                            // Increase time function
{
            switch (Select_Field)                               // Increment the proper field
            {
                case 0:                                         // hour
                    setup_hour++;                               //
                    if (setup_hour >= 24) setup_hour = 0;       //
                    break;

                case 1:                                         // minute
                    setup_minute++;                             //
                    if (setup_minute >= 60) setup_minute = 0;   //
                    break;                                      //
                    
                case 2:                                         // second
                    setup_second++;                             //
                    if (setup_second >= 60) setup_second = 0;   //
                    break;                                      //
                    
                case 3:                                         // month
                    setup_month++;                              //
                    if (setup_month >= 13) setup_month = 1;     // 
                    break;                                      //
                    
                case 4:                                         // day
                    setup_day++;                                //
                    if (setup_day >= 31) setup_day = 1;         //
                    break;                                      //
                    
                case 5:                                         // year
                    setup_year++;                               //
                    if (setup_year >= 100) setup_year = 0 ;     //
                    break;                                      //
                    
                default:                                        //
                    break;                                      //
            }    
            Update_Setup_Time_Screen();                         // Update screen with latest info
}

void Decrease_Time()                                            // Decrease time function
{
            switch (Select_Field)                               // Decrement the proper field
            {
                case 0:                                         // hours
                    if (setup_hour == 0) setup_hour = 23;       //
                    else --setup_hour;                          //
                    break;
                case 1:                                         // minutes
                    if (setup_minute == 0) setup_minute = 59;   //
                    else --setup_minute;                        //
                    break;
                case 2:                                         // seconds
                    if (setup_second == 0) setup_second = 59;   //
                    else --setup_second;                        //
                    break;
                case 3:                                         // months
                    if (setup_month == 1) setup_month = 12;     //
                    else --setup_month;                         //
                    break;
                case 4:                                         // days
                    if (setup_day == 1) setup_day = 31;         //
                    else --setup_day;                           //
                    break;
                case 5:                                         // years
                    if (setup_year == 0) setup_year = 99;       //
                    else --setup_year;                          //
                    break;
                default:                                        // default 
                    break;
            }                
            Update_Setup_Time_Screen();                         // Update screen with latest info
        } 

void Go_Next_Field()                                            // go to next field function
{
        
    Select_Field++;                                             // Move to next field
    if (Select_Field == 6) Select_Field = 0;                    // Wrap around if necessary
    Update_Setup_Screen_Cursor_Forward(Select_Field);           // Update cursor

}  

void Go_Prev_Field()                                            // go to previous field function
{
    if (Select_Field == 0) Select_Field = 5;                    // if field is 0 to to last field
    else Select_Field--;                                        // Move to next field
    Update_Setup_Screen_Cursor_Backward(Select_Field);          // Update cursor
} 

void Do_Save_New_Time()                                         // Do save new time function
{   
    DS3231_Write_Time();                                        // Write time
    DS3231_Read_Time();                                         // Read time
    Initialize_Screen();                                        // Initialize the screen before returning  
    In_Time_Setup = 0;                                          // IN time setup will be 0
}
    
void Exit_Time_Setup()                                          // Exit the time setup function
{   
    DS3231_Read_Time();                                         // Read time
    Initialize_Screen();                                        // Initialize the screen before returning  
    In_Time_Setup = 0;                                          // Set in mode to 0
}   
void Initialize_Setup_Time_Screen(void)                         // initlaize setup time screen function
{   
    fillScreen(ST7735_BLACK);                                   // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L F23 Final\0");                        // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);
                                                                // X and Y coordinates of where the text is to be displayed

    strcpy(txt, "Time Setup\0");                                // Text displayed 
    drawtext(start_x+3 , start_y+15, txt, ST7735_MAGENTA, ST7735_BLACK, TS_2); 
       
    strcpy(txt, "Time");                                        // Text displayed
    drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);
    
    fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
    strcpy(txt, "Date");                                        // Text Displayed
    drawtext(date_x  , date_y , txt, ST7735_RED    , ST7735_BLACK, TS_1);
}

void Update_Setup_Time_Screen(void)                             // Update the Time screen function
{
    setup_time[0]  = (setup_hour/10)  + '0';                    // msd
    setup_time[1]  = (setup_hour%10)  + '0';                    // lsd
    
    setup_time[3]  = (setup_minute/10)  + '0';                  // msd
    setup_time[4]  = (setup_minute%10)  + '0';                  // lsd
    
    setup_time[6]  = (setup_second/10)  + '0';                  // msd
    setup_time[7]  = (setup_second%10)  + '0';                  // lsd
    
    setup_date[0]  = (setup_month/10)  + '0';                   // msd
    setup_date[1]  = (setup_month%10)  + '0';                   // lsd
    
    setup_date[3]  = (setup_day/10)  + '0';                     // msd
    setup_date[4]  = (setup_day%10)  + '0';                     // lsd
        
    setup_date[6]  = (setup_year/10)  + '0';                    // msd
    setup_date[7]  = (setup_year%10)  + '0';                    // lsd

    drawtext(data_time_x, data_time_y, setup_time, ST7735_CYAN, ST7735_BLACK, TS_2);
    drawtext(data_date_x, data_date_y, setup_date, ST7735_GREEN, ST7735_BLACK, TS_2);
}

void Update_Setup_Screen_Cursor_Forward(char field)             // Update screen cursor foward function
{
    char xinc = 36;
    char yinc = 30;   
    switch (field)
    {
        case 0:
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
            break;
            
        case 1:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break; 
            
        case 2:
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break;  
            
        case 3:
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;   
            
       case 4:
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;  

       case 5:
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;                           
    }
}

void Update_Setup_Screen_Cursor_Backward(char field)
{
    char xinc = 36;
    char yinc = 30;   
    switch (field)
    {
        case 0:
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_CYAN);
            break;
            
        case 1:
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break; 
            
        case 2:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+2*xinc, data_time_y+16, 25,2,ST7735_CYAN);
            break;  
            
        case 3:
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;   
            
       case 4:
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_BLACK);
            fillRect(data_time_x-1+xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;  

       case 5:
            fillRect(data_time_x-1, data_time_y+16, 25,2,ST7735_BLACK);           
            fillRect(data_time_x-1+2*xinc, data_time_y+16+yinc, 25,2,ST7735_CYAN);
            break;              
    }
}




