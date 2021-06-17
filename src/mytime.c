#include "stm8s.h"
#include "stddef.h"
#include "mytime.h"

uint8_t lookup[] =
{
    0x00, // 0: 0000 0000
    0x80, // 1: 1000 0000
    0x40, // 2: 0100 0000
    0xC0, // 3: 1100 0000
    0x20, // 4: 0010 0000
    0xA0, // 5: 1010 0000
    0x60, // 6: 0110 0000
    0xE0, // 7: 1110 0000
    0x10, // 8: 0001 0000
    0x90, // 9: 1001 0000
};

void encode_time(mytime_t *time, uint8_t *out)
{
    if (time->h > 23 || time->m > 59 || time->s > 59 || out == NULL)
    {
        return;
    }

    *(out + 0) = lookup[time->h / 10];
    *(out + 1) = lookup[time->h % 10];
    *(out + 2) = lookup[time->m / 10];
    *(out + 3) = lookup[time->m % 10];
    *(out + 4) = lookup[time->s / 10];
    *(out + 5) = lookup[time->s % 10];
}

void increment_time(mytime_t *time)
{
    time->s++;
    if (time->s > 59)
    {
        time->s = 0;
        time->m++;
        if (time->m > 59)
        {
            time->m = 0;
            time->h++;
            if (time->h > 23)
            {
                time->h = 0;
            }
        }
    }
}

