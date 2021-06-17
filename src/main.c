/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "tm1637.h"
#include "string.h" /* memset */
#include "main.h"

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/



void
main(void)
{
    uint8_t data[6] = {0};
    uint8_t brightness = 0; /* 0 to 9, 0 - display off */

    /* GPIO */
    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW);
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW);
    GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);

    /* UART */
    UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
               UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    UART1_Cmd(ENABLE);

    /* I2C */
    //I2C_Init(100000, 0x00, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 2);
    //I2C_Cmd(ENABLE);

    while (1)
    {
        while(GPIO_ReadInputPin(GPIOA, GPIO_PIN_1) == 0); //test button
        UART1_SendData8('A');
        tm1637_display(&data[0], sizeof(data), brightness);
        brightness++;
        if (brightness > 8)
        {
            brightness = 0;
        }
        memset(&data[0], 1 << (brightness-1), sizeof(data));
        for (volatile uint32_t i = 0; i < 100000; i++);
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
