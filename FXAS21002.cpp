#include "FXAS21002.h"

#define FXAS_STATUS         0x00    //R
#define FXAS_OUT_X_MSB      0x01    //R
#define FXAS_OUT_X_LSB      0x02    //R
#define FXAS_OUT_Y_MSB      0x03    //R
#define FXAS_OUT_Y_LSB      0x04    //R
#define FXAS_OUT_Z_MSB      0x05    //R
#define FXAS_OUT_Z_LSB      0x06    //R
#define FXAS_DR_STATUS      0x07    //R
#define FXAS_F_STATUS       0x08    //R
#define FXAS_F_SETUP        0x09    //R/W
#define FXAS_F_EVENT        0x0a    //R
#define FXAS_INT_SRC_FLAG   0x0b    //R
#define FXAS_WHO_AM_I       0x0c    //R
#define FXAS_CTRL_REG0      0x0d    //R/W
#define FXAS_RT_CFG         0x0e    //R/W
#define FXAS_RT_SRC         0x0f    //R
#define FXAS_RT_THS         0x10    //R/W
#define FXAS_RT_COUNT       0x11    //R/W
#define FXAS_TEMP           0x12    //R
#define FXAS_CTRL_REG1      0x13    //R/W
#define FXAS_CTRL_REG2      0x14    //R/W
#define FXAS_CTRL_REG3      0x15    //R/W

namespace I2C {

    FAXS21002::FAXS21002(unsigned int I2CBus, unsigned int I2CAddress=0x21):
	    I2CDevice(I2CBus, I2CAddress), temp(0){   // this member initialisation list calls the parent constructor
	        this->I2CAddress = I2CAddress;
	        this->I2CBus = I2CBus;
            //Set FIFO to circular buffer
            this->writeRegister(FXAS_F_SETUP, (0b01 << 6));
            unsigned char ctl1 = this->readRegister(FXAS_CTRL_REG1);
            ctl1 = ctl1 & 0xFD; //Disable Active
            this->writeRegister(FXAS_CTRL_REG1, ctl1);
    }

    int FAXS21002::activate(){
        int count = 0;
        unsigned char ctl1 = this->readRegister(FXAS_CTRL_REG1);
        while (((ctl1 & 0x01) != 1) && (count < 10)){
            ctl1 = this->readRegister(FXAS_CTRL_REG1);
            count++;
        }
        if (count < 10){
            ctl1 = ctl1 | 0x02; //Set to active
            this->writeRegister(FXAS_CTRL_REG1, ctl1);
            return 0;
        } else {
            return 1;
        }
        
    }

    int FAXS21002::readSensorState(){
        this->registers = this->readRegisters(DATA_BUFF_SIZE, 0x00);
        //TODO: Do we need to address overflow?
        //TODO: Do we want to use watermake?
        int data_packets = this->*(registers) & 0x3F;
        while (data_packets > 0){
            this->last_pitch = this->combineRegisters(*(registers+FXAS_OUT_X_MSB), *(registers+FXAS_OUT_X_LSB));
            this->last_roll = this->combineRegisters(*(registers+FXAS_OUT_Y_MSB), *(registers+FXAS_OUT_Y_LSB));
            this->last_yaw = this->combineRegisters(*(registers+FXAS_OUT_Z_MSB), *(registers+FXAS_OUT_Z_LSB));
            this->pitch_queue.push_back(this->last_pitch);
            this->roll_queue.push_back(this->last_roll);
            this->yaw_queue.push_back(this->last_yaw);
        }
        this->temp = this->readRegister(FXAS_TEMP);
    }

    int FAXS21002::get_last_pitch() {
        return this->last_pitch;
    }

    int FAXS21002::get_last_roll() {
        return this->last_roll;
    }

    int FAXS21002::get_last_yaw(){
        return this->last_yaw;
    }

    int FAXS21002::queue_size() {
        return this->pitch_queue.size();
    }

    int FAXS21002::pop_data(int* pitch, int* roll, int* yaw) {
        int q_size = this->pitch_queue.size();
        if ( q_size == 0){
            return -1;
        } else {
            pitch = this->pitch_queue.pop_front();
            roll = this->roll_queue.pop_front();
            yaw = this->yaw_queue.pop_front();
            return (q_size - 1);
        }
    }
    
    int FAXS21002::last_temp(){
        return (int)this->temp
    }

    short FAXS21002::combineRegisters(unsigned char msb, unsigned char lsb){
        //shift the MSB left by 8 bits and OR with LSB
        return ((short)msb<<8)|(short)lsb;
    }

}