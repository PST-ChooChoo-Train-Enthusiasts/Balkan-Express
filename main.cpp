#include "mbed.h"
#include "main.h"
#include "TextLCD.h" 
#include "LocDetection.h" 
 
DigitalOut Track(p19); //Digital output bit used to drive track power via H-bridge
DigitalIn  killSwitch(p30); //Kill Switch
DigitalOut ledGreen(p28);  // led Green
DigitalOut ledRed(p27);  // led Red
TextLCD lcd(p22, p21, p23, p24, p25, p26);


// DO NOT EDIT THIS FUNCTION
// CODE FROM https://os.mbed.com/users/4180_1/notebook/controlling-a-model-railroad-using-mbed/
void DCC_send_command(unsigned int address, unsigned int inst, unsigned int repeat_count){
    unsigned __int64 command = 0x0000000000000000;
    unsigned __int64 temp_command = 0x0000000000000000;
    unsigned __int64 prefix = 0x3FFF;
    unsigned int error = 0x00; 
    error = address ^ inst;
    command = (prefix<<28)|(address<<19)|(inst<<10)|((error)<<1)|0x01;
    
    int i=0;
    while(i < repeat_count) {
        temp_command = command;
        for (int j=0; j<64; j++) {
            if((temp_command&0x8000000000000000)==0) {
                Track=0;
                wait_us(100);
                Track=1;
                wait_us(100);
            } else {
                Track=0;
                wait_us(58);
                Track=1;
                wait_us(58);
            }
            temp_command = temp_command<<1;
        }
        i++;
    }
}

void lightStatus(bool status){
    if (status){
        ledGreen = 0;
        ledRed = 1;
    } else {
        ledGreen = 1;
        ledRed = 0;
    }
}

class Train {       
    public:             
        bool updatePos(int x){
            int stations[] = {1,22,2,21,3,9,8,7,6,5,11,12,13,0};
            int n = sizeof(stations)/sizeof(stations[0]);
            
            if (blocked[0] != x && blocked[1] != x){
                return 0;
            }
            pos = x;
            
            int i = 0;
            while(i < n){
                if (stations[i] == pos) {
                    break;
                }
                i++;
            }
            
            blocked[0] = (i == 0) ? n : i - 1;
            blocked[1] = (i == n) ? 0 : i + 1;
            return 1;
        }
        
        int getPos(){
            return pos;
        }
        
        void updateSpeed(int x){
            speed = x;
        }
       
    private:
        int pos;
        int speed;
        int blocked[2];
        
};

int main(){
    lcd.cls(); 
    lcd.printf("Balkan Express\n"); 
    init_mcp();
    init();
    int speed;
    int loopCounter = 1;
    
    while(1) {
        //Stop All Trains On Track
        if(killSwitch || train_loc < 0){
            lightStatus(0);
            DCC_send_command(TRAIN2,FWESTOP,1);
            continue;
        }
        lightStatus(1);
        if( train_loc == 9){
            speed = FW50;
            loopCounter = 1;
        } else if( train_loc == 3 || train_loc == 21){
            speed = FW30;
            loopCounter = 1;
        } else if (train_loc == 2){
            speed = FWSTOP;
            loopCounter = 200;
            DCC_send_command(TRAIN2,speed,loopCounter); 
            speed = FW75;
            loopCounter = 600;
        } else {
            speed = FW75;
            loopCounter = 1;
        }
        
        DCC_send_command(TRAIN2,speed,loopCounter); 
        
    }
}
 