/*
 * led.h
 *
 *  Created on: Jun 1, 2024
 *      Author: z0976
 */

#ifndef LED_H_
#define LED_H_

#define LED_GREEN  0
#define LED_BLUE   7
#define LED_RED    14
#define LED_Dark   ff



#define DELAY_COUNT_1MS 		 1250U
#define DELAY_COUNT_1S  		(1000U * DELAY_COUNT_1MS)
#define DELAY_COUNT_500MS  		(500U  * DELAY_COUNT_1MS)
#define DELAY_COUNT_250MS 		(250U  * DELAY_COUNT_1MS)
#define DELAY_COUNT_125MS 		(125U  * DELAY_COUNT_1MS)

void init_led_all(void);
void led_on(uint8_t led_no);
void led_off(uint8_t led_no);
void delay(uint32_t count);

#endif /* LED_H_ */
