#include "stm8s.h"
#include "stddef.h"
#include "tm1637.h"

#define CLK_H GPIO_WriteHigh(GPIOB, GPIO_PIN_4)
#define CLK_L GPIO_WriteLow(GPIOB, GPIO_PIN_4)
#define DIO_H GPIO_WriteHigh(GPIOB, GPIO_PIN_5)
#define DIO_L GPIO_WriteLow(GPIOB, GPIO_PIN_5)
#define DELAY for (volatile uint8_t i = 0; i < 25; i++)

static void
SendStart()
{
    DIO_H;
    DIO_L;
}

static void
SendStop()
{
    CLK_L;
    DIO_L;
    CLK_H;
    DIO_H;
    DELAY;
}

static void
ReadAck()
{
    CLK_L;
    DIO_H;
    CLK_H;
    // read here
    DIO_L; // prevent dio going high
}

static void
SendByte(uint8_t byte)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        CLK_L;
        (byte & (1 << i)) != 0 ? DIO_H : DIO_L;
        CLK_H;
    }
    ReadAck();
}

/*
    @brief show up to 6 bytes on display
    @param data data to display
    @param len data length, 1 to 6
    @param brightness 0 to 8, where 0 is display off
 */
void
tm1637_display(uint8_t *data, uint8_t len, uint8_t brightness)
{
    if (data == NULL || len == 0)
    {
        return;
    }

    if (len > 6)
    {
        len = 6;
    }

    if (brightness == 0)
    {
        /* display control: OFF */
        brightness = 0x80;
    }
    else
    {
        /* display control: ON and brightness level */
        brightness = 0x88 | ((brightness - 1) & 7);
    }

    SendStart();
    SendByte(0x40); // addr autoincrement
    SendStop();

    SendStart();
    SendByte(0xC0); // start with row 0
    for (uint8_t i = 0; i < len; i++)
    {
        SendByte(*(data + i));
    }
    SendStop();

    SendStart();
    SendByte(brightness);
    SendStop();
}

/* Not used since tm1637 has another order of bits */
static void
Send_I2C(void)
{
    I2C_GenerateSTART(ENABLE); for (volatile uint8_t i = 0; i < 255; i++) ;I2C_GenerateSTART(DISABLE);
    I2C_SendData(0x40); // addr autoincrement
    I2C_GenerateSTOP(ENABLE); for (volatile uint8_t i = 0; i < 255; i++); I2C_GenerateSTOP(DISABLE);

    I2C_GenerateSTART(ENABLE); for (volatile uint8_t i = 0; i < 255; i++); I2C_GenerateSTART(DISABLE);
    I2C_SendData(0xC0); // start with row 0
    I2C_SendData(0xAA); // 6 bytes of data
    I2C_SendData(0x55);
    I2C_SendData(0xAA);
    I2C_SendData(0x55);
    I2C_SendData(0xAA);
    I2C_SendData(0x55);
    I2C_GenerateSTOP(ENABLE); for (volatile uint8_t i = 0; i < 255; i++); I2C_GenerateSTOP(DISABLE);

    I2C_GenerateSTART(ENABLE); for (volatile uint8_t i = 0; i < 255; i++); I2C_GenerateSTART(DISABLE);
    I2C_SendData(0x8C); //display control: ON, 11/16
    I2C_GenerateSTOP(ENABLE); for (volatile uint8_t i = 0; i < 255; i++); I2C_GenerateSTOP(DISABLE);
}