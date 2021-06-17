#ifndef MYTIME_H
#define MYTIME_H

typedef struct mytime
{
    uint8_t h; /** @param hours */
    uint8_t m; /** @param minutes */
    uint8_t s; /** @param seconds */
} mytime_t;


void increment_time(mytime_t *time);
void encode_time(mytime_t *time, uint8_t *out);

#endif /* MYTIME_H */