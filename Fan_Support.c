#include <p18f4620.h>               
#include "Main.h"                   
#include "Fan_Support.h"            
#include "stdio.h"                  
#include "utils.h"                                  // Include the Utilities header file

extern char FAN;                                    // Declare FAN variable as external 
extern char duty_cycle;                             // Declare duty_cycle variable as external 
extern char fan_set_temp;                           // Declare fan set temperatire as external
extern signed int DS1621_tempF;                     // Declare the temperature in F as external

int get_RPM()                                       // Get fan RPM value using hardware timer
{
    T3CONbits.TMR3ON = 1;                           // Turn on timer 3
    int RPS = TMR3L / 2;                            // RPS = count /2
    TMR3L = 0;                                      // clear out the count
    return (RPS * 60);                              // return RPM = 60 * RPS
}

int get_duty_cycle (signed int temp, int set_temp)  // Get the duty cycle function
{
    float diff_temp = set_temp - temp;              // float for difference temperature
    float dc_float;                                 // float for duty cyce

    if (temp >= set_temp) duty_cycle = 0;           // if the temperature is higher or equal to the set temperature DC will be 0
    else {
        if (diff_temp > 50) dc_float = 100.0;       // if difference temp' is higher than 50, DC is 100
        else if (35 <= diff_temp && diff_temp < 50){// else if difference temperature is between 35 and 50 
            dc_float = 2.0 * diff_temp;}            // set the DC to difference temperature * 2
        else if (25 <= diff_temp && diff_temp < 35){// else if difference temperature is between 25 and 35
            dc_float = 1.5 * diff_temp;}            // set DC to difference temp' * 1.5
        else if (10 <= diff_temp && diff_temp < 25){// else if diff' temp' is between 10 and 25
            dc_float = 1.25 * diff_temp;}           // set DC to diff' temp' *1.25
        else if (0 <= diff_temp && diff_temp < 10){ // else if diff' temp' is between 0 and 10
            dc_float = 1.00 * diff_temp;}           // set DC to difference temp' *1
        
        duty_cycle = (int)dc_float;                 // forced the float DC into integer
    }
        
    return duty_cycle;                              // returned the integer Duty Cycle
}
    
void Monitor_Fan()                                  // Function for monitoring the fan
{
    duty_cycle = get_duty_cycle(DS1621_tempF, fan_set_temp); // apply the duty cycle to 
    do_update_pwm(duty_cycle);                      // update the pwm depending on new DC
    if (FAN == 1) Turn_On_Fan();                    // If FAN is 1 then Turn on the fan
    else Turn_Off_Fan();                            // Else turn off the fan
}

void Toggle_Fan()                                   // Toggle Fan function
{
    if (FAN == 0) Turn_On_Fan();                    // inverse fan variable if currently 0
    else if (FAN == 1) Turn_Off_Fan();              // inverse fan variable if currently 1
}

void Turn_Off_Fan()                                 // Turn off fan function
{
    FAN = 0;                                        // Fan variable = 0
    FAN_EN = 0;                                     // Fan_EN variable = 0
    FAN_EN_LED = 0;                                 // turn off FAN_EN LED
}

void Turn_On_Fan()                                  // Turn on Fan function
{
    FAN = 1;                                        // Fan variable = 1
    do_update_pwm(duty_cycle);                      // update pwm to duty cycle
    FAN_EN = 1;                                     // FAN_EN variable = 1
    FAN_EN_LED = 1;                                 // turn on FAN_EN LED
}

const int rpm_thresholds[] = {0, 500, 1000, 1500, 2000, 2500, 3000};  // Array of RPM thresholds

void Set_RPM_RGB(int rpm)                           // Function to set the color based on RPM
{
    char color = 0;                                 // Default color 0

    for (int i = 0; i < 7; i++)                     // loop for 7 
    {
        if (rpm == 0)                               // if rpm is 0
        {
            Set_D2_Color(0);                        // set color to 0
        }
        else if (rpm >= rpm_thresholds[i])          // else if rpm is greater than threshold
        {
            color = i + 1;                          // Add 1 to offset the color values
        } 
        else                                        // else
        {
            break;                                  // Exit the loop as soon as we find the correct color
        }
    }
    Set_D2_Color(color);                            // Set color for D2 LED
}