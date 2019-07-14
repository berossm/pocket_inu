#ifndef FXAS21002_H_
#define FXAS21002_H_

#include "I2CDevice.h"
#include <queue>

#define DATA_BUFF_SIZE 0x07 //Registers 0x00 through 0x06 with status and data

namespace I2C {
    class FAXS21002:protected I2CDevice{
        public:
            FAXS21002(unsigned int I2CBus, unsigned int I2CAddress=0x21);
            virtual int readSensorState();
            virtual int get_last_pitch();
            virtual int get_last_roll();
            virtual int get_last_yaw();
            virtual int queue_size();
            virtual int pop_data();
            virtual int last_temp();

        private:
            unsigned int I2CBus, I2CAddress;
	        unsigned char *registers;
            int last_pitch;
            int last_roll;
            int last_yaw;
            std::queue<int> pitch_queue;
            std::queue<int> roll_queue;
            std::queue<int> yaw_queue;
            short temp;

            short combineRegisters(unsigned char msb, unsigned char lsb);
    }
}

#endif /* FXAS21002_H_ */
