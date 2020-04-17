/*
 * cible.h
 *
 *  Created on: Apr 13, 2020
 *      Author: manteauxthomas
 */

#ifndef CIBLE_H_
#define CIBLE_H_

#define DISTANCE_MAX		300 //en mm (au max une cible peut se trouver à 1m de distance)
#define TOUR				1300
#define NB_CIBLES			5
#define CONSIGNE			22

typedef struct {
	int32_t orientation;
	uint16_t distance;
    bool couleur;
} etat_cible;

void init_tab_cible(void);
void tri_croissant(void);
void return_cible(int32_t compteur, bool target);
void direction_cible(uint8_t num_cible);
void action_cible(int16_t speed, uint8_t cible);
uint16_t get_step(uint16_t distance);
int16_t pi_regulator(void);
void friend(int16_t speed, uint8_t num_cible);
void ennemy(void);
void reset_motor(void);

#endif /* CIBLE_H_ */
