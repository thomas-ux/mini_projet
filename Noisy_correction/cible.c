#include "ch.h"
#include "hal.h"
#include <chprintf.h>

#include <main.h>
#include "cible.h"
#include "motors.h"
#include "sensors/VL53L0X/VL53L0X.h"
#include <math.h>

static etat_cible tab_cible[NB_CIBLES] = {0};
//static uint16_t count = 0;

void init_tab_cible(void)
{
	for(int i=0; i<NB_CIBLES; i++)
	{
		tab_cible[i].distance = DISTANCE_MAX;
		tab_cible[i].orientation = 0;
	}
}

void tri_croissant(void)
{
	uint16_t min = DISTANCE_MAX;
	uint8_t i_min = 0;
	int32_t position = 0;
	for(int j=0; j<NB_CIBLES-1; j++)
	{
		for(int i=j; i<NB_CIBLES; i++)
		{
			if(tab_cible[i].distance < min)
			{
				min = tab_cible[i].distance;
				position = tab_cible[i].orientation;
				i_min = i;
			}
		}
		tab_cible[i_min].distance = tab_cible[j].distance;
		tab_cible[i_min].orientation = tab_cible[j].orientation;
		tab_cible[j].distance = min;
		tab_cible[j].orientation = position;
		min = DISTANCE_MAX;
	}
}

void return_cible(int32_t compteur, bool target)
{
	if(compteur<TOUR && !target)
	{
	    right_motor_set_speed(400);
	    left_motor_set_speed(-400);

	    if(VL53L0X_get_dist_mm()<DISTANCE_MAX && VL53L0X_get_dist_mm()>0)
	    {
	    	if((VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance) && (compteur<tab_cible[NB_CIBLES-1].orientation+100))
	    	{
	    		tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    		tab_cible[NB_CIBLES-1].orientation = compteur;
	    	}
	    	else if(compteur > tab_cible[NB_CIBLES-1].orientation+100)
	    	{
	    		tri_croissant();
	    		if(VL53L0X_get_dist_mm() < tab_cible[NB_CIBLES-1].distance)
	    		{
	    			tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    			tab_cible[NB_CIBLES-1].orientation = compteur;
	    		}
	    	}
	    }
	}
 	else if(compteur==TOUR)
	{
 		right_motor_set_speed(0);
	    left_motor_set_speed(0);
	}
}


void direction_cible(uint8_t num_cible)
{
	tri_croissant();
	for(int i=0; i<NB_CIBLES; i++)
		chprintf((BaseSequentialStream *)&SD3, " orientation = %d distance = %d\n", tab_cible[i].orientation, (tab_cible[i].distance));

	left_motor_set_pos(0);
	if(tab_cible[num_cible].orientation >= (TOUR/2)){
		while(left_motor_get_pos()<=(TOUR-tab_cible[num_cible].orientation)){
			right_motor_set_speed(-VITESSE_SCAN);
			left_motor_set_speed(VITESSE_SCAN);
		}
	}
	else{
		while((-left_motor_get_pos())<=tab_cible[num_cible].orientation){
			right_motor_set_speed(VITESSE_SCAN);
			left_motor_set_speed(-VITESSE_SCAN);
		}
	}
	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
}

void action_cible(void)
{
	while(pi_regulator())
	{
		right_motor_set_speed(pi_regulator());
		left_motor_set_speed(pi_regulator());
	}
	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
}

void friend(int16_t speed, uint8_t num_cible)
{
	while(VL53L0X_get_dist_mm() < tab_cible[num_cible].distance)
	{
		right_motor_set_speed(speed);
	    left_motor_set_speed(speed);
	}
	right_motor_set_speed(VITESSE_NULLE);
    left_motor_set_speed(VITESSE_NULLE);
}

void ennemy(void)
{
	right_motor_set_pos(0);
	while(right_motor_get_pos()<650)
	{
		right_motor_set_speed(-VITESSE_STRIKE);
    	   	left_motor_set_speed(VITESSE_STRIKE);
    	   	chprintf((BaseSequentialStream *)&SD3, "pos = %d\n", right_motor_get_pos());
	}
	right_motor_set_speed(VITESSE_NULLE);
    left_motor_set_speed(VITESSE_NULLE);
}

int16_t pi_regulator(void)
{

	float error = 0, speed = 0;

	static float sum_error = 0;

	error = CONSIGNE - VL53L0X_get_dist_mm();
	//chprintf((BaseSequentialStream *)&SD3, "error = %f mesure %d\n", error, VL53L0X_get_dist_mm());

	if(error >= (-ERROR_THRESHOLD) && error <= 0)
		return 0;

	sum_error += error;

	//we set a maximum and a minimum for the sum to avoid an uncontrolled growth
	if(sum_error > MAX_SUM_ERROR)
		sum_error = MAX_SUM_ERROR;
	else if(sum_error < -MAX_SUM_ERROR)
		sum_error = -MAX_SUM_ERROR;

	speed = -(KP * error + KI * sum_error);

    return (int16_t)speed;
}

void reset_motor(void)
{
	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
	right_motor_set_pos(POSITION_RESET);
	left_motor_set_pos(POSITION_RESET);
}
