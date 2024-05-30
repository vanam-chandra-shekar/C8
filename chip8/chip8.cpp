#include "chip8.hpp"
#include "common.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <random>

#define Location_OF_FONTS 0

static  std::array<u8,80> chip8Font {
    0xF0,0x90,0x90,0x90,0xF0, //0
    0x20,0x60,0x20,0x20,0x70, //1
    0xF0,0x10,0xF0,0x80,0xF0, //2
    0xF0,0x10,0xF0,0x10,0xF0, //3
    0x90,0x90,0xF0,0x10,0x10,//4
    0xF0,0x80,0xF0,0x10,0xF0,//5
    0xF0,0x80,0xF0,0x90,0xF0,//6
    0xF0,0x10,0x20,0x40,0x40,//7
    0xF0,0x90,0xF0,0x90,0xF0,//8
    0xF0,0x90,0xF0,0x10,0xF0,//9
    0xF0,0x90,0xF0,0x90,0x90,//a
    0xE0,0x90,0xE0,0x90,0xE0,//b
    0xF0,0x80,0x80,0x80,0xF0,//c
    0xE0,0x90,0x90,0x90,0xE0,//d
    0xF0,0x80,0xF0,0x80,0xF0,//e
    0xF0,0x80,0xF0,0x80,0x80,//f
};


void chip8::reset()
{
    std::fill(m_mem.begin()+chip8Font.size() , m_mem.end() , 0);

    std::copy(chip8Font.begin() , chip8Font.end() , m_mem.begin());

    std::fill(m_v.begin() , m_v.end() , 0);
    std::fill(m_stack.begin() , m_stack.end() , 0);
    std::fill(m_keypad.begin() , m_keypad.end() , 0);

    m_pc = 0x200;
    m_sp = 0;

    m_delay = 0;
    m_sound = 0;
}

chip8::chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    this->randByte  = std::uniform_int_distribution<u8>(0,255U);    
    this->reset();
}

