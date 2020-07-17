/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>

/* Freedom metal includes. */
#include <metal/exception.h>
#include <metal/platform.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

static __attribute__ ((aligned(16))) StackType_t xISRStack[ configMINIMAL_STACK_SIZE  + 1 ] __attribute__ ((section (".heap"))) ;
__attribute__ ((aligned(4))) uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((section (".heap")));


__attribute__((constructor)) static void FreeRTOS_init(void);

#if( configENABLE_FPU == 1 )
	extern void prvSetupFPU( void );
#endif /* configENABLE_FPU */

__attribute__((constructor)) static void FreeRTOS_init(void)
{
	extern BaseType_t xPortFreeRTOSInit( StackType_t xIsrStack );
	
	/*
	 * Call xPortFreeRTOSInit in order to set xISRTopStack
	 */
	if ( 0 != xPortFreeRTOSInit((StackType_t)&( xISRStack[ ( (configMINIMAL_STACK_SIZE - 1) & ~portBYTE_ALIGNMENT_MASK ) ] ))) {
		_exit(-1);
	}
}


void FreedomMetal_InterruptHandler( void )
{	
    portUBASE_TYPE mcause, hartid, mtvec;
	
    __asm__ __volatile__ (
		"csrr %0, mhartid \n"
		"csrr %1, mcause \n"
		"csrr %2, mtvec \n"
		: "=r"(hartid), "=r"(mcause), "=r"(mtvec)
		::
	);

    __metal_interrupt_handler(mcause);
}

void FreedomMetal_ExceptionHandler( void )
{
    uintptr_t mcause;
    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));
    __metal_exception_handler(mcause);
}
