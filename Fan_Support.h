int get_RPM();                                   // Declarations
void Toggle_Fan();                               //
void Turn_Off_Fan();                             //
void Turn_On_Fan();                              //
void Increase_Speed();                           //
void Decrease_Speed();                           //
void Set_DC_RGB(int);                            //
void Set_RPM_RGB(int);                           //


#define FAN_EN		PORTDbits.RD3                // Defining FAN Enbale to PORT D bit 3
#define FAN_EN_LED	PORTDbits.RD7                // Defining FAN LED to PORT D bit 7
#define FAN_PWM		PORTCbits.RC2                // Defining FAN PWN to PORTC bit 2 PORTCbits.RC2

#define D1_RED      PORTDbits.RD0                // Defining D1 Red bit
#define D1_GREEN    PORTDbits.RD1                // Defining D1 Green bit
#define D1_BLUE     PORTDbits.RD2                // Defining D1 Blue bit

#define D2_RED      PORTEbits.RE0                // Defining D1 Red bit
#define D2_GREEN    PORTEbits.RE1                // Defining D1 Green bit
#define D2_BLUE     PORTEbits.RE2                // Defining D1 Blue bit