#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>

#include <arm_math.h>
#include "sensors/VL53L0X/VL53L0X.h"
#include "process_image.h"

#define DISTANCE_MAX		1000 //en mm (au max une cible peut se trouver à 1m de distance)
#define TOUR				1300
#define NB_CIBLES			10

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

void SendUint8ToComputer(uint8_t* data, uint16_t size)
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}


void init_tab_cible(void);
void tri_croissant(void);
void return_cible(int32_t compteur, bool target);
void direction_cible(uint8_t num_cible);


//static uint16_t mesure = DISTANCE_MAX;
static uint8_t indice = 0;
static etat_cible tab_cible[NB_CIBLES] = {0};

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();
    //starts the USB communication
    usb_start();
    //inits the motors
    motors_init();

    VL53L0X_start();
    dcmi_start();
    po8030_start();
    process_image_start();

    int32_t compteur = 0;
    uint8_t num_cible = 0;
    bool target = 0; //indique si on a une cible acquise ou non
    init_tab_cible();

    while(1)
    {
    	//partie pour la cible
    	compteur = right_motor_get_pos();


    	return_cible(compteur, target);
    	//chprintf((BaseSequentialStream *)&SD3, "mesure = %d mm cible %d = \n", mesure, cible);

    	if(compteur==TOUR)
    	{
    		target=1;
    		direction_cible(num_cible);
    	}




    	//partie pour la caméra
    	if(get_action())
    	{
    		while(right_motor_get_pos()<650)
    		{
    			right_motor_set_speed(400);
    			left_motor_set_speed(-400);
    		}
    	}
    	else
    	{
    		right_motor_set_speed(0);
    		left_motor_set_speed(0);
    	}
    	   chThdSleepMilliseconds(1000);
    }
}

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
	    		tab_cible[indice].distance = VL53L0X_get_dist_mm();
	    		tab_cible[indice].orientation = compteur;
	    		if(indice>0)
	    			indice--;
	    		if(indice==0)
	    		{
	    			tri_croissant(); //pour qu'on écrase seulement les cibles les plus loin si il y a plus de 10 cibles
	    			indice=9;
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

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
