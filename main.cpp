#include "mbed.h"
#include "main.h"
#include "TextLCD.h"
#include "LocDetection.h"

DigitalOut Track(p19); //Digital output bit used to drive track power via H-bridge
DigitalIn  killSwitch(p30); //Kill Switch
DigitalOut ledGreen(p27);  // led Green
DigitalOut ledRed(p28);  // led Red
TextLCD lcd(p22, p21, p23, p24, p25, p26);

Serial pc(USBTX, USBRX); // tx, rx

// DO NOT EDIT THIS FUNCTION
// CODE FROM https://os.mbed.com/users/4180_1/notebook/controlling-a-model-railroad-using-mbed/
void DCC_send_command(unsigned int address, unsigned int inst, unsigned int repeat_count)
{
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

void lightStatus(bool status)
{
    if (status) {
        ledGreen = 0;
        ledRed = 1;
    } else {
        ledGreen = 1;
        ledRed = 0;
    }
}

class Train
{
public:
    Train() {}
    Train(unsigned int address, int startPosition)
    {
        DCCaddress = address;
        pos = startPosition;
        updateExpectedPositions();
    }

    void sendCommand(unsigned int inst)
    {
        DCC_send_command(DCCaddress,inst,1);
    }

    bool isInIntersection(int positionOtherTrain, int counter)
    {
        //if(counter == 20) {
//            pc.printf("future pos of the train at %d is %d %d \n", pos, fpos1, fpos2);
//        }
        if( (positionOtherTrain == 5
                || positionOtherTrain == 6
                || positionOtherTrain == 7
                || positionOtherTrain == 8
                || positionOtherTrain == 9)
                && (pos == 11 || pos == 12 || pos == 21 || pos == 3)) {
            return true;
        }

        if(fpos1 == positionOtherTrain
                || fpos2 == positionOtherTrain) {
            return true;
        }


        return false;

    }

    int updatePos(int p)
    {
        if (p == fpos1 || p == fpos2 || p == pos) {
            pos = p;
            updateExpectedPositions();
            return 1;
        }
        return -1;
    }

    int getPos()
    {
        return pos;
    }


private:
    unsigned int DCCaddress;
    int pos;
    int fpos1;
    int fpos2;

    void updateExpectedPositions()
    {
        int path[] = {1,22,2,21,3,9,8,7,6,5,11,12,13,0};
//        int path[] = {0,13,12, 11, 5, 6, 7, 8, 9, 3, 21, 2, 22, 1};
        int size = sizeof(path) / sizeof(path[0]);

        int i = 0;
        while(i < size) {
            if (path[i] == pos)
                break;
            i++;
        }
        fpos1 = (i == size - 1) ? path[0] : path[i + 1];
        i = (i == size - 1) ? 0 : i + 1;
        fpos2 = (i == size - 1) ? path[0] : path[i + 1];
    }
};

class TrainTracker
{
public:
    TrainTracker(Train* a, Train* b)
    {
        trainA = a;
        trainB = b;
    }

    int updateTracker()
    {
        int pos;

        if(int0_changed)
            pos = read_int0_data();
        else if(int1_changed)
            pos = read_int1_data();
        else
            return 1;


        if ((*trainA).updatePos(pos) == 1 || (*trainB).updatePos(pos) == 1) {
            printTrainPositions();
            return 1;
        }
        return -1;
    }

private:
    Train* trainA;
    Train* trainB;

    void printTrainPositions()
    {
        lcd.cls();
        lcd.printf("TRAIN A (D%d)\nTRAIN B (D%d)", (*trainA).getPos(),(*trainB).getPos());
    }
};

void setSwitch1(int switchPos)
{
    unsigned int DCCaddress = 0x06;
    unsigned int DCCinst = 0x80;

    if (switchPos == 1) {
        DCCinst = 0x81;
        DCC_send_command(DCCaddress,DCCinst,2);
        DCCinst = 0x80;
        DCC_send_command(DCCaddress,DCCinst,2);
        pc.printf("Switch went to switchpos == 1");

    } else {
        DCCinst = 0x81;
        DCC_send_command(DCCaddress,DCCinst,2);
        pc.printf("Switch went to switchpos == 0");

    }
}
void setSwitch2(int switchPos)
{
    unsigned int DCCaddress = 0x06;
    unsigned int DCCinst = 0x80;

    if (switchPos == 1) {
        pc.printf("Switch went to switchpos == 1");

        DCCinst = 0x82;
        DCC_send_command(DCCaddress,DCCinst,2);
        DCCinst = 0x80;
        DCC_send_command(DCCaddress,DCCinst,2);
    } else {
        pc.printf("Switch went to switchpos == 0");

        DCCinst = 0x82;
        DCC_send_command(DCCaddress,DCCinst,2);
    }
}

void setSwitch3(int switchPos)
{
    unsigned int DCCaddress = 0x06;
    unsigned int DCCinst = 0x80;

    if (switchPos == 1) {
        pc.printf("Switch went to switchpos == 1");

        DCCinst = 0x84;
        DCC_send_command(DCCaddress,DCCinst,2);
        DCCinst = 0x80;
        DCC_send_command(DCCaddress,DCCinst,2);
    } else {
        pc.printf("Switch went to switchpos == 0");

        DCCinst = 0x84;
        DCC_send_command(DCCaddress,DCCinst,2);
    }
}

void setSwitch4(int switchPos)
{
    unsigned int DCCaddress = 0x06;
    unsigned int DCCinst = 0x80;

    if (switchPos == 1) {
        pc.printf("Switch went to switchpos == 1");

        DCCinst = 0x88;
        DCC_send_command(DCCaddress,DCCinst,2);
        DCCinst = 0x80;
        DCC_send_command(DCCaddress,DCCinst,2);
    } else {
        pc.printf("Switch went to switchpos == 0");

        DCCinst = 0x88;
        DCC_send_command(DCCaddress,DCCinst,2);
    }
}
int main()
{
    lcd.cls();
    lcd.printf("Balkan Express\n");
    init_mcp();
    init();

    Train trainA(TRAIN2, 12);
    Train trainB(TRAIN3, 2);
//    Train trainB(TRAIN3, 12);
    Train* trainAPtr;
    trainAPtr = &trainA;
    Train* trainBPtr;
    trainBPtr = &trainB;


    int counter = 0;
    TrainTracker trainTracker(trainAPtr, trainBPtr);

    while(1) {
        //Stop All Trains On Track
        //if(killSwitch) {
//            lightStatus(0);
//            trainA.sendCommand(FWESTOP);
//            trainB.sendCommand(FWESTOP);
//            continue;
//        }
        lightStatus(1);
        if(trainA.isInIntersection(trainB.getPos(), counter) == true) {
            trainA.sendCommand(FWESTOP);
        } else {
            trainA.sendCommand(BW50);
        }
        if(trainB.isInIntersection(trainA.getPos(), counter ) == true) {
            trainB.sendCommand(FWESTOP);
        } else {
            trainB.sendCommand(BW50);
        }
        if(counter == 250) {
//            setSwitch2(1);
//            setSwitch2(1);
//            setSwitch3(1);
            setSwitch4(1);
        }

        if(counter == 500) {
//            setSwitch2(0);
//            setSwitch2(0);
//            setSwitch3(0);
            setSwitch4(0);
            counter = 0;
        }
        counter = counter + 1;


        // If we lose track of the trains stop and break everything.
        // if (trainTracker.updateTracker() == -1) {
//            trainA.sendCommand(FWESTOP);
//            trainB.sendCommand(FWESTOP);
//            break;
//        }
    }
}

