#include <p18f4520.h>
#include <Delays.h>
#include "gamecube.h"
#include "snake.h" 

unsigned char layerToLight = 0;
unsigned short displayCounter = 0;
unsigned char switchFlag = 0;
unsigned char loseFlag = 0;

void blit(void); 
void init(void); 
void game(void); 
void lose(void);
void start(void); 
 


void main(void)
{


    while(1)
    {
        // Initialize everything
        init();

    
        // Go to the loading screen for the rand() to work
        //start();

        // Enable them interupts
        INTCONbits.GIE =  1; //Enable global interrupts

        // The Game!
        game();

        
        // Loser!!
        //lose();
        
    }

}




#pragma interrupt blit //High priority interrupt service routine
    void blit(void)
    {
        // Interrupt Stuffs
        PIR1bits.TMR1IF = 0; //Reset Timer1 interrupt flag
        TMR1H = 0xF0;
        TMR1L = 0x01;

        if(displayCounter++ >= 0x00FF) 
        {
            displayCounter=0;
            switchFlag=1;
        }

        // Light up the layers
        if(LATC == 0x80) LATC = 0x08;
        LATC = LATC << 1;
        LATD = currentDisplay[layerToLight++];
        LATB = currentDisplay[layerToLight++];
        if(layerToLight == 8) layerToLight = 0;

        // Display Counter

    }
#pragma code




// Main ISR
#pragma code high_vector = 0x000008
    void high_ISR (void)
    {
        _asm goto blit _endasm
    }
#pragma code


void init()
{
    // Initialize Variables
    layerToLight = 0;
    displayCounter = 0;
    switchFlag = 0;
    loseFlag=0;

    // Set all outputs to Digital 
    ADCON1 = 0xFF; 
    CMCON  = 0x07; 

    //Set all to output except A0
    TRISA = 0x01;  
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;

    // Initially set everything to 0
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x08;
    LATD = 0x00;

    // Interupt Stuff
    RCONbits.IPEN = 1;   //Enable priority levels on interrupts
    TMR1H = 0xF0;
    TMR1L = 0x01;
    T1CON = 0b00100001;  //Timer1 settings: 1:8 prescalar, clock = Fosc/4, Timer1 ON
    INTCON = 0b10000000; //
    INTCONbits.GIE =  0; //Enable global interrupts
    INTCONbits.PEIE = 1; //Enable peripheral interrupts
    IPR1bits.TMR1IP = 1; //Timer1 interrupt priority high
    PIE1bits.TMR1IE = 1; //Timer1 interrupt enable

    // Initialize snake then turn on interrupts
    snakeInit();
}


void start(void)
{
    for(i=0; i<8; i++)
    {
        currentDisplay[i]=0x00;
    }
    while(!START)
    {
        //controllerUpdate();
    }
}

void game()
{

    while(!loseFlag)
    {
        addFruit();
        updateDirection();
        if(switchFlag)
        {
            snake();
            nextFrame();
            switchFlag=0;
        }
        if(loseFlag==1) return;        
    }
}

// Your loss.
void lose(void)
{
    /*loseFlag = 0;

    // Initialialize plane
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {            
            for(k=0;k<4;k++)
            {
                plane[i][j][k] = 0;
            }
            for(k=0;k<4;k+=2)
            {
                plane[i][j][k] = 1;
            }
        }
    }
    plane[0][0][0]=1;

    while(UP)
    {
        if(switchFlag) 
        {
            for(i=0;i<4;i++)
            {
                for(j=0;j<4;j++)
                {            
                    for(k=0;k<4;k++)
                    {
                        plane[i][j][k] = ~plane[i][j][k];
                    }
                }
            }
            for(k=0;k<4;k++)
            {
                placeHolder = 0x0001;
                nextDisplay[k] = 0x0000;
                for(j=0;j<4;j++)
                {
                    for(i=0;i<4;i++)
                    {      
                        if(plane[i][j][k] != 0) 
                        {
                            nextDisplay[k]|= placeHolder;
                        }
                        placeHolder = placeHolder << 1;
                    }
                }
            }
            nextFrame();
            switchFlag=0;
        }
        controllerUpdate();
    }*/
    
}