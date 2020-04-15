/*
 * cible.c
 *
 *  Created on: 15 avr. 2020
 *      Author: marti
 */
#include "ch.h"
#include "hal.h"
#include <math.h>
#include <control_motor.c>
#include "cible.h"

//static uint16_t mesure = DISTANCE_MAX;
static uint8_t indice = 9;
static etat_cible tab_cible[NB_CIBLES] = {0};

void init_tab_cible(void)
{
	for(int i=0; i<NB_CIBLES; i++)
		tab_cible[i].distance=DISTANCE_MAX;
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
	    	right_motor_set_speed(150);
	    	left_motor_set_speed(-150);

	    	if(VL53L0X_get_dist_mm() < DISTANCE_MAX && VL53L0X_get_dist_mm() > 0)
	    	{
	    		if(indice>0)
	    		{
	    			if(indice==9)
	    			{
	    				if(abs(tab_cible[0].distance-VL53L0X_get_dist_mm())>10 && (compteur-tab_cible[0].orientation)>100)
	    				{
	    					tab_cible[indice].distance = VL53L0X_get_dist_mm();
	    					tab_cible[indice].orientation = compteur;
	    					indice--;
	    				}
	    			}
	    			else
	    			{
	    				if(abs(tab_cible[indice+1].distance-VL53L0X_get_dist_mm())>10 && (compteur-tab_cible[indice+1].orientation)>100)
	    				{
	    					tab_cible[indice].distance = VL53L0X_get_dist_mm();
	    					tab_cible[indice].orientation = compteur;
	    					indice--;
	    				}
	    			}
	    		}
	    	    if(indice==0)
	    		{
	    	    	if(abs(tab_cible[indice+1].distance-VL53L0X_get_dist_mm())>10 && (compteur-tab_cible[indice+1].orientation)>100)
	    	    	{
	    	    		tri_croissant(); //pour qu'on écrase seulement les cibles les plus loin si il y a plus de 10 cibles
	    	    		tab_cible[indice].distance = VL53L0X_get_dist_mm();
	    	    		tab_cible[indice].orientation = compteur;
	    	    		indice=9;
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
	tri_croissant(); //ordonne tableau dans ordre croissant
	left_motor_set_pos(0);
	if(tab_cible[num_cible].orientation >= (TOUR/2)){
		while(left_motor_get_pos()<=(TOUR-tab_cible[num_cible].orientation)){
			right_motor_set_speed(-150);
			left_motor_set_speed(150);
		}
	}
	else{
		while((-left_motor_get_pos())<=tab_cible[num_cible].orientation){
			right_motor_set_speed(150);
			left_motor_set_speed(-150);
		}
	}
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}



