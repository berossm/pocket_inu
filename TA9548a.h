#ifndef TA9548A_H_
#define TA9548A_H_

#include"I2CDevice.h"

#define MUX_ADDR 0x00

namespace I2C {
    class TA9548A:protected I2CDevice{
    public:
        TA9548A(unsigned int I2CBus, unsigned int I2CAddress=0x70);
        int mux_sel(unsigned int mux);
        unsigned int get_mux();


    private:
        int current_selected;
    };

};

#endif /* TA9548A_H */