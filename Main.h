#define _XTAL_FREQ      8000000         // 8 Mhz frequency 

#define ACK             1               // defined ACK to 1
#define NAK             0               // defined NAK to 0

#define ACCESS_CFG      0xAC            // Access CFG to 0xAC
#define START_CONV      0xEE            // Start Convesion to 0xEE
#define READ_TEMP       0xAA            // Read Temperature 0xAA
#define CONT_CONV       0x02            // Cont Conv to 0x02

#define FANEN_LED       PORTDbits.RD7   // Fan Enable LED to PORTD bit 7
#define FAN_EN          PORTDbits.RD3   // Fan Enable Pin to PORTD bit 3
#define FAN_PWM         PORTCbits.RC2   // Fan PWM Pin to PORTC bit 2
#define RTC_ALARM_NOT   PORTAbits.RA4   // RTC Alarm Notification Pin to PORTA bit 4

#define Ch_Minus                0       // Channel Minus Key
#define Ch_Plus                 2       // Channel Plus Key
#define Setup_Time_Key          12      // Setup Time Key
#define Setup_Alarm_Key         13      // Setup Alarm Key
#define Setup_Fan_Temp_Key      14      // Setup Fan Temperature Key
#define Toggle_Fan_Key          5       // Toggle Fan Key
#define Prev                    3       // Previous Key
#define Next                    4       // Next Key
#define Minus                   6       // Minus Key
#define Plus                    7       // Plus Key