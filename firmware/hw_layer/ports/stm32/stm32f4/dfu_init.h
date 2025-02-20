/*
 * dfu_init.h
 *
 * @date Aug 3, 2019
 * @author Andrey Belomutskiy, (c) 2012-2017
 */

#ifndef HW_LAYER_PORTS_STM32_STM32F4_DFU_INIT_H_
#define HW_LAYER_PORTS_STM32_STM32F4_DFU_INIT_H_

LDR R0, =0x40023844 // RCC_APB2ENR
LDR R1, =0x00004000 // ENABLE SYSCFG CLOCK
STR R1, [R0, #0]
LDR R0, =0x40013800 // SYSCFG_MEMRMP
LDR R1, =0x00000001 // MAP ROM AT ZERO
STR R1, [R0, #0]
LDR R0, =0x1FFF0000 // ROM BASE
LDR SP,[R0, #0] // SP @ +0
LDR R0,[R0, #4] // PC @ +4

#endif /* HW_LAYER_PORTS_STM32_STM32F4_DFU_INIT_H_ */
