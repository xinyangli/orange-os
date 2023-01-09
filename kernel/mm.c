#include <x86def.h>
#include <mm.h>

u32 seg2phys(u16 seg) {
    DESCRIPTOR *p_dest = &gdt[seg >> 3];
    return (p_dest->base_high << 24 | p_dest->base_mid << 16 |
            p_dest->base_low);
}