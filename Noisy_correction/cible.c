/*
 * cible.c
 *
 *  Created on: Apr 13, 2020
 *      Author: manteauxthomas
 *
 *  Stockage des cibles et mouvements du robot pour les atteindre
 */

#include "ch.h"
#include "hal.h"
#include <chprintf.h>

#include <main.h>
#include "cible.h"
#include "motors.h"
#include "sensors/VL53L0X/VL53L0X.h"
#include <math.h>

static etat_cible tab_cible[NB_CIBLES] = {0};

static uint16_t distance_min = DISTANCE_MAX;
static int32_t orientation_correction = 0;

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

/*! Remplit le tableau de structures tab_cible[] en comparant les valeurs courantes renvoyées
 * par le ToF et la valeur du compteur avec les autres cases du tableau.
 * Si le nombre de cibles est supérieur à NB_CIBLES, alors on ne stocke que les plus
 * proches dans le tableau.
 *
 * \param: compteur : indique le nombre de pas du moteur. Paramètre indiquant l'orientation d'une cible.
 * 		   target: booleen valant 1 si on se dirige vers une cible, 0 sinon
 */
void return_cible(int32_t compteur, bool target)
{
	if(compteur<TOUR && !target)
	{
	    right_motor_set_speed(VITESSE_SCAN);
	    left_motor_set_speed(-VITESSE_SCAN);
	    if(VL53L0X_get_dist_mm()<DISTANCE_MAX && VL53L0X_get_dist_mm()>0)
	    {
	    		//si cible se trouve à des orientations <1150
	    		if(compteur<(TOUR-REBOUCLEMENT))
	    		{
	    			//test pour voir si on scanne la même cible que précedemment (ecart d'orientation<150 pas => même cible)...
	    			if((abs(compteur-tab_cible[NB_CIBLES-1].orientation)<ECART_CIBLE))
	    			{
	    				//si oui mais que la distance est plus faible à cette orientation...
	    				if(VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance)
	    				{
	    					//...on remplace
	    					tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    					tab_cible[NB_CIBLES-1].orientation = compteur;
	    				}
	    			}
	    			//si il ne s'agit pas de la même cible...
	    			else
	    			{
	    				tri_croissant_distance();
	    				//...on ecrase la case du tableau contenant la plus grande distance
	    				if(VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance)
	    				{
	    					tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    					tab_cible[NB_CIBLES-1].orientation = compteur;
	    				}
	    			}
	    		}
	    		//si cible scannée se trouve à des orientations >1150...
	    		else if(compteur>(TOUR-REBOUCLEMENT) && compteur<TOUR)
	    		{
	    			//...et si une cible autour de 0 pas (cible d'orientation min_orientation<150 stockée dans tableau)
	    			if(min_orientation()<REBOUCLEMENT)
	    			{
	    				//alors il s'agit de la même cible déjà présente autour de 0
	    				tri_croissant_orientation();
	    				//si la distance est plus faible à cette orientation: on remplace
	    				if(VL53L0X_get_dist_mm()<tab_cible[0].distance)
	    				{
	    					tab_cible[0].distance = VL53L0X_get_dist_mm();
	    					tab_cible[0].orientation = compteur;
	    				}
	    			}
	    			//si pas de cible autour de 0 pas MAIS cible un peu avant 1150 (cible identique autour de 1150)
	    			else if(abs(compteur-tab_cible[NB_CIBLES-1].orientation)<ECART_CIBLE)	//cible identique autour de 1200
	    			{
	    				if(VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance)
	    				{
	    					tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    					tab_cible[NB_CIBLES-1].orientation = compteur;
	    				}
	    			}
	    			//si pas de cible autour de 0 NI un peu avant 1150...
	    			else
	    			{
	    				//...alors c'est une nouvelle cible
	    				tri_croissant_distance();
	    				//on ecrase la case du tableau contenant la plus grande distance
	    				if(VL53L0X_get_dist_mm()<tab_cible[NB_CIBLES-1].distance)
	    				{
	    					tab_cible[NB_CIBLES-1].distance = VL53L0X_get_dist_mm();
	    					tab_cible[NB_CIBLES-1].orientation = compteur;
	    				}
	    			}
	    		}
	    }
	}
	//fin du scan
	else if(compteur==TOUR)
	{
		right_motor_set_speed(VITESSE_NULLE);
		left_motor_set_speed(VITESSE_NULLE);
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

/*! Oriente le robot vers la cible stockée à la case "num_cible" du tableau.
 *
 * \param: num_cible (indice du tableau tab_cible): indique le numéro de la cible actuelle
 * 		   target: booleen valant 1 si on se dirige vers une cible, 0 sinon
 */
void direction_cible(uint8_t num_cible, bool target)
{
	if(!target)
		tri_croissant_distance();

	left_motor_set_pos(POSITION_RESET);
	//si la cible actuelle est dans demi cercle à droite du robot
	if(tab_cible[num_cible].orientation >= (TOUR/2))
		mvt_robot((-VITESSE_SCAN), VITESSE_SCAN, (TOUR-tab_cible[num_cible].orientation));
	//si la cible actuelle est dans demi cercle à gauche du robot
	else
		mvt_robot(VITESSE_SCAN, (-VITESSE_SCAN), tab_cible[num_cible].orientation);

	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
}

/*! Avance/recule le robot jusqu'à 70% de la distance qui le sépare de la cible courante (RATIO_STEP = 0.7)
 * N'avance pas directement jusqu'à 100% de la distance car fait une correction de l'orientation
 * avant de se diriger définitivement vers la cible.
 *
 * \param: speed: vitesse des moteurs (positive = avancer; negative = reculer)
 * 		   cible (indice du tableau tab_cible): indique le numéro de la cible actuelle
 */
void action_cible(int16_t speed, uint8_t cible)
{
	left_motor_set_pos(POSITION_RESET);
	float step = (RATIO_STEP*get_step(tab_cible[cible].distance));
	mvt_robot(speed, speed, (int32_t)(step));

	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
}

void ennemy(void)
{
	left_motor_set_pos(POSITION_RESET);
	mvt_robot((-VITESSE_STRIKE), VITESSE_STRIKE, TOUR);
}

uint32_t get_orientation(uint8_t cible)
{
	return tab_cible[cible].old_orientation;
}

/*! Renvoie l'orientation relative entre l'orientation actuelle du robot et celle de la prochaine cible
 *
 * \param: cible (indice du tableau tab_cible): indique le numéro de la cible actuelle
 * 		   difference: orientation de la cible qui vient d'être traitée
 * 		   (qui est donc aussi l'orientation actuelle du robot).
 */
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

	left_motor_set_pos(POSITION_RESET);

	//s'oriente de 45° vers la gauche
	mvt_robot(VITESSE_SCAN, (-VITESSE_SCAN), (FENETRE_SCAN));

	reset_motor();
	//revient de 90° vers la droite et mémorise la distance minimale ainsi que l'orientation associée
	while(left_motor_get_pos()<=(2*FENETRE_SCAN))
	{
		right_motor_set_speed(-VITESSE_SCAN);
		left_motor_set_speed(VITESSE_SCAN);
		if(VL53L0X_get_dist_mm()<distance_min)
		{
			distance_min = VL53L0X_get_dist_mm();
			orientation_correction = left_motor_get_pos();
		}
	}

	reset_motor();
	//s'oriente là ou il avait mesuré la distance minimale
	mvt_robot(VITESSE_SCAN, (-VITESSE_SCAN), ((2*FENETRE_SCAN)-orientation_correction));

	//on éteint la front_led avant de prendre l'image pour ne pas perturber la mesure de la couleur
	palClearPad(GPIOD, GPIOD_LED_FRONT);

	reset_motor();
	//avance vers la cible
	mvt_robot(VITESSE_STANDARD, VITESSE_STANDARD, get_step(distance_min));

	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
}

/*! Après avoir lu la couleur, retourne à la position d'où il s'était réorienté.
 */
void retour_scan(void)
{
	left_motor_set_pos(POSITION_RESET);
	mvt_robot((-VITESSE_STANDARD),(-VITESSE_STANDARD), get_step(distance_min));

	left_motor_set_pos(POSITION_RESET);
	if(orientation_correction<(FENETRE_SCAN))
		mvt_robot((-VITESSE_SCAN), VITESSE_SCAN, (FENETRE_SCAN - orientation_correction));
	else if(orientation_correction>(FENETRE_SCAN))
		mvt_robot(VITESSE_SCAN, (-VITESSE_SCAN), (orientation_correction-(FENETRE_SCAN)));
}

uint16_t get_step(uint16_t distance)
{
	//convertit la distance en nombre de pas moteur
	return ((distance-MARGE)*STEP_ONE_TURN/WHEEL_PERIMETER);
}

uint8_t nb_cibles(void)
{
	uint8_t nombre = 0;
	//compte le nombre de cibles valides dans le tableau
	for(uint8_t i=0; i<NB_CIBLES; i++)
		if(tab_cible[i].distance < DISTANCE_MAX && tab_cible[i].orientation < TOUR)
			nombre++;
	return nombre;
}

void mvt_robot(int speed_right, int speed_left, int32_t comparant)
{
	if(speed_left>0)
	{
		while(left_motor_get_pos() <= comparant)
		{
			right_motor_set_speed(speed_right);
			left_motor_set_speed(speed_left);
		}
	}
	if(speed_left<0)
	{
		while((-left_motor_get_pos()) <= comparant)
		{
			right_motor_set_speed(speed_right);
			left_motor_set_speed(speed_left);
		}
	}
}

void reset_motor(void)
{
	right_motor_set_speed(VITESSE_NULLE);
	left_motor_set_speed(VITESSE_NULLE);
	right_motor_set_pos(POSITION_RESET);
	left_motor_set_pos(POSITION_RESET);
}
