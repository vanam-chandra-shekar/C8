#pragma once

#include "raylib.h"
#include <stdint.h>
#include <utility>



typedef uint8_t u8;
typedef uint16_t u16;

Color hexToColor(unsigned int hex);

const Color BACKGROUND {std::move(hexToColor(0x153448))};
const Color FOREGROUND {std::move(hexToColor(0x3C5B6F))};

