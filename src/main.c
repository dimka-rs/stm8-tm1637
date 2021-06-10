/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/
#define CLK_H GPIO_WriteHigh(GPIOB, GPIO_PIN_4)
#define CLK_L GPIO_WriteLow(GPIOB, GPIO_PIN_4)
#define DIO_H GPIO_WriteHigh(GPIOB, GPIO_PIN_5)
#define DIO_L GPIO_WriteLow(GPIOB, GPIO_PIN_5)
#define DELAY for (volatile uint8_t i = 0; i < 25; i++)

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

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

static void
Send_BitBang(void)
{
    static uint8_t i = 0;
    i++;

    SendStart();
    SendByte(0x40); // addr autoincrement
    SendStop();

    SendStart();
    SendByte(0xC0); // start with row 0
    SendByte(i); // 6 bytes of data
    SendByte(i);
    SendByte(i);
    SendByte(i);
    SendByte(i);
    SendByte(i);
    SendStop();

    SendStart();
    SendByte(0x8C); //display control: ON, 11/16
    SendStop();
}

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

void
main(void)
{
    /* GPIO */
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW);
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW);

    /* UART */
    UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
               UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    UART1_Cmd(ENABLE);

    /* I2C */
    //I2C_Init(100000, 0x00, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 2);
    //I2C_Cmd(ENABLE);

    while (1)
    {

        UART1_SendData8('A');
        //Send_I2C();
        Send_BitBang();
        for (volatile uint32_t i = 0; i < 10000; i++);
    }
}



/***********************************************************************/

/* Interrupt example */
#if 0
void tim4_isr(void) __interrupt(ITC_IRQ_TIM4_OVF) {

}
#endif


/* Debug output */
#if 0
int putchar (int c)
{
    // this implementation is SDCC 3.8 specific
    // see sdcc stdio.h for details
    UART1->DR = (uint8_t) c;
    while (!(UART1->SR & UART1_FLAG_TXE));
    return c;
}
#endif

// This is called by some of the SPL files on error.
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    (void)file;
    (void)line;

    /* User can add their own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
        GPIO_WriteReverse(LED_PORT, LED_PIN);
        for (volatile uint32_t i = 0; i < 10000; i++);
    }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
