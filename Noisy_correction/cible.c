#include "ch.h"
#include "hal.h"
#include <chprintf.h>

#include <main.h>
#include "cible.h"
#include "motors.h"
#include "sensors/VL53L0X/VL53L0X.h"
#include <math.h>

static etat_cible tab_cible[NB_CIBLES] = {0};

uint16_t distance_min = DISTANCE_MAX;
int32_t orientation_correction = 0;

void init_tab_cible(void)
{
	for(int i=0; i<NB_CIBLES; i++)
	{
		tab_cible[i].distance = DISTANCE_MAX;
		tab_cible[i].orientation = TOUR;
		tab_cible[i].old_orientation = 0;
	}
}

void tri_croissant_distance(void)
{
	uint16_t min = DISTANCE_MAX;
	uint8_t i_min = 0;
	int32_t position = TOUR;
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
		tab_cible[j].old_orientation = tab_cible[j].orientation;
		min = DISTANCE_MAX;
	}
}

void tri_croissant_orientation(void)
{
	uint16_t min = TOUR;
	uint8_t i_min = 0;
	int32_t distance = DISTANCE_MAX;
	for(int j=0; j<NB_CIBLES-1; j++)
	{
		for(int i=j; i<NB_CIBLES; i++)
		{
			if(tab_cible[i].orientation < min)
			{
				min = tab_cible[i].orientation;
				distance = tab_cible[i].distance;
				i_min = i;
			}
		}
		tab_cible[i_min].distance = tab_cible[j].distance;
		tab_cible[i_min].orientation = tab_cible[j].orientation;
		tab_cible[j].orientation = min;
		tab_cible[j].distance = distance;
		tab_cible[j].old_orientation = tab_cible[j].orientation;
		min = TOUR;
	}
}

void return_cible(int32_t compteur, bool target)
{
	if(compteur<TOUR && !target)
	{
	    right_motor_set_speed(VITESSE_SCAN);
	    left_motor_set_speed(-VITESSE_SCAN);
	    if(VL53L0X_get_dist_mm()<DISTANCE_MAX && VL53L0X_get_dist_mm()>0)
	    {
	    if(compteur<(TOUR-100))
	    {
	    		if((abs(compteur-tab_cible[NB_CIBLES-1].orientation)<150))// || (abs(compteur-tab_cible[NB_CIBLES-1].orientation)>1150))
	    		{
	    			if(VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance)
	    			{
	    				tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    				tab_cible[NB_CIBLES-1].orientation = compteur;
	    			}
	    		}
	    		else
	    		{
	    			tri_croissant_distance();
	    			if(VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance)
	    			{
	    				tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    				tab_cible[NB_CIBLES-1].orientation = compteur;
	    				//chprintf((BaseSequentialStream *)&SD3, "distance = %d compteur = %d\n", VL53L0X_get_dist_mm(), compteur);
	    			}
	    		}
	    }
	    else if(compteur>(TOUR-100) && compteur <TOUR)
	    {
	    		//chprintf((BaseSequentialStream *)&SD3, "min orientation = %d\n", min_orientation());
	    		if(min_orientation()<100)										//cible identique autour de 0
	    		{
	    			//chprintf((BaseSequentialStream *)&SD3, "avant distance = %d compteur = %d\n", tab_cible[NB_CIBLES-1].distance, tab_cible[NB_CIBLES-1].orientation);
	    			tri_croissant_orientation();
	    			if(VL53L0X_get_dist_mm()<tab_cible[0].distance)
	    			{
	    				tab_cible[0].distance = VL53L0X_get_dist_mm();
	    				tab_cible[0].orientation = compteur;
	    			}
	    		}
	    		else if(abs(compteur-tab_cible[NB_CIBLES-1].orientation)<150)	//cible identique autour de 1200
	    		{
	    			if(VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance)
	    			{
	    				tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    				tab_cible[NB_CIBLES-1].orientation = compteur;
	    			}
			}
	    		else
	    		{
	    			tri_croissant_distance();
	    			if(VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance)
	    			{
	    				tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    				tab_cible[NB_CIBLES-1].orientation = compteur;
	    			}
	    		}
	    }

	}
	}
	else if(compteur==TOUR)
	{
		right_motor_set_speed(VITESSE_NULLE);
		left_motor_set_speed(VITESSE_NULLE);
		//for(int i=0; i<NB_CIBLES; i++)
		//	chprintf((BaseSequentialStream *)&SD3, "dir orientation = %d distance = %d\n", tab_cible[i].orientation, (tab_cible[i].distance));
	}
}

int32_t min_orientation(void)
{
	int32_t min_orientation = TOUR;
	for(int i=0; i<NB_CIBLES; i++)
	{
		if(tab_cible[i].orientation<min_orientation)
		{
			min_orientation = tab_cible[i].orientation;
		}
	}
	return min_orientation;
}

