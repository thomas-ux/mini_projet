/*
 * distance.c
 *
 *  Created on: Apr 6, 2020
 *      Author: manteauxthomas
 */

#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <distance.h>
#include "sensors/VL53L0X/VL53L0X.h"
#include <control_motor.h>
#include <motors.h>


static uint16_t dist_cible = 0;
static uint16_t cible = 0;

static THD_WORKING_AREA(waDistance, 256);
static THD_FUNCTION(Distance, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	uint16_t distance[1310] = {0};
	dist_cible = distance[0];
	int i = 0;

	while(1)
	{
		while(control_state())
		{
			//i = control_compteur();
			distance[i] = VL53L0X_get_dist_mm();
			//chprintf((BaseSequentialStream *)&SD3, "i = %d\n", distance[i]);
			i++;
		}

		//right_motor_set_speed(0);
		//left_motor_set_speed(0);

		for(uint16_t j=0; j<1310; j++)
		{
			if(distance[j]<dist_cible)
			{
				dist_cible = distance[j];
				cible = j;
			}
		}
		//chprintf((BaseSequentialStream *)&SD3, "cible = %d distance = %d\n", cible, dist_cible);
    }
}

void distance_start(void)
{
	chThdCreateStatic(waDistance, sizeof(waDistance), NORMALPRIO, Distance, NULL);
}

uint16_t distance_get_cible(void)
{
	return cible;
}

uint16_t distance_get_dist_cible(void)
{
	return dist_cible;
}
