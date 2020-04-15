/*
 * cible.h
 *
 *  Created on: 15 avr. 2020
 *      Author: marti
 */

#ifndef CIBLE_H_
#define CIBLE_H_

#define DISTANCE_MAX		1000 //en mm (au max une cible peut se trouver à 1m de distance)
#define TOUR				1300
#define NB_CIBLES			10

void init_tab_cible(void);
void tri_croissant(void);
void return_cible(int32_t compteur, bool target);
void direction_cible(uint8_t num_cible);

#endif /* CIBLE_H_ */
