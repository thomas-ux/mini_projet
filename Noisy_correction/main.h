#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"


//constants for the differents parts of the project

#define DISTANCE_MAX				300 //en mm (au max une cible peut se trouver à 1m de distance)
#define TOUR						1300
#define NB_CIBLES				5
#define VITESSE_NULLE			0
#define VITESSE_STRIKE			1000
#define VITESSE_SCAN				300
#define VITESSE_STANDARD			600
#define POSITION_RESET			0
#define WHEEL_PERIMETER			130
#define STEP_ONE_TURN			1000
#define RATIO_STEP				0.7f
#define FENETRE_SCAN				(TOUR/5-100)
#define REBOUCLEMENT				100
#define ECART_CIBLE				150

#define IMAGE_BUFFER_SIZE		640
#define FENETRE_MIN				0
#define FENETRE_MAX				640
#define SEUIL_BLANC				200



/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
