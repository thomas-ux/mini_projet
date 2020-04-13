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

#include "sensors/VL53L0X/VL53L0X.h"

#include <arm_math.h>

#define DISTANCE_MAX = 8190


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

static void timer12_start(void){
    //General Purpose Timer configuration   
    //timer 12 is a 16 bit timer so we can measure time
    //to about 65ms with a 1Mhz counter
    static const GPTConfig gpt12cfg = {
        1000000,        /* 1MHz timer clock in order to measure uS.*/
        NULL,           /* Timer callback.*/
        0,
        0
    };

    gptStart(&GPTD12, &gpt12cfg);
    //let the timer count to max value
    gptStartContinuous(&GPTD12, 0xFFFF);
}

int32_t return_cible(int32_t compteur);
static uint16_t mesure = DISTANCE_MAX;




int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    //lire valeurs du Time of Flight
    VL53L0X_start();

    int16_t mesure = DISTANCE_MAX;
    int32_t compteur = 0;
    int32_t cible = 0;

    while(1){
    	compteur = right_moteur_get_pos();
    	cible = return_cible(compteur);

    	chprintf((BaseSequentialStream *)&SD3, "mesure = %d; cible = %d\n", mesure, cible);
    }

}


int32_t return_cible(int32_t compteur)
{
	int32_t cible = 0;
	//1300 = nb de steps à faire pour faire un tour sur soi-même
	if(compteur<1300){
		right_motor_set_speed(150);
		left_motor_set_speed(-150);

		if(VL53L0X_get_dist_mm() < mesure && VL53L0X_get_dist_mm() > 0){
			mesure = VL53L0X_get_dist_mm();
			cible=compteur;
		}
	}
	else if(compteur == 1300){
		right_motor_set_speed(0);
		left_motor_set_speed(0);
	}
	return cible;
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
