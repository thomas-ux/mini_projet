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

int32_t return_cible(int32_t compteur, int32_t cible);

void position_cible(int32_t cible, int32_t compteur);



static uint16_t mesure = DISTANCE_MAX;



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
    int32_t compteur2 = 0;
    int32_t cible = 0;
    int32_t direction = 0;

    while(1)
    {
    	//partie pour la cible
    	compteur = right_motor_get_pos();


    	cible = return_cible(compteur, cible);
    	chprintf((BaseSequentialStream *)&SD3, "mesure = %d mm cible %d = \n", mesure, cible);

    	if(compteur==TOUR)
    	{
    		direction = (TOUR - cible)
    		right_motor_set_pos(0);
    		position_cible(cible, compteur);
    	}


    	//partie pour la caméraaa
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



int32_t return_cible(int32_t compteur, int32_t cible)
{
	if(compteur<TOUR)
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

void position_cible(int32_t cible, int32_t compteur)
{
	int32_t direction = (TOUR - cible);
	if(direction >= (TOUR/2))
	{
		right_motor_set_pos(cible);
		left_motor_set_pos(-cible);
	}
	else
	{
		right_motor_set_pos(-cible);
		left_motor_set_pos(cible);
	}
}

void go_cible(int32_t direction)
{
	if(direction >= (TOUR/2))
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
