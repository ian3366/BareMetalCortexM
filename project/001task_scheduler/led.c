#include<stdint.h>
#include "led.h"



void delay(uint32_t count)
{
  for(uint32_t i = 0 ; i < count ; i++);
}

void init_led_all(void)
{

	uint32_t *pRccAhb1Enr = (uint32_t*)0x40023830;
	uint32_t *pGpiodModeReg = (uint32_t*)0x40020400;


	*pRccAhb1Enr |= ( 1 << 1);
	//configure LED_GREEN
	*pGpiodModeReg |= ( 1 << (2 * LED_GREEN));
	*pGpiodModeReg |= ( 1 << (2 * LED_RED));
	*pGpiodModeReg |= ( 1 << (2 * LED_BLUE));


    led_off(LED_GREEN);
    led_off(LED_RED);
    led_off(LED_BLUE);



}

void led_on(uint8_t led_no)
{
  uint32_t *pGpiodDataReg = (uint32_t*)0x40020414;
  *pGpiodDataReg |= ( 1 << led_no);

}

void led_off(uint8_t led_no)
{
	  uint32_t *pGpiodDataReg = (uint32_t*)0x40020414;
	  *pGpiodDataReg &= ~( 1 << led_no);

}
