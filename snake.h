    #include <stdlib.h>
//#include <stdio.h>
//#include <math.h>

// Prototypes
int snake(void);
void lose(void);
void nextFrame(void);
void snakeInit(void);
void flash(char level);
void error(char level);
void pressed(char level, char b1, char b2);
void updateDirection(void);
void addFruit(void);
void pause(void);




// Global Variables
unsigned char i,j,k, loseFlag;
// Plane[X][Y][Z]:
// Each number represents how many iterations
// that part of the snake will remain on the cube.
// The largest number is the head. 
unsigned char plane[4][4][4];

//THIS SHOULD PROBABLY BE GLOBAL... along with other things
// currentDisplay[4]:
// Holds the array of LEDs that are to display at after
// being optimized to display as fast as possible
unsigned char currentDisplay[8];
unsigned short int nextDisplay[4];
unsigned short int placeHolder;

// Direction:
// 0 = Left     (-X) Joy Left or Left Bumper or Left D-Pad
// 1 = Right    (+X) Joy Right or Right Bumper or Right D-Pad
// 2 = Away     (-Y) Joy Up or Up D-Pad
// 3 = Toward   (+Y) Joy Down or Down D-Pad
// 4 = Down     (-Z) B or X
// 5 = Up       (+Z) A or Y
// 6 = Start    Pause
unsigned char direction;    // Start off to the Right
// These are the defines for the direction buttons. 
// 0100 0001 = 65  -  0100 0000 = 64
// 1011 1111 =-65  -  1100 0000 =-64
unsigned char oldDirection;

// Start:
// Pauses the game until pushed again
//unsigned Char

// Length:
// This will be the number the amount that's added 
// to the next point the head goes to. 
unsigned char length;       // Start with length of 2

// XYZ:
// These are the coordinates of the snake's head
char X;
char Y;
char Z;

char nextX;
char nextY;
char nextZ;

// displayCounter:
// This is like a timer except you know... It's a counter.
    // edit: switchFlag used to be here but I moved it. this keeps getting worse
// Just kidding I meant switch flag...

unsigned char numFruit;
unsigned char maxFruit;


// Functions
void error(char level)
{
    INTCONbits.GIE = 0;
    LATC=level;
    LATD=0xFF;
    LATB=0xFF;
    
    while(1)
    {
        Delay10KTCYx(50);
        LATD=~LATD;
        LATB=~LATB;
    }
}

void flash(char level)
{
    INTCONbits.GIE = 0;
    LATC=level;
    LATD=0xFF;
    LATB=0xFF;
    Delay10KTCYx(200);
    INTCONbits.GIE = 1;
    return;
}

void pressed(char level, char b1, char b2)
{
    LATC=level;
    LATD=b1;
    LATB=b2;
    return;
}

void snakeInit(void)
{
    // Initialialize plane
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            for(k=0;k<4;k++)
            {
                plane[i][j][k] = 0;
            }
        }
    }
    plane[0][0][3] = 1;
    plane[1][0][3] = 2;

    // Initialize currentDisplay
    for(i=0; i<8; i++)
    {
        currentDisplay[i]=0x00;
    }
    currentDisplay[6]=0x03;

    // Initialize "the rest"
    direction = 1; 
    length = 2;
    X = 1;
    Y = 0;
    Z = 3;
    nextX = 1;
    nextY = 0;
    nextZ = 3;
    oldDirection = 1;
    numFruit=0;
    maxFruit=1;
    loseFlag=0;
}

// Let's get this show on the road!
// 0 == Nothing
// 1 == Lose
int snake(void)
{
    // Insert next coordinates into current
    X=nextX;
    Y=nextY;
    Z=nextZ;    
    
    // Move the head forward one, eat a fruit, or die
    if(plane[X][Y][Z] == 0 || plane[X][Y][Z] == 1)
    {
        plane[X][Y][Z] = length + 1;
    }
    else if(plane[X][Y][Z] > 15)
    {
        numFruit--;
        plane[X][Y][Z] = ++length + 1;
    }
    else
    {
        loseFlag = 1;
    }

    // Now convert plane to nextDisplay and
    // decrement the life of each peice of the snake by 1
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
                    if(plane[i][j][k] == 16) {plane[i][j][k] = 1; numFruit--;}
                    if(--plane[i][j][k] !=0)
                    {
                        nextDisplay[k]|= placeHolder;
                    }
                }
                placeHolder = placeHolder << 1;
            }
        }
    }
    return 0;
}

void nextFrame(void)
{
    // Convert nextDisplay into currentDisplay
    INTCONbits.GIE = 0;
    for(i=0;i<8;i+=2)
    {
        currentDisplay[i] = nextDisplay[i>>1];
        currentDisplay[i+1] = (nextDisplay[i>>1] >> 8);
    }
    INTCONbits.GIE = 1;
}

void updateDirection(void)
{
    //Consider putting a delay here for interupt sake
    // Or set the clock ratio lower. that's a good idea too!

    // Get buttons being pressed DISABLE INTERRUPTS!
    INTCONbits.GIE = 0;
    controllerUpdate();
    INTCONbits.GIE = 1;
    
    oldDirection = direction;

    // Update Direction
    if(LEFT && direction !=1){ direction = 0; }
    else if (RIGHT && direction !=0){ direction = 1; }
    else if (AWAY && direction !=3){ direction = 2; }
    else if (TOWARDS && direction !=2){ direction = 3; }
    else if (DOWN && direction !=5){ direction = 4; }
    else if (UP && direction !=4){ direction = 5; }
    else if (START) pause();

    // Find next position
    nextX=X;
    nextY=Y;
    nextZ=Z;
    switch (direction)
    {
        case 0: // 0 = Left     (-X) Joy Left or Left Bumper or Left D-Pad
            if(X-1 < 0) nextX = 3;
            else nextX=X-1;
        break;
        
        case 1: // 1 = Right    (+X) Joy Right or Right Bumper or Right D-Pad
            if(X+1 > 3) nextX = 0;
            else nextX=X+1;
        return;
        
        case 2: // 2 = Away     (-Y) Joy Up or Up D-Pad
            if(Y+1 > 3) nextY = 0;
            else nextY = Y+1;
        return;
        
        case 3: // 3 = Toward   (+Y) Joy Down or Down D-Pad
            if(Y-1 < 0) nextY = 3;
            else nextY = Y-1;
        return;
        
        case 4: // 4 = Down     (-Z) B or X
            if(Z-1 < 0) nextZ = 3;
            else nextZ = Z-1;
        return;
        
        case 5: // 5 = Up       (+Z) A or Y
            if(Z+1 > 3) nextZ = 0;
            else nextZ = Z+1;
        return;
        
        default:
            // Why would it come here?
            //error(0x10);
        return;
    
    }
}

void pause()
{
    direction=oldDirection;
    INTCONbits.GIE=0;
    while(!START)
    {
        //controllerUpdate();
    }
    INTCONbits.GIE=1;
}

void addFruit(void)
{
    char a,b,c;



    if(numFruit == maxFruit) return;

    while(1)
    {    
        a = (rand() % 4);
        b = (rand() % 4);
        c = (rand() % 4);

        if(plane[a][b][c]==0)
        {
            plane[a][b][c] = 43;
            numFruit++;
            return;
        }
        flash(0x10);
    }

}

