#include <stdio.h>
#include <p18f4620.h>
#include "I2C_Support.h"                                        // include I2C support header
#include "I2C.h"                                                // include I2C header
#include "utils.h"                                              // include utilities header

#define ACCESS_CFG      0xAC                                    // Access CFG defined 0xAC
#define START_CONV      0xEE                                    // Start Conversion defined 0xEE
#define READ_TEMP       0xAA                                    // Read temperature defined 0xAA
#define CONT_CONV       0x02                                    // Read cont conersion defined 0x02
#define ACK     1                                               // defined ACK to 1
#define NAK     0                                               // defined NAK to 0


extern unsigned char second, minute, hour, dow, day, month, year;                                   // declaring variables
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;    //
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;                            //
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;                      //


void DS1621_Init()                                              // Initialize DS1621 function
{
    char Device = 0x48;                                         // Device ID
    I2C_Write_Cmd_Write_Data (Device, ACCESS_CFG, CONT_CONV);   //
    I2C_Write_Cmd_Only(Device, START_CONV);                     // 
}

int DS1621_Read_Temp()                                          // DS1621 read temperature function
{

    char Device = 0x48;                                         // Device ID
    char Cmd = READ_TEMP;                                       // As Given
    char Data_Ret;                                              // create Data Ret
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address
    I2C_Write(Cmd);                                             // Send register address
    I2C_ReStart();                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                               // Initialize data read
    Data_Ret = I2C_Read(NAK);                                   // Set data Ret to I2C read
    I2C_Stop();                                                 // End I2C protocol
    return Data_Ret;                                            // return Data
}

void DS3231_Read_Time()                                         // Read Time DS3231 function
{
    char Device = 0x68;                                         // Device ID
    char Address = 0x00;                                        // Beginning Address 0
 
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address
    I2C_Write(Address);                                         // Send register address
    I2C_ReStart();                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                               // Initialize data read
    second = I2C_Read(ACK);                                     // Read seconds
    minute = I2C_Read(ACK);                                     // Read minutes
    hour = I2C_Read(ACK);                                       // Read hours
    dow = I2C_Read(ACK);                                        // Read day of the week
    day = I2C_Read(ACK);                                        // Read day
    month = I2C_Read(ACK);                                      // Read month
    year = I2C_Read(NAK);                                       // Read year
    I2C_Stop();                                                 // End I2C protocol
  
}

void DS3231_Setup_Time()
{
    char Device = 0x68;                                         // Device ID
    char Address = 0x00;                                        // Beginning Address 0
    second = 0x06;                                              // Set seconds to :06
    minute = 0x28;                                              // Set minutes to :28
    hour = 0x13;                                                // Set hours to 0700
    dow = 0x01;                                                 // Set to... Monday?
    day = 0x20;                                                 // Set to the 24th
    month = 0x04;                                               // Set to July
    year = 0x21;                                                // Set to '52
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address Write mode
    I2C_Write(Address);                                         // Send register address
    I2C_Write(second);                                          // Write seconds
    I2C_Write(minute);                                          // Write minutes
    I2C_Write(hour);                                            // Write hours
    I2C_Write(dow);                                             // Write DOW
    I2C_Write(day);                                             // Write day
    I2C_Write(month);                                           // Write month
    I2C_Write(year);                                            // Write year
    I2C_Stop();                                                 // End I2C protocol
}

void DS3231_Write_Time()
{
    char Device = 0x68;                                         // Device ID
    char Address = 0x00;                                        // Beginning Address 0
    second = dec_2_bcd(setup_second);                           // set setup second to bcd
    minute = dec_2_bcd(setup_minute);                           // set minute to setup bcd minute
    hour = dec_2_bcd(setup_hour);                               // set setup second to bcd
    dow = 0x01;                                                 // set dow to 0x01
    day = dec_2_bcd(setup_day);                                 // set day to setup day bcd
    month = dec_2_bcd(setup_month);                             // set month to bcd setup month
    year = dec_2_bcd(setup_year);                               // set year to setup year bcd
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address Write mode
    I2C_Write(Address);                                         // Send register address
    I2C_Write(second);                                          // Write seconds
    I2C_Write(minute);                                          // Write minutes
    I2C_Write(hour);                                            // Write hours
    I2C_Write(dow);                                             // Write DOW
    I2C_Write(day);                                             // Write day
    I2C_Write(month);                                           // Write month
    I2C_Write(year);                                            // Write year
    I2C_Stop();                                                 // End I2C protocol
}

