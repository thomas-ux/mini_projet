/*
 * control_motor.c
 *
 *  Created on: Apr 6, 2020
 *      Author: manteauxthomas
 */

#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <motors.h>
#include <control_motor.h>

static uint16_t state = 1;
static uint16_t compteur = 0;

static THD_WORKING_AREA(waControl_motor, 256);
static THD_FUNCTION(ControlMotor, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1)
    {
    		while((right_motor_get_pos()) <= 1310)
    		{
    			right_motor_set_speed(200);
    	    		left_motor_set_speed(-200);
    	    		//chprintf((BaseSequentialStream *)&SDU1, "position = %d\n", right_motor_get_pos());
    	    	state = 1;
    	    	//	compteur = right_motor_get_pos();
    	    		//chprintf((BaseSequentialStream *)&SD3, "%d\n", compteur);
    		}
    		right_motor_set_speed(0);
    		left_motor_set_speed(0);

    	    	state = 0;
    		chThdSleepMilliseconds(10000);
    }
}

void control_motor_start(void)
{
	chThdCreateStatic(waControl_motor, sizeof(waControl_motor), NORMALPRIO+1, ControlMotor, NULL);
}

uint16_t control_state(void)
{
	return state;
}

uint16_t control_compteur(void)
{
	return compteur;
}