void direction_cible(uint8_t num_cible, bool target)
{
	if(!target)
		tri_croissant_distance();
	//for(int i=0; i<NB_CIBLES; i++)
	//	chprintf((BaseSequentialStream *)&SD3, "dir orientation = %d distance = %d\n", tab_cible[i].orientation, (tab_cible[i].distance));

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
	chprintf((BaseSequentialStream *)&SD3, "left position = %d \n",left_motor_get_pos());
}

void action_cible(int16_t speed, uint8_t cible)
{
	right_motor_set_pos(POSITION_RESET);
	if(speed>0)
	{
		while(right_motor_get_pos()<(get_step(tab_cible[cible].distance)-310))
		{
			right_motor_set_speed(speed);
			left_motor_set_speed(speed);
		}
	}
	else
	{
		while((-right_motor_get_pos())<(get_step(tab_cible[cible].distance)-310))
		{
			right_motor_set_speed(speed);
			left_motor_set_speed(speed);
		}
	}
	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
}

void ennemy(void)
{
	right_motor_set_pos(0);
	while((-right_motor_get_pos())<TOUR)
	{
		right_motor_set_speed(-VITESSE_STRIKE);
    	   	left_motor_set_speed(VITESSE_STRIKE);
	}
}

uint32_t get_orientation(uint8_t cible)
{
	return tab_cible[cible].old_orientation;
}

void relative_orientation(uint8_t cible, int32_t difference)
{
	if(tab_cible[cible].orientation < difference)
		tab_cible[cible].orientation = (TOUR - abs(tab_cible[cible].orientation - difference));
	else if(tab_cible[cible].orientation > difference)
		tab_cible[cible].orientation = abs(tab_cible[cible].orientation - difference);
}

void correction_orientation(void)
{
	distance_min = DISTANCE_MAX;
	orientation_correction = 0;
	left_motor_set_pos(0);
	while((-left_motor_get_pos())<=(TOUR/5-60))
	{
		right_motor_set_speed(VITESSE_SCAN);
		left_motor_set_speed(-VITESSE_SCAN);
	}
	reset_motor();
	while(left_motor_get_pos()<=(2*TOUR/5-120))
	{
		right_motor_set_speed(-VITESSE_SCAN);
		left_motor_set_speed(VITESSE_SCAN);
		if(VL53L0X_get_dist_mm()<distance_min)
		{
			distance_min = VL53L0X_get_dist_mm();
			orientation_correction = left_motor_get_pos();
			//chprintf((BaseSequentialStream *)&SD3, "distance = %d orientation %d\n", distance_min, orientation_correction);
		}
	}
	reset_motor();
	while((-left_motor_get_pos())<=((2*TOUR/5-120)-orientation_correction))
	{
		right_motor_set_speed(VITESSE_SCAN);
		left_motor_set_speed(-VITESSE_SCAN);
	}
	reset_motor();
	while(left_motor_get_pos() < get_step(distance_min))
	{
		right_motor_set_speed(VITESSE_STANDARD);
		left_motor_set_speed(VITESSE_STANDARD);
	}

	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
}

void retour_scan(void)
{
	right_motor_set_pos(POSITION_RESET);
	while((-right_motor_get_pos())<get_step(distance_min))
	{
		right_motor_set_speed(-VITESSE_STANDARD);
		left_motor_set_speed(-VITESSE_STANDARD);
	}
	//chprintf((BaseSequentialStream *)&SD3, "orientation correction = %d\n", orientation_correction);
	left_motor_set_pos(POSITION_RESET);
	if(orientation_correction<(TOUR/5-60))
	{
		while(left_motor_get_pos()<(TOUR/5-60 - orientation_correction))
		{
			right_motor_set_speed(-VITESSE_SCAN);
			left_motor_set_speed(VITESSE_SCAN);
		}
	}
	else if(orientation_correction>(TOUR/5-60))
	{
		while((-left_motor_get_pos())<(orientation_correction-(TOUR/5-60)))
		{
			right_motor_set_speed(VITESSE_SCAN);
			left_motor_set_speed(-VITESSE_SCAN);
		}
	}
}

uint16_t get_step(uint16_t distance)
{
	return ((distance-20)*STEP_ONE_TURN/WHEEL_PERIMETER);
}

uint8_t nb_cibles(void)
{
	uint8_t nombre = 0;
	for(uint8_t i=0; i<NB_CIBLES; i++)
		if(tab_cible[i].distance < DISTANCE_MAX && tab_cible[i].orientation < TOUR)
			nombre++;
	return nombre;
}

void mvt_robot(int speed_right, int speed_left, int32_t compare_pos, int32_t comparant)
{
	while(compare_pos <= comparant)
	{
		right_motor_set_speed(speed_right);
		left_motor_set_speed(speed_left);
	}
}

void reset_motor(void)
{
	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
	right_motor_set_pos(POSITION_RESET);
	left_motor_set_pos(POSITION_RESET);
}
