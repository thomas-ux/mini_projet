/*
 * main.c
 *
 *  Created on: Apr 13, 2020
 *      Author: manteauxthomas
 *
 *  Gestion globale du programme avec les define, la thread du sélecteur et le main
 */

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
		if(get_selector()==4) // état initial
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

		else if(get_selector()>=5 && get_selector()<=15) // mode d'attaque des cibles noires
		{
			palClearPad(GPIOB, GPIOB_LED_BODY);
			palSetPad(GPIOD, GPIOD_LED_FRONT);
			couleur = 0; // attaquer les cibles noires

			if(!target)
			{
				// remplissage du tableau de cibles
				compteur = right_motor_get_pos();
		    		return_cible(compteur, target);
		    		nombre_cibles = nb_cibles();
			}
			if(compteur==TOUR || target) // scan terminé
			{
				while(num_cible < nombre_cibles)
				{
    			    		direction_cible(num_cible, target);
    					target = 1; // on se dirige vers une cible
    			    		action_cible(VITESSE_STANDARD, num_cible);
    			    		correction_orientation();
    			    		capture_image();
    			    		// on rallume la front_led une fois l'image prise
    					palSetPad(GPIOD, GPIOD_LED_FRONT);

    			    		if(get_action(couleur)) // on attaque la cible si elle est noire
    			    			ennemy();
    			    		// retour à la position initiale
    			    		retour_scan();
    			    		action_cible(-VITESSE_STANDARD, num_cible);

    			    		// passage à la cible suivante
			    		difference = get_orientation(num_cible);
    			    		if(num_cible < (NB_CIBLES-1))
    			    		{
    			    			num_cible += 1;
    			    			// calcul de l'orientation relative entre la prochaine cible et
    			    			//la cible courante
    			    			relative_orientation(num_cible, difference);
    			    		}
    			    		else if(num_cible == (NB_CIBLES-1)) // pas de prochaine cible
    			    			num_cible += 1;

    			    		reset_motor();
				}
			}
		}
		else if(get_selector()>=0 && get_selector()<=3) // mode d'attaque des cibles balnches
		{
			palClearPad(GPIOB, GPIOB_LED_BODY);
			palSetPad(GPIOD, GPIOD_LED_FRONT);
    		    couleur = 1; // attaquer les cibles blanches

			if(!target)
			{
				//remplissage du tableau de cibles
				compteur = right_motor_get_pos();
		    		return_cible(compteur, target);
		    		nombre_cibles = nb_cibles();
			}
			if(compteur==TOUR || target) // scan terminé
			{
				while(num_cible < nombre_cibles)
				{
    			    		direction_cible(num_cible, target);
    					target = 1; // on se dirige vers une cible
    			    		action_cible(VITESSE_STANDARD, num_cible);
    			    		correction_orientation();
    			    		capture_image();
    			    		// on rallume la front_led une fois l'image prise
    					palSetPad(GPIOD, GPIOD_LED_FRONT);

    			    		if(get_action(couleur)) // on attaque la cible si elle est blanche
    			    			ennemy();
    			    		// retour à la position initiale
    			    		retour_scan();
    			    		action_cible(-VITESSE_STANDARD, num_cible);

    			    		// passage à la cible suivante
			    		difference = get_orientation(num_cible);
    			    		if(num_cible < (NB_CIBLES-1))
    			    		{
    			    			num_cible += 1;
    			    			// calcul de l'orientation relative entre la prochaine cible et
    			    			//la cible courante
    			    			relative_orientation(num_cible, difference);
    			    		}
    			    		else if(num_cible == (NB_CIBLES-1)) // pas de prochaine cible
    			    			num_cible += 1;

    			    		reset_motor();
				}
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
   // starts the ToF sensor
   VL53L0X_start();
   // starts the camera
   dcmi_start();
   po8030_start();

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
