/*
 * cible.h
 *
 *  Created on: Apr 13, 2020
 *      Author: manteauxthomas
 */

#ifndef CIBLE_H_
#define CIBLE_H_

#define DISTANCE_MAX		400 //en mm (au max une cible peut se trouver à 1m de distance)
#define TOUR				1300
#define NB_CIBLES		5
#define CONSIGNE			22

typedef struct {
	int32_t orientation;
	uint16_t distance;
    int32_t old_orientation;
} etat_cible;

void init_tab_cible(void);
void tri_croissant(void);
void return_cible(int32_t compteur, bool target);
void direction_cible(uint8_t num_cible, bool target);
void action_cible(int16_t speed, uint8_t cible);
int16_t pi_regulator(void);
void ennemy(void);
void reset_motor(void);
uint16_t get_step(uint16_t distance);
uint32_t get_orientation(uint8_t cible);
void relative_orientation(uint8_t cible, int32_t difference);
uint8_t nb_cibles(void);

#endif /* CIBLE_H_ */
