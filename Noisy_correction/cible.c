#include "ch.h"
#include "hal.h"
#include <chprintf.h>

#include <main.h>
#include "cible.h"
#include "motors.h"
#include "sensors/VL53L0X/VL53L0X.h"

static uint16_t mesure = DISTANCE_MAX;

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
	chprintf((BaseSequentialStream *)&SD3, "mesure = %d mm cible %d = \n", mesure, cible);
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

