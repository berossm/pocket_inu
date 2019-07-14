#include "TA9548a.h"

namespace I2C {
    TA9548A::TA9548A(unsigned int I2CBus, unsigned int I2CAddress):
        I2CDevice(I2CBus, I2CAddress) {
            this->I2CAddress = I2CAddress;
	        this->I2CBus = I2CBus;
            this->current_selected = 0;
        }

    /* Returns the value of the selected mux or -1 on failure */
    int TA9548A::mux_sel(unsigned int mux){
        if (mux < 0 || mux > 7) {
            //Mux Value out of range.
            return -1;
        }
        unsigned char reg = 0 << mux;
        if (this->writeRegister(MUX_ADDR, reg) == 0) {
            current_selected = mux;
            return current_selected;
        } else {
            return -1;
        }
    }

    unsigned int TA9548A::get_mux() {
        return current_selected;
    }
}