void chip8::cycle()
{
    u16 opcode = (m_mem[m_pc]<<8) | m_mem[m_pc+1];

    m_pc += 2;

    if(m_delay) m_delay--;
    if(m_sound) m_sound--;

    switch (opcode & 0xF000) {
        
        case 0x0 :{
            switch (opcode & 0x00FF) {
                case 0xE0:{
                    memset(m_gxf,0, sizeof(m_gxf));
                    break;
                }
                case 0xEE:{
                    --m_sp;
                    m_pc = m_stack[m_sp];
                    break;
                }
                break;
            }
        }

        case 0x1:{
            m_pc = opcode & 0x0FFF;
            break;
        }

        case 0x2:{
            u16 addr = opcode & 0x0FFF;
            m_stack[m_sp] = m_pc;
            ++m_sp;
            m_pc = addr;
            break;
        }

        case 0x3:{
            u8 x = (opcode & 0x0F00)>>8;
            u8 kk = opcode & 0x00FF;

            if(m_v[x] == kk){
                m_pc +=2;
            }
            break;
        }

        case 0x4:{
            u8 x = (opcode & 0x0F00)>>8;
            u8 kk = opcode & 0x00FF;

            if(m_v[x] != kk){
                m_pc +=2;
            }
            break;
        }

        case 0x5:{
            u8 x = (opcode & 0x0F00)>>8;
            u8 y = (opcode & 0x00F0)>>4;

            if(m_v[x] == m_v[y]){
                m_pc+=2;
            }
            break;
        }

        case 0x6:{
            u8 x = (opcode& 0x0F00)>>8;
            u8 kk = (opcode & 0x00FF);

            m_v[x] = kk;
            break;
        }

        case 0x7:{
            u8 x = (opcode & 0x0F00)>>8;
            u8 kk = (opcode & 0x00FF);

            m_v[x] += kk;
            break;
        }

        case 0x8:{
            u8 x = (opcode & 0x0F00)>>8;
            u8 y = (opcode & 0x00F0)>>4;

            switch (opcode & 0x000F) {
                
                case 0x0: m_v[x]  = m_v[y]; break;
                case 0x1: m_v[x] |= m_v[y]; break;
                case 0x2: m_v[x] &= m_v[y]; break;
                case 0x3: m_v[x] ^= m_v[y]; break;

                case 0x4:{
                    u16 sum = m_v[x] + m_v[y];

                    if(sum > 255){
                        m_v[0xF] = 1;
                    }else{
                        m_v[0xF] = 0;
                    }

                    m_v[x] = sum & 0xFF;
                    break;
                }

                case 0x5:{
                    if(m_v[x] > m_v[y]){
                        m_v[0XF] = 1;
                    }else{
                        m_v[0xF] = 0;
                    }
                    m_v[x] -= m_v[y];
                    break;
                }

                case 0x6:{
                    m_v[0xF] = (m_v[x] & 0x1);
                    m_v[x] >>= 1;
                    break;
                }

                case 0x7:{
                    
                    if(m_v[y] > m_v[x]){
                        m_v[0xF] = 1;
                    }else{
                        m_v[0xF] = 0;
                    }

                    m_v[x] = m_v[y] - m_v[x];
                    break;
                }

                case 0xE:{

                    m_v[0xF] = (m_v[x] & 0x80) >>7;
                    m_v[x] <<= 1;
                    break;
                }
            }
            break;
        }

        case 0x9:{
            u8 x = (opcode & 0x0F00)>>8;
            u8 y = (opcode & 0x00F0)>>4;

            if(m_v[x] != m_v[y]) m_pc+=2;
            break;
        }

        case 0xA:{
            u16 nnn = (opcode & 0x0FFF);
            m_I = nnn;
            break;
        }

        case 0xB:{
            u16 nnn = (opcode & 0x0FFF);
            m_pc = nnn + m_v[0];
            break;
        }

        case 0xC:{
            u8 x = (opcode & 0x0F00)>>8;
            u8 kk = (opcode & 0x00FF);

            m_v[x] = randByte(randGen) & kk;
            break;
        }

        case 0xD:{
	        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	        uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	        uint8_t height = opcode & 0x000Fu;

	        // Wrap if going beyond screen boundaries
	        uint8_t xPos = m_v[Vx] % VIDEO_WIDTH;
	        uint8_t yPos = m_v[Vy] % VIDEO_HEIGHT;

	        m_v[0xF] = 0;

	        for (unsigned int row = 0; row < height; ++row)
	        {
		        uint8_t spriteByte = m_mem[m_I + row];

		        for (unsigned int col = 0; col < 8; ++col)
		        {
			        uint8_t spritePixel = spriteByte & (0x80u >> col);
			        uint32_t* screenPixel = &m_gxf[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			        // Sprite pixel is on
			        if (spritePixel)
			        {
				        // Screen pixel also on - collision
				        if (*screenPixel == 0xFFFFFFFF)
				        {
					        m_v[0xF] = 1;
				        }

				        // Effectively XOR with the sprite pixel
				        *screenPixel ^= 0xFFFFFFFF;
			        }
		        }
	        }
            break;
        }

        case 0xE:{
            u8 x = (opcode & 0x0F00)>>8;

            switch (opcode & 0x00FF) {
                
                case 0x9E:{
                    u8 key = m_v[x];

                    if(m_keypad[key]) m_pc += 2;
                    break;
                }

                case 0xA1:{
                    u8 key = m_v[x];

                    if(!m_keypad[key]) m_pc +=2;
                    break;
                }

            }

            break;
        }

        case 0xF:{
            
            u8 x = (opcode & 0x0F00)>>8;

            switch (opcode & 0x00FF) {
                case 0x07:{
                    m_v[x] = m_delay;
                    break;
                }

                case 0x0A:{
                    
                    if(m_keypad[0])         m_v[x] = 0;
                    else if(m_keypad[1])    m_v[x] = 1;
                    else if(m_keypad[2])    m_v[x] = 2;
                    else if(m_keypad[3])    m_v[x] = 3;
                    else if(m_keypad[4])    m_v[x] = 4;
                    else if(m_keypad[5])    m_v[x] = 5;
                    else if(m_keypad[6])    m_v[x] = 6;
                    else if(m_keypad[7])    m_v[x] = 7;
                    else if(m_keypad[8])    m_v[x] = 8;
                    else if(m_keypad[9])    m_v[x] = 9;
                    else if(m_keypad[10])   m_v[x] = 10;
                    else if(m_keypad[11])   m_v[x] = 11;
                    else if(m_keypad[12])   m_v[x] = 12;
                    else if(m_keypad[12])   m_v[x] = 13;
                    else if(m_keypad[14])   m_v[x] = 14;
                    else if(m_keypad[15])   m_v[x] = 15;
                    else m_pc -= 2;
                    break;
                }

                case 0x15:{
                    m_delay = m_v[x];
                    break;
                }

                case 0x18:{
                    m_sound = m_v[x];
                    break;
                }

                case 0x1E:{
                    m_I += m_v[x];
                    break;
                }

                case 0x29:{
                    u8 digit = m_v[x];

                    m_I = Location_OF_FONTS + (5 * digit);

                }

                case 0x33:{
                    u8 value  = m_v[x];

                    m_mem[m_I+2] = value % 10;
                    value /= 10;

                    m_mem[m_I+1] = value %10;
                    value /= 10;

                    m_mem[m_I] = value;
                    break;
                }

                case 0x55:{
                    for(u8 i =0; i<= x; ++i){
                        m_mem[m_I +i] = m_v[i];
                    }
                    break;
                }

                case 0x65:{
                    for(u8 i=0 ; i<=x; ++i){
                        m_v[i] = m_mem[m_I + i];
                    }
                    break;
                }
            }


            break;
        }

    }


}