void DS3231_Write_Initial_Alarm_Time()                          // Write initial alarm time function
{
    DS3231_Read_Time();                                         // Read current time
    alarm_date = day;                                           // Set alarm to today
    char Device = 0x68;                                         // Device ID given
    char Address = 0x07;                                        // Write to register 07
    alarm_hour = dec_2_bcd(0x01);                               // Convert info to BCD
    alarm_minute = dec_2_bcd(0x01);                             // convert alarm minute to bcd
    alarm_second = dec_2_bcd(0x01);                             // convert alarm second to bcd
    alarm_second = alarm_second & 0x7f;                         // Mask off bit 7
    alarm_minute = alarm_minute & 0x7f;                         // Mask off bit 7 
    alarm_hour   = alarm_hour   & 0x7f;                         // Mask off bit 7
    alarm_date   = alarm_date   | 0x80;                         // Set A1M4 as 1
    
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address Write mode
    I2C_Write(Address);                                         // Send register address 7

    I2C_Write(alarm_second);                                    // Write alarm seconds value to DS3231
    I2C_Write(alarm_minute);                                    // Write alarm minute value to DS3231
    I2C_Write(alarm_hour);                                      // Write alarm hour value to DS3231
    I2C_Write(alarm_date);                                      // Write alarm date value to DS3231    
    I2C_Stop();                                                 // End I2C protocol
}    

void DS3231_Read_Alarm_Time()                                   // Read alarm time function
{
char Device = 0x68;                                             // Device ID given
char Address = 0x07;                                            // Read from register 07
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address
    I2C_Write(Address);                                         // Send register address
    I2C_ReStart();                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                               // Initialize data read
    alarm_second = I2C_Read(ACK);                               // Read seconds from register 7
    alarm_minute = I2C_Read(ACK);                               // Read minutes from register 8
    alarm_hour   = I2C_Read(ACK);                               // Read hour from register 9
    alarm_date   = I2C_Read(NAK);                               // Read hour from register A
    I2C_Stop();                                                 // End I2C protocol
}


void DS3231_Init()                                              // Initialize DS3231 function
{   
char Device = 0x68;                                             // create character Device to 0x68
char Address_7 = 0x07;                                          // create character Address 7 to 0x07
char Address_E = 0x0E;                                          //
char control_E;                                                 // create character control E

    control_E = I2C_Write_Address_Read_One_Byte(Device, Address_E); // control E set to write address read one byte
    control_E = control_E & 0x01;                               // Control masked with 0x01
    control_E = control_E | 0x25;                               // Control OR with 0x25
    I2C_Write_Address_Write_One_Byte(Device, Address_E, control_E); // include Control and address in function
    I2C_Start();                                                // Start I2C protocol   
    I2C_Write((Device << 1) | 0);                               // Device address
    I2C_Write(Address_7);                                       // Send register address
    I2C_ReStart();                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                               // Initialize data read
    alarm_second = I2C_Read(ACK);                               // Read seconds from register 7
    alarm_minute = I2C_Read(ACK);                               // Read minutes from register 8
    alarm_hour   = I2C_Read(ACK);                               // Read hour from register 9
    alarm_date   = I2C_Read(NAK);                               // Read hour from register A
      
    alarm_second = alarm_second & 0x7f;                         // Mask off bit 7
    alarm_minute = alarm_minute & 0x7f;                         // Mask off bit 7 
    alarm_hour   = alarm_hour   & 0x7f;                         // Mask off bit 7
    alarm_date   = alarm_date   | 0x80;                         // Mask off bit 7
    
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address Write mode
    I2C_Write(Address_7);                                       // Send register address 7

    I2C_Write(alarm_second);                                    // Reset alarm seconds value to DS3231
    I2C_Write(alarm_minute);                                    // Write alarm minute value to DS3231
    I2C_Write(alarm_hour);                                      // Write alarm hour value to DS3231
    I2C_Write(alarm_date);                                      // Write alarm date value to DS3231    
    I2C_Stop();                                                 // Stop I2C
}

