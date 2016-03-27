/*	(Source: Nintendo Gamecube Controller Protocol
		updated 8th March 2004, by James.)

	Bit		Function
	0-2		Always 0 
	3		Start
	4		Y
	5		X
	6		B
	7		A
	8		Always 1
	9		L
	10		R
	11		Z
	12-15	Up,Down,Right,Left
	16-23	Joy X
	24-31	Joy Y
	32-39	C Joystick X
	40-47	C Joystick Y
	48-55	Left Btn Val
	56-63	Right Btn Val
 */

// Defines
#define LEFT    ((~buttons2 & 0x80) & ((~buttons2 & 0x40) << 1)) | (buttons1 & 0x41) 
#define RIGHT   ((buttons2 & 0x80) & ((buttons2 & 0x40) << 1)) | (buttons1 & 0x22) 
#define AWAY    ((buttons3 & 0x80) & ((buttons3 & 0x40) << 1)) | (buttons1 & 0x08) 
#define TOWARDS ((~buttons3 & 0x80) & ((~buttons3 & 0x40) << 1)) | (buttons1 & 0x04) 
#define DOWN    (buttons0 & 0x06)
#define UP      (buttons0 & 0x09)
#define START   (buttons0 & 0x10)
#define bitRev(c)	c = (c & 0x30) << 4 | (c & 0xF0) >> 4; \
                    c = (c & 0x33) << 2 | (c & 0xCC) >> 2; \
                    c = (c & 0x55) << 1 | (c & 0xAA) >> 1;

// Global Variables
unsigned char buttons0,buttons1,buttons2,buttons3;
unsigned char buttons4,buttons5,buttons6,buttons7;

// Prototypes
void controllerUpdate(void);

