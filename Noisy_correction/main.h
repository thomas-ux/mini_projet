#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"


//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE		640
#define FENETRE_MIN				300
#define FENETRE_MAX				500
#define VITESSE_NULLE			0
#define VITESSE_STRIKE			1000
#define VITESSE_SCAN			300
#define POSITION_RESET			0
#define WHEEL_PERIMETER			130
#define STEP_ONE_TURN			1000
#define VITESSE_STANDARD		600

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
