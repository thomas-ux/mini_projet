/*
 * cible.h
 *
 *  Created on: Apr 13, 2020
 *      Author: manteauxthomas
 */

#ifndef CIBLE_H_
#define CIBLE_H_

typedef struct {
	int32_t orientation;
	uint16_t distance;
    int32_t old_orientation; // contient toujours l'orientation mesurée lors du 1er scan par le robot
} etat_cible;

void init_tab_cible(void);
void tri_croissant_distance(void);
void tri_croissant_orientation(void);

/*! \brief Remplit le tableau 'tab_cible' des 5 (ou moins) cibles les plus proches
 *  \param compteur : nombre de step effectuées par le moteur
 *  \param target : booléen qui vaut 1 si on se dirige vers une cible, 0 sinon
 */
void return_cible(int32_t compteur, bool target);

/*! \brief Le robot s'oriente vers la cible de la manière le plus rapide possible : tourne vers
 *  la droite si la cible est sur sa droite, tourne vers la gauche sinon.
 *  \param num_cible : indice de la cible courante
 *  \param target : booléen qui vaut 1 si on se dirige vers une cible, 0 sinon
 */
void direction_cible(uint8_t num_cible, bool target);

/*! \brief Calcule le nombre de step à faire pour parcourir 70% de la distance robot-cible et
 *  avance/recule de cette distance.
 *  \param speed : vitesse à laquelle il faut avancer/reculer
 *  \param cible : cible visée
 */
void action_cible(int16_t speed, uint8_t cible);

//tourne sur lui-même pour taper la cible
void ennemy(void);

void reset_motor(void);
// retourne le nombre de step à parcourir en connaissant la distance. On prend une MARGE de 15mm
// pour éviter de percuter les cibles.
uint16_t get_step(uint16_t distance);

// retourne la plus petite orientation du tableau
int32_t min_orientation(void);

// retourne l'orientation absolue de la cible
uint32_t get_orientation(uint8_t cible);

// le robot re-scan de plus ou moins 45° pour corriger une éventuelle erreur d'alignement avec la
// cible
void correction_orientation(void);

/*! \brief Calcule l'orientation relative de la cible suivante par rapport à la cible précédente
 *  \param cible : prochaine cible à aller voir
 *  \param difference : orientation absolue de la cible précédente
 */
void relative_orientation(uint8_t cible, int32_t difference);

// retourne le nombre de cibles vues par le robot
uint8_t nb_cibles(void);

/*! \brief Fait avancer/reculer/tourner le robot
 *  \param speed_right : vitesse de la roue droite
 *  \param speed_left : vitesse de la roue gauche
 *  \param comparant : condition qui fixe le nombre de pas à atteindre
 */
void mvt_robot(int speed_right, int speed_left, int32_t comparant);

// recule de la cible jusqu'à l'endroit où il s'est réorienté
void retour_scan(void);

#endif /* CIBLE_H_ */
