/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

/* LED1 toggle thread */
static void vLEDTask1(void *pvParameters) {
	bool LedState = false;
	while (1) {
		LedState = false;

		for (int i = 0; i<6; i++){
			LedState = (bool)!LedState;
			Board_LED_Set(1, LedState);
			vTaskDelay(configTICK_RATE_HZ / 6);
		}
		vTaskDelay(configTICK_RATE_HZ / 6);
		vTaskDelay(configTICK_RATE_HZ / 6);

		for (int i = 0; i<6; i++){
			LedState = (bool)!LedState;
			Board_LED_Set(1, LedState);
			vTaskDelay(configTICK_RATE_HZ / 2);
		}

		for (int i = 0; i<6; i++){
			LedState = (bool)!LedState;
			Board_LED_Set(1, LedState);
			vTaskDelay(configTICK_RATE_HZ / 6);
		}

		vTaskDelay(configTICK_RATE_HZ / 6);
		vTaskDelay(configTICK_RATE_HZ / 6);
		vTaskDelay(configTICK_RATE_HZ / 6);
		vTaskDelay(configTICK_RATE_HZ / 6);
	}
}

/* LED2 toggle thread */
static void vLEDTask2(void *pvParameters) {
	bool LedState = false;

	while (1) {
		Board_LED_Set(2, LedState);
		LedState = (bool) !LedState;

		/* About a 7Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ * 6);
	}
}

/* UART (or output) thread */
static void vUARTTask(void *pvParameters) {
	int tickCnt = 0;

	while (1) {

		if (!Chip_GPIO_GetPinState(LPC_GPIO, 0, 17)){
			tickCnt--;
			Board_LED_Set(0, true);
		}else{
			tickCnt++;
			Board_LED_Set(0, false);
		}

		DEBUGOUT("Tick: %d \r\n", tickCnt);

		/* About a 1s delay here */
		vTaskDelay(configTICK_RATE_HZ);
	}
}


int main(void) {

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
	// Set the LED to the state of "On"
#endif
#endif

	uint32_t pm = IOCON_DIGMODE_EN;

	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 17, IOCON_DIGMODE_EN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 17);


	/* LED1 toggle thread */
	xTaskCreate(vLEDTask1, "vTaskLed1",
			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(vLEDTask2, "vTaskLed2",
			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vUARTTask, "vTaskUart",
			configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	// Force the counter to be placed into memory
	volatile static int i = 0 ;
	// Enter an infinite loop, just incrementing a counter
	while(1) {
		i++ ;
	}
	return 0 ;
}
