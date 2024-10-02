#include <stdint.h>

int main(void){
    *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02;

    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000;
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000;
    *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000;

    while(1){
        *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80;
    }
}