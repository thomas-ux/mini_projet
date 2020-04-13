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

#define DISTANCE_MAX		8190
#define TOUR				1300

//.
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

int32_t return_cible(int32_t compteur, int32_t cible, bool target);

void direction_cible(int32_t cible);

static uint16_t mesure = DISTANCE_MAX;

static uint16_t state = 1;

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

    //VL53L0X_start();
    dcmi_start();
    po8030_start();
    process_image_start();

    int32_t compteur = 0;
    int32_t cible = 0;
    bool target = 0;

    while(1)
    {

       //partie pour la caméra
        	      	//chprintf((BaseSequentialStream *)&SDU1, "get_action() = %d \n", get_action(state));

        	        if(get_action(state))
        	        	{
        	        		state = 0;
        	        		right_motor_set_pos(0);
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
        	        	   state = 1;


    }
}

int32_t return_cible(int32_t compteur, int32_t cible, bool target)
{
 	//chprintf((BaseSequentialStream *)&SD3, "compteur = %d \n", compteur);
	if(compteur<TOUR && !target)
	{
	    	right_motor_set_speed(150);
	    	left_motor_set_speed(-150);

	    	if(VL53L0X_get_dist_mm() < mesure && VL53L0X_get_dist_mm() > 0)
	    	{
	    		mesure = VL53L0X_get_dist_mm();
	    		cible = compteur;
	    	}
	    //	return 0;
	}
 	else if(compteur==TOUR)
	{
 		right_motor_set_speed(0);
	    left_motor_set_speed(0);
	}

	return cible;
}

void direction_cible(int32_t cible)
{
	left_motor_set_pos(0);
	if(cible >= (TOUR/2)){
		while(left_motor_get_pos()<=(TOUR-cible)){
			right_motor_set_speed(-150);
			left_motor_set_speed(150);
		}
	}
	else{
	 	//chprintf((BaseSequentialStream *)&SD3, "cible %d = \n", cible);
		while((-left_motor_get_pos())<=cible){
			right_motor_set_speed(150);
			left_motor_set_speed(-150);
		}
	}
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