// Functions
void controllerUpdate(void) 
{
_asm 
//1 uS = 10 cycles 
//3 uS = 30 cycles 
//9 NOPs + instruction to set next bit should be 10 cycles 
//We want to send 0100 0000 0000 0011 0000 0011 <- The last bit is high because I want some rumble action! 
//But don't forget to TERMINATE by sending a 1 at the end of the 24 bit stream. 

top:
//Initializations for the whole rest of the Assembly
    // This is WaitForResponse loop initialization
    MOVLW 0xFF              // 1
    MOVWF 0x0A, 0           // 1 
    
    // This the initialization for status registers
    MOVLW 0x00              // 1
    MOVWF 0x00, 0           // 1 
    MOVWF 0x01, 0           // 1 
    MOVWF 0x02, 0           // 1 
    MOVWF 0x03, 0           // 1 
    MOVWF 0x04, 0           // 1 
    MOVWF 0x05, 0           // 1 
    MOVWF 0x06, 0           // 1 
    MOVWF 0x07, 0           // 1 
    

//First 0100 
    CALL sendZero, 0 
    CALL sendOne, 0 
    CALL sendZero, 0 
    CALL sendZero, 0 

// Then 0000 
    CALL sendZero, 0 
    CALL sendZero, 0 
    CALL sendZero, 0 
    CALL sendZero, 0 

// And 0000 
    CALL sendZero, 0 
    CALL sendZero, 0 
    CALL sendZero, 0 
    CALL sendZero, 0 

// Followed by 0011 
    CALL sendZero, 0 
    CALL sendZero, 0 
    CALL sendOne, 0 
    CALL sendOne, 0 

// Proceded with 0000 
    CALL sendZero, 0 
    CALL sendZero, 0 
    CALL sendZero, 0 
    CALL sendZero, 0 

// Finally 0011 
    CALL sendZero, 0 
    CALL sendZero, 0 
    CALL sendOne, 0 
    CALL sendOne, 0 
    
// Terminated with 1, After termination A0 will be an input, and should be high. 
    CALL sendOne, 0 




   
//============================================================================ 
// Check to see if anything drives A0 low.     
// This is just to see if we get any thing back 
    //MOVLW 0xFF          // 1  - 
    //MOVWF 0x0A, 0
   
    loopA:  
        BTFSS PORTA, 0, 0   // 1 (2 or 3) - Skip next instrucion if A0 is high 
        GOTO parseResponse  // 2 
        DECFSZ 0x0A, 1, 0   // 1 (2 or 3) - Decrement 0x0A and skip next instruction if 0
    BRA loopA               // 2

    GOTO top            // 2 - This is if it times out!

    
    
    
    
    
    
    
//============================================================================     
// Now we want to parse and save the 64 + 1 bit response 
// from the Game Cube Controller. 
//
// Best case: 0.4 uS after the first 1 uS LOW
// Worst case: 0.9 uS after the first 1 uS LOW
// Average: 0.7 uS
//
// Cycle: 1-10 10-20 20-30 30-40
// Cycle: 0-1   1-2   2-3   3-4
//  HIGH:  0     1     1     1
//	 LOW:  0     0     0     1
//
// So the strategy is to check PORTA0 around 2.0 uS into each bit. 
// Lets assume we started from 0.7 uS in. We will need to delay 1.3 uS first

parseResponse:
    CALL delay9, 0         // 11 - Delay until we reach the 2.0 uS mark

    CALL get8Bits, 0
    MOVFF 0x00, buttons0    // 1 This wont work :( BUT IT DID!
    CLRF 0x00, 0            // 1  - Clear it for the next time    
    CALL get8Bits, 0
    MOVFF 0x00, buttons1    // 1 This wont work :( BUT IT DID!
    CLRF 0x00, 0            // 1  - Clear it for the next time    
    CALL get8Bits, 0
    MOVFF 0x00, buttons2    // 1 This wont work :( BUT IT DID!
    CLRF 0x00, 0            // 1  - Clear it for the next time
    CALL get8Bits, 0
    MOVFF 0x00, buttons3    // 1 This wont work :( BUT IT DID!
    CLRF 0x00, 0            // 1  - Clear it for the next time
    CALL get8Bits, 0
    MOVFF 0x00, buttons4    // 1 This wont work :( BUT IT DID!
    CLRF 0x00, 0            // 1  - Clear it for the next time
    CALL get8Bits, 0
    MOVFF 0x00, buttons5    // 1 This wont work :( BUT IT DID!
    CLRF 0x00, 0            // 1  - Clear it for the next time    
    CALL get8Bits, 0
    MOVFF 0x00, buttons6    // 1 This wont work :( BUT IT DID!
    CLRF 0x00, 0            // 1  - Clear it for the next time    
    CALL get8Bits, 0
    MOVFF 0x00, buttons7    // 1 This wont work :( BUT IT DID!
    CLRF 0x00, 0            // 1  - Clear it for the next time

 
    GOTO fin                // 2  - Done!!









//============================================================================ 
//===========================Function Declarations============================ 
//============================================================================ 

unhandled:              //LED 2
    MOVLW 0x04 
    MOVWF LATD, 0
BRA unhandled

sendZero: 
    BCF TRISA, 0, 0     // 1  - Set A0 as output (low) for 3 uSeconds 
    CALL delay29, 0     // 29 = 2(call) + 25(NOP) + 2(return) 
    BSF TRISA, 0, 0     // 1  - Set A0 as input (high) for 1 uSecond 
    CALL delay5, 0      // 5  = 2(call) + 1(NOP) + 2(return) 
RETURN 0           // 2   - This is always followed by a CALL sendOne or CALL sendZero 
// A0 has been high for 8 cycles already. The next instruction will be a  
// CALL sendOne or CALL sendZero which will last 2 clock cycles then A0 
// will be set low on the following clock cycle. So 2 more clock cycles remain.

sendOne: 
    BCF TRISA, 0, 0     // 1  - Set A0 as output (low) for 1 uSeconds 
    CALL delay9, 0      // 9  = 2(call) + 5(NOP) + 2(return) 
    BSF TRISA, 0, 0     // 1  - Set A0 as input (high) for 3 uSecond 
    CALL delay25, 0     // 25 = 2(call) + 21(NOP) + 2(return) 
RETURN 0           // 2   - This is always followed by a CALL sendOne or CALL sendZero 
// A0 has been high for 28 cycles already. The next instruction will be a  
// CALL sendOne or CALL sendZero which will last 2 clock cycles then A0 
// will be set low on the following clock cycle. So 2 more clock cycles remain.


get8Bits:
    MOVLW 0x07          // 1  - MAYBE CHANGE THIS TO 0x07 But I doub't it
    MOVWF 0x0A, 0       // 1 
    shifter:
        RLNCF 0x00, 1, 0    // 1  - Shift the bit left
        BTFSC PORTA, 0, 0   // 1 (2 or 3) - Skip next instrucion if PORTA0 is LOW 
        INCF 0x00, 1, 0     // 1  - Set reg HIGH if port A0 is HIGH
        CALL delay34, 0     // 34 - At this point we're at 2.3 uS. We need to delay:
                            //      4.0(full cycle) - 0.3(prev instrucs) - 0.3(dec & bra)
                            //      = 3.4 uS delay
        DECFSZ 0x0A, 1, 0   // 1 (2 or 3) - Skip next instrucion if 0x0A is 0
    BRA shifter             // 2  - Do it again!
    
    NOP                     // 1  - To make up for half of the branch we skipped
    RLNCF 0x00, 1, 0        // 1  - Shift the bit left
    BTFSC PORTA, 0, 0       // 1 (2 or 3) - Skip next instrucion if PORTA0 is LOW 
    INCF 0x00, 1, 0         // 1  - Set reg HIGH if port A0 is HIGH
    
    CALL delay30, 0         // 31 - See next comment block for explaination
RETURN 0                // 2
// We also have to take into account that after this RETURN (0.2uS)(which I remembered this time),
// there will be a MOVFF (0.1 uS) and a CALL (0.2 uS). We last checked PortA0 at 2.0 uS. So we need 
// to check again in 4.0 uS. 4.0 - 0.1(INC) - 0.5(call/ret/movff) - 0.3(movlw,movwf,rlncf) = 3.1 uS.



// My Super Effective Delay Function    
// 3.4 uS = 2(CALL) + 30(NOP) + 2(RETURN)
delay34:NOP                 // 1   // 30 
        NOP                 // 1 
        NOP                 // 1 
delay31:NOP                 // 1 
delay30:NOP                 // 1
        
// 29 = 2(CALL) + 25(NOP) + 2(RETURN)
delay29:NOP                 // 1   - 25 
        NOP                 // 1 
        NOP                 // 1 
        NOP                 // 1 
delay25:NOP                 // 1
    
        NOP                 // 1   - 20 
        NOP                 // 1 
        NOP                 // 1 
        NOP                 // 1 
        NOP                 // 1 
    
        NOP                 // 1   - 15 
        NOP                 // 1 
        NOP                 // 1 
        NOP                 // 1 
        NOP                 // 1 
    
        NOP                 // 14  - 10 
delay13:NOP                 // 13 
        NOP                 // 12 
delay11:NOP                 // 11 = 2(CALL) + 7(NOP) + 2(RETURN)
        NOP                 // 10
        
delay9: NOP                 // 9   - 5
        NOP                 // 8 
        NOP                 // 7 
        NOP                 // 6
delay5: NOP                 // 5

RETURN 0            // 2 


// Errors
timeOut:                // LED 3
    MOVLW 0x08          // 1
    MOVWF LATD, 0       // 1 
    BRA timeOut
    
fin: 
_endasm 
} 

/*
	(Source: Nintendo Gamecube Controller Protocol
		updated 8th March 2004, by James.)

	Bit		Function
	0-2		Always 0 
	3		Start
	4		Y
	5		X
	6		B
	7		A
	8		Always 1
	9		L
	10		R
	11		Z
	12-15	Up,Down,Right,Left
	16-23	Joy X
	24-31	Joy Y
	32-39	C Joystick X
	40-47	C Joystick Y
	48-55	Left Btn Val
	56-63	Right Btn Val
 */

