#pragma once
#include "common.hpp"
#include <array>
#include <cstdint>
#include <random>

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

class chip8
{
    
private:

    std::array<u8,0xFFF> m_mem;

    std::array<u8, 0xF> m_v;
    std::array<u16,0XF> m_stack;
    std::array<u8, 0xF> m_keypad;
    u16 m_I;

    u8 m_delay;
    u8 m_sound;

    u16 m_pc;
    u8 m_sp;

    std::default_random_engine randGen;
    std::uniform_int_distribution<u8> randByte;

public:
    uint32_t m_gxf[64*32]{};

    void reset();
    void cycle();
    chip8();


};