void DS3231_Write_Alarm_Time()                                  // Write Alarm time DS3231 function
{
    DS3231_Read_Time();                                         // Read current time
    alarm_date = day;                                           // Set alarm to today
    char Device = 0x68;                                         // Device ID given
    char Address = 0x07;                                        // Write to register 07
    alarm_hour = dec_2_bcd(setup_alarm_hour);                   // Convert info to BCD
    alarm_minute = dec_2_bcd(setup_alarm_minute);               // Convert minute to BCD
    alarm_second = dec_2_bcd(setup_alarm_second);               // COnvert second to BCD
    alarm_second = alarm_second & 0x7f;                         // Mask off bit 7
    alarm_minute = alarm_minute & 0x7f;                         // Mask off bit 7 
    alarm_hour   = alarm_hour   & 0x7f;                         // Mask off bit 7
    alarm_date   = alarm_date   | 0x80;                         // Set A1M4 as 1
    I2C_Start();                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                               // Device address Write mode
    I2C_Write(Address);                                         // Send register address 7

    I2C_Write(alarm_second);                                    // Write alarm seconds value to DS3231
    I2C_Write(alarm_minute);                                    // Write alarm minute value to DS3231
    I2C_Write(alarm_hour);                                      // Write alarm hour value to DS3231
    I2C_Write(alarm_date);                                      // Write alarm date value to DS3231    
    I2C_Stop();                                                 // End I2C protocol
}    

void DS3231_Turn_Off_Alarm()                                    // Turn off alarm DS3231 function
{
char Device = 0x68;                                             // Set char Device to 0x6
char Address_E = 0x0E;                                          // Set char Address E to 0x0E
char Address_F = 0x0F;                                          // Set char Address F to 0x0F
char control_E;                                                 // Create control E
char control_F;                                                 // Create control F

    control_E = I2C_Write_Address_Read_One_Byte(Device, Address_E); // control set to Write address function
    control_E = control_E & 0xFE;                               // control E is masked
    I2C_Write_Address_Write_One_Byte(Device, Address_E, control_E); // include control
    control_F = I2C_Write_Address_Read_One_Byte(Device, Address_F); // set control F to 
    control_F = control_F & 0xFE;                               // Control F is masked 
    I2C_Write_Address_Write_One_Byte(Device, Address_F, control_F); // include control
    DS3231_Init();                                              // initialize DS3231
}

void DS3231_Turn_On_Alarm()                                     // Turn on Alarm function
{
char Device = 0x68;                                             // Device set to 0x68
char Address_E = 0x0E;                                          // Address E st to 0x0E
char Address_F = 0x0F;                                          // Address F set to 0x0F
char control_E;                                                 // control E char
char control_F;                                                 // control F char

    control_E = I2C_Write_Address_Read_One_Byte(Device, Address_E); // include in function
    control_E = control_E | 0x01;                               // mask control E
    I2C_Write_Address_Write_One_Byte(Device, Address_E, control_E); // include in function
    control_F = I2C_Write_Address_Read_One_Byte(Device, Address_F); // control F 
    control_F = control_F & 0xFE;                               // control F masked
    I2C_Write_Address_Write_One_Byte(Device, Address_F, control_F); // 
    DS3231_Init();                                              // Initialize DS3231
    
}


