#include "Common.h"
#include <cmath>

namespace common {

std::string binary(u8 v) {
    std::string b;
    for (int x=7;x>=0;x--) {
        if (std::pow(2,x) <= v) {
            v=v-std::pow(2,x);
            b.append("1");
        } else {
            b.append("0");
        }
    }
    return b;
}

} //ns
