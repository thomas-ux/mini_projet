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

#include "audio/audio_thread.h"
#include "audio/play_melody.h"

//.

static THD_WORKING_AREA(selector_thd_wa, 2048);

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

static THD_FUNCTION(selector_thd, arg)
{
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    int32_t compteur = 0, difference = 0;
    uint8_t num_cible = 0, nombre_cibles = 0;
    bool target = 0, couleur = 0;
    init_tab_cible();

	while(1)
	{
		if(get_selector()==0)
		{
			 palClearPad(GPIOD, GPIOD_LED_FRONT);
			 palSetPad(GPIOB, GPIOB_LED_BODY);

			 reset_motor();
			 target = 0;
			 compteur = 0;
			 num_cible = 0;
			 difference = 0;
			 nombre_cibles = 0;
			 init_tab_cible();
		}
		else if(get_selector()>=1 && get_selector()<8)
		{
			palClearPad(GPIOB, GPIOB_LED_BODY);
			palSetPad(GPIOD, GPIOD_LED_FRONT);
			couleur = 0;

			if(!target)
			{
				compteur = right_motor_get_pos();
		    		return_cible(compteur, target);
		    		nombre_cibles = nb_cibles();
			}
			if(compteur==TOUR || target)
			{
				while(num_cible < nombre_cibles)
				{
    			    		direction_cible(num_cible, target);
    					target = 1;
    			    		action_cible(VITESSE_STANDARD, num_cible);
    			    		correction_orientation();
    			    		capture_image();

    			    		if(get_action(couleur))
    			    		{
    			    			//playMelody(IMPOSSIBLE_MISSION, ML_SIMPLE_PLAY, NULL);
    			    			ennemy();
    			    		}
    			    		action_cible(-VITESSE_STANDARD, num_cible);
    			    		if(get_action(couleur))
    			    			stopCurrentMelody();

			    		difference = get_orientation(num_cible);
    			    		if(num_cible < (NB_CIBLES-1))
    			    		{
    			    			num_cible += 1;
    			    			relative_orientation(num_cible, difference);
    			    		}

    			    		reset_motor();
				}
				//reset_motor();
			}
		}
		else
		{
			palClearPad(GPIOB, GPIOB_LED_BODY);
			palSetPad(GPIOD, GPIOD_LED_FRONT);
    		    couleur = 1;

			if(!target)
			{
				compteur = right_motor_get_pos();
		    		return_cible(compteur, target);
		    		nombre_cibles = nb_cibles();
			}
			//chprintf((BaseSequentialStream *)&SD3, "nb = %d\n", nombre_cibles);
			if(compteur==TOUR || target)
			{
				while(num_cible < nombre_cibles)
				{
    			    		direction_cible(num_cible, target);
    					target = 1;
    			    		action_cible(VITESSE_STANDARD, num_cible);
    			    		capture_image();

    			    		if(get_action(couleur))
    			    		{
    			    			//playMelody(SIMPSON, ML_SIMPLE_PLAY, NULL);
    			    			ennemy();
    			    		}
    			    		action_cible(-VITESSE_STANDARD, num_cible);
    			    		if(get_action(couleur))
    			    			stopCurrentMelody();

			    		difference = get_orientation(num_cible);
    			    		if(num_cible < (NB_CIBLES-1))
    			    		{
    			    			num_cible += 1;
    			    			relative_orientation(num_cible, difference);
    			    		}

    			    		reset_motor();
				}
				//reset_motor();
			}
		}
	}
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
   dac_start();
   playMelodyStart();

   chThdCreateStatic(selector_thd_wa, sizeof(selector_thd_wa), NORMALPRIO, selector_thd, NULL);

   while(1)
    {
	   chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
