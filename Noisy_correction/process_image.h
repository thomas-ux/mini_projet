/*
 * process_image.h
 *
 *  Created on: Apr 13, 2020
 *      Author: manteauxthomas
 *
 *  Capture de l'image et analyse de la couleur
 */

#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

// appelle les fonctions pour prendre une image et extraire les pixels
void capture_image(void);

// extrait les pixels de l'image et remplit la tableau image[] de taille IMAGE_BUFFER_SIZE
void image_process(void);

/*! \brief Analyse les pixels de l'image capturée. Si le nombre de pixels blancs
 *  (intensité > SEUIL_BLANC) est supérieur à NB_BLANC alors on considère que la cible blanche,
 *  sinon on la considère noire.
 *  \param couleur : spécifie quelle couleur on doit attaquer. Couleur vaut 1 si c'est les cibles
 *  blanches, 0 si c'est les cibles noires
 */
bool get_action(bool couleur);


#endif /* PROCESS_IMAGE_H */
