#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>

#include <arm_math.h>
#include "sensors/VL53L0X/VL53L0X.h"
#include "process_image.h"
#include "cible.h"
#include "selector.h"


static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

void SendUint8ToComputer(uint8_t* data, uint16_t size)
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

int main(void)
{
    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();
    //starts the USB communication
    usb_start();
    //inits the motors
    motors_init();

    VL53L0X_start();
    dcmi_start();
    po8030_start();
    process_image_start();

    int32_t compteur = 0;
    uint8_t num_cible = 0;
    int selector = 0;
    bool target = 0; //indique si on a une cible acquise ou non
    init_tab_cible();
    init_selector();

    while(1)
    {
    	selector = get_selector();
    	if(selector==0)
    	{
    		palClearPad(GPIOB, GPIOB_LED_BODY);
    	}
    	else
    	{
    		//partie pour la cible
    		compteur = right_motor_get_pos();


    		return_cible(compteur, target);
    		//chprintf((BaseSequentialStream *)&SD3, "mesure = %d mm cible %d = \n", mesure, cible);

    		if(compteur==TOUR)
    		{
    			target=1;
    			direction_cible(num_cible);
    		}




    		//partie pour la caméra
    		if(get_action())
    		{
    			while(right_motor_get_pos()<650)
    			{
    				right_motor_set_speed(400);
    				left_motor_set_speed(-400);
    			}
    		}
    		else
    		{
    			right_motor_set_speed(0);
    			left_motor_set_speed(0);
    		}
    		chThdSleepMilliseconds(1000);
    	}
    }
}


#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
