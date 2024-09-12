// Include necessary header files
#include <xc.h>
#include <p18f4620.h>
#include <stdio.h>
#include "Interrupt.h"                  // include interrupt header

unsigned char bit_count;                // Declare global variables
unsigned int Time_Elapsed;              // 

unsigned long long Nec_code;            //
unsigned char Nec_State = 0;            //  

                                        // External variables
extern char Nec_Button;                 //
extern short nec_ok;                    //

extern char INT0_flag;                  //
extern char INT1_flag;                  // Declare external variable INT1_flag
extern char INT2_flag;                  // Declare external variable INT2_flag


void init_INTERRUPT()                   // Function to initialize interrupts
{
                                        // Reset Timer1
    TMR1H = 0;                          //
    TMR1L = 0;                          //
    PIR1bits.TMR1IF = 0;                // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                // Enable Timer 1 interrupt

                                        // Set up external interrupts
    INTCON2bits.INTEDG0 = 0;            // INT0 EDGE falling
    INTCON2bits.INTEDG1 = 0;            // INT1 EDGE falling
    INTCON2bits.INTEDG2 = 0;            // INT2 EDGE falling
    INTCONbits.INT0IF = 0;              // Clear INT0IF 
    INTCON3bits.INT1IF = 0;             // Clear INT1IF 
    INTCON3bits.INT2IF = 0;             // Clear INT2IF 
                                        // Enable external interrupts
    INTCONbits.INT0IE = 1;              // Set INT0 IE 
    INTCON3bits.INT1IE = 1;             // Set INT1 IE 
    INTCON3bits.INT2IE = 1;             // Set INT2 IE
    INTCONbits.PEIE = 1;                // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                 // Enable global interrupts
}

                                        // High-priority interrupt service routine
void interrupt high_priority chkisr()   // interrupt priority function
{
                                        // Check Timer 1 interrupt
    if (PIR1bits.TMR1IF == 1)           //
        TIMER1_isr();                   // call timer 1 isr
    
                                        // Check INT0 interrupt
    if (INTCONbits.INT0IF == 1)         //    
        INT0_isr();                     // call int 0 isr 
        
                                        // Check INT1 interrupt flag
    if (INTCON3bits.INT1IF == 1)        // 
        INT1_isr();                     // call int 1 isr
        
                                        // Check INT2 interrupt
    if (INTCON3bits.INT2IF == 1)        //  
        INT2_isr();                     // call int 2 isr
}

void INT1_isr()                         // INT1 ISR function
{
    INTCON3bits.INT1IF = 0;             // Clear INT1 interrupt flag
    INT1_flag = 1;                      // Set software INT1_flag
}

void INT2_isr()                         // INT2 ISR function
{
    INTCON3bits.INT2IF = 0;             // Clear INT2 interrupt flag
    INT2_flag = 1;                      // Set software INT2_flag
}

void Enable_INT_Interrupt()             // Enable INT function
{
    INTCONbits.INT0IE = 1;              // Enable external interrupt
    INTCON2bits.INTEDG0 = 0;            // Edge programming for INT0 falling edge
}    

void TIMER1_isr(void)                   // Timer 1 ISR function
{
    Nec_State = 0;                      // Reset decoding process
    INTCON2bits.INTEDG0 = 0;            // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;               // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                // Clear interrupt flag
}

void Reset_Nec_State()                  // Reset the Nec State function
{
    Nec_State = 0;                      // Set Nec state to 0
    T1CONbits.TMR1ON = 0;               // Turn off Timer 1
}   

void INT0_isr()                         // INT0 ISR function
{
    INTCONbits.INT0IF = 0;              // Clear external interrupt INT0IF
    if (Nec_State != 0)                 // If Nec state is not 0
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;    // Store Timer1 value
        TMR1H = 0;                              // Reset Timer1
        TMR1L = 0;                              // 
        
        //switch(Nec_State)             // Switch statement for Nec state
        //{
                                        // Cases for different Nec states with associated actions
       // }
    }
    
    INTCON2bits.INTEDG0 = 1;            // Change Edge interrupt of INT0 to Low to High
}
