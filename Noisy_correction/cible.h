/*
 * cible.h
 *
 *  Created on: Apr 13, 2020
 *      Author: manteauxthomas
 */

#ifndef CIBLE_H_
#define CIBLE_H_

#define DISTANCE_MAX		1000 //en mm (au max une cible peut se trouver à 1m de distance)
#define TOUR				1300
#define NB_CIBLES			10

typedef struct {
	int32_t orientation;
	uint16_t distance;
    bool couleur;
} etat_cible;

void init_tab_cible(void);
void tri_croissant(void);
void return_cible(int32_t compteur, bool target);
void direction_cible(uint8_t num_cible);
void go_no_go(int16_t speed, uint8_t num_cible);



#endif /* CIBLE_H_ */
