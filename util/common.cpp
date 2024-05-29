#include "common.hpp"

Color hexToColor(unsigned int hex)
{
    Color out;
    out.r = (hex>>16)&0xFF;
    out.g = (hex>>8)&0xFF;
    out.b = (hex) & 0xFF;
    out.a = 0xFF;
    return out;
}