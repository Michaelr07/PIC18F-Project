#include <p18f4620.h>                
#include "I2C.h"                       // Include the I2C header file

#define ACK     1                      // Define ACK as 1
#define NAK     0                      // Define NAK as 0

#define _XTAL_FREQ      8000000        // Define the crystal frequency

void I2C_Init(unsigned long c)         // Initialize I2C with a given clock frequency
{
   i2cHighScl();                       // Set SCL to high
   i2cHighSda();                       // Set SDA to high
}

unsigned char I2C_Read(char ack)       // Read a byte from I2C with an acknowledgment parameter
{
   unsigned char inByte, n;            // Declare variables for incoming byte and counter
   i2cHighSda();                       // Set SDA to high
   for (n = 0; n < 8; n++)             // for loop from 0 to 8
   {
      i2cHighScl();                    // Set SCL to high

      if (SDA_PIN)                     // if SDA pin is 1
         inByte = (inByte << 1) | 0x01;// make the inbyte shifted to the left and mask 1st bit
      else
         inByte = inByte << 1;         // else just shift the inByte to the left
      i2cLowScl();                     // Set SCL to low
   }
   if (ack) i2cAck();                  // If acknowledgment is needed, send ACK
   else i2cNack();                     // Otherwise, send NAK

   return(inByte);                     // Return the received byte
} 

void I2C_Write(unsigned char outByte)  // Write a byte to I2C
{
   unsigned char n;                    // Declare counter variable
   for(n = 0; n < 8; n++)              // loop from 0 to 8
   {
      if(outByte & 0x80) i2cHighSda(); // If the most significant bit is 1, set SDA to high
      else i2cLowSda();                // Otherwise, set SDA to low
      i2cHighScl();                    // Set SCL to high
      i2cLowScl();                     // Set SCL to low
      outByte = outByte << 1;          // Shift outByte to the left
   }
   i2cHighSda();                       // Set SDA to high
   i2cHighScl();                       // Set SCL to high
   i2cLowScl();                        // Set SCL to low
}

void i2cNack(void)                     // Send NAK signal in I2C function
{
   i2cHighScl();                       // Set SCL to high
   i2cLowScl();                        // Set SCL to low (bring data high and clock)
}

void i2cAck(void)                      // Send ACK signal in I2C function
{
   i2cLowSda();                        // Set SDA to low
   i2cHighScl();                       // Set SCL to high
   i2cLowScl();                        // Set SCL to low (bring data low and clock)
   i2cHighSda();                       // Set SDA to high
}

void I2C_Start(void)                   // Generate I2C start condition function
{
   i2cHighSda();                       // Set SDA to high
   i2cHighScl();                       // Set SCL to high
   i2cLowSda();                        // Set SDA to low while SCL is high
   i2cLowScl();                        // Set SCL to low
}

void I2C_ReStart(void)                 // Generate I2C restart condition function
{
   i2cLowScl();                        // Set SCL to low
   i2cHighSda();                       // Set SDA to high
   i2cHighScl();                       // Set SCL to high
   i2cLowSda();                        // Set SDA to low while SCL is high
   i2cLowScl();                        // Set SCL to low
}

void I2C_Stop(void)                    // Generate I2C stop condition function
{
   i2cLowScl();                        // Set SCL to low
   i2cLowSda();                        // Set SDA to low
   i2cHighScl();                       // Set SCL to high
   i2cHighSda();                       // Set SDA to high while SCL is high
}

void i2cHighSda(void)                  // Set SDA to high impedance function
{
   SDA_DIR = 1;                        // Bring SDA to high impedance
   __delay_us(2);                      // Delay for stability
}

void i2cLowSda(void)                   // Set SDA to low function
{
   SDA_PIN = 0;                        // Output a logic zero
   SDA_DIR = 0;                        // Set to output mode
   __delay_us(2);                      // Delay for stability
}

void i2cHighScl(void)                  // Set SCL to high impedance function
{
   SCL_DIR = 1;                        // Bring SCL to high impedance
   __delay_us(10);                     // Delay for stability
}

void i2cLowScl(void)                   // Set SCL to low function
{
   SCL_PIN = 0;                        // Output a logic zero
   SCL_DIR = 0;                        // Set to output mode
   __delay_us(1);                      // Delay for stability
}

char I2C_Write_Address_Read_One_Byte(char Device, char Address) // Write address and read one byte function
{
    char Data_Ret;                     // create char Data Ret
    I2C_Start();                       // Start I2C protocol
    I2C_Write((Device << 1) | 0);      // DS3231 address Write mode
    I2C_Write(Address);                // Send register address
    I2C_ReStart();                     // Restart I2C
    I2C_Write((Device << 1) | 1);      // Initialize data read
    Data_Ret = I2C_Read(NAK);          // Read data with NAK
    I2C_Stop();                        // Stop I2C
    return Data_Ret;                   // return Data Ret
}

void I2C_Write_Address_Write_One_Byte(char Device, char Address, char Data_Out) // Write address and write one byte function
{
    I2C_Start();                       // Start I2C protocol
    I2C_Write((Device << 1) | 0);      // Device address Write mode
    I2C_Write(Address);                // Send register address
    I2C_Write(Data_Out);               // Write data
    I2C_Stop();                        // Stop I2C
}

void I2C_Write_Cmd_Only(char Device, char Cmd) // Write command only
{
    I2C_Start();                       // Start I2C protocol
    I2C_Write((Device << 1) | 0);      // Device address Write mode
    I2C_Write(Cmd);                    // Send Command
    I2C_Stop();                        // Stop I2C
}

void I2C_Write_Cmd_Write_Data(char Device, char Cmd, char Data_Out) // Write command and write data function
{
    I2C_Start();                       // Start I2C protocol
    I2C_Write((Device << 1) | 0);      // Device address Write mode
    I2C_Write(Cmd);                    // Send Command
    I2C_Write(Data_Out);               // Send Data
    I2C_Stop();                        // Stop I2C
}

char I2C_Write_Cmd_Read_One_Byte(char Device, char Cmd) // Write command and read one byte function
{
    char Data_Ret;                     // Data Ret character
    I2C_Start();                       // Start I2C protocol
    I2C_Write((Device << 1) | 0);      // Device address
    I2C_Write(Cmd);                    // Send register address
    I2C_ReStart();                     // Restart I2C
    I2C_Write((Device << 1) | 1);      // Initialize data read
    Data_Ret = I2C_Read(NAK);          // Read data with NAK
    I2C_Stop();                        // Stop I2C
    return Data_Ret;                   // Return data Ret 
}
