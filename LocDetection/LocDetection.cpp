#include "mbed.h"
#include "TextLCD.h" 
#include "MCP23017.h" 

InterruptIn int0(p11), int1(p12); 
TextLCD lcd_screen(p22, p21, p23, p24, p25, p26); 
I2C i2c(p9, p10); 
MCP23017 *mcp; 
int train_loc;

void init_mcp() { 
    mcp = new MCP23017(i2c, 0x40); 
    mcp->_write(IODIRA, (unsigned char )0xff); 
    mcp->_write(IODIRB, (unsigned char )0xff); 
    mcp->_write(IPOLA, (unsigned char )0x00); 
    mcp->_write(IPOLB, (unsigned char )0x00); 
    mcp->_write(DEFVALA, (unsigned char )0xff); 
    mcp->_write(DEFVALB, (unsigned char )0xff); 
    mcp->_write(INTCONA, (unsigned char )0xff); 
    mcp->_write(INTCONB, (unsigned char )0xff); 
    mcp->_write(IOCONA, (unsigned char )0x2); 
    mcp->_write(IOCONB, (unsigned char )0x2); 
    mcp->_write(GPPUA, (unsigned char )0xff); 
    mcp->_write(GPPUB, (unsigned char )0xff); 
} 

void on_int0_change() { 
    // In your code, you might want to move this logic out of the interrupt routine 
    // (for instance, by setting a flag and checking this flag in your main-loop)     // This will prevent overhead. 
    wait_us(2000);  
    int sensor_data = mcp->_read(INTCAPA);     // The only thing we do with the interrupt signal is printing it 
    switch (sensor_data){
        case 0xFE: 
            train_loc = 0;
            break;
        case 0xFD:
            train_loc = 1;
            break; 
        case 0xFB:
            train_loc = 2;
            break;
        case 0xF7:
            train_loc = 3;
            break;
        case 0xEF:
            train_loc = 4;
            break;
        case 0xDF:
            train_loc = 5;
            break; 
        case 0xBF:
            train_loc = 6;
            break; 
        case 0x7F:
            train_loc = 7;
            break;  
        default:
            train_loc = -2;
            break;
    }
    
    lcd_screen.cls(); 
    lcd_screen.printf("TRAIN AT (D%d)\n Hex-code (0x%X)", train_loc, sensor_data); 
} 

void on_int1_change() { 
    wait_us(2000);  
    int sensor_data = mcp->_read(INTCAPB);    
    switch (sensor_data){
        case 0xFE: 
            train_loc = 8;
            break;
        case 0xFD:
            train_loc = 9;
            break; 
        case 0xFB:
            train_loc = 10;
            break;
        case 0xF7:
            train_loc = 11;
            break;
        case 0xEF:
            train_loc = 12;
            break;
        case 0xDF:
            train_loc = 13;
            break; 
        case 0xBF:
            train_loc = 21;
            break; 
        case 0x7F:
            train_loc = 22;
            break;  
        default:
            train_loc = -1;
            break;
    }
    lcd_screen.cls(); 
    lcd_screen.printf("TRAIN AT (D%d)\n Hex-code (0x%X)", train_loc, sensor_data); 
} 

void init() { 
    // Clear current interrupts 
    mcp->_read(GPIOA); 
    mcp->_read(GPIOB); 
    // Register callbacks 
    int0.fall(&on_int0_change); 
    int1.fall(&on_int1_change); 
    // Enable interrupts on the MCP 
    mcp->_write(GPINTENA, (unsigned char )0xff); 
    mcp->_write(GPINTENB, (unsigned char )0xff); 
    // Ready to go! 
} 