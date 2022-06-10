#include "mbed.h"
#include "TextLCD.h" 
#include "MCP23017.h" 

extern int train_loc;

void init_mcp();
void on_int0_change();
void on_int1_change();
void init();