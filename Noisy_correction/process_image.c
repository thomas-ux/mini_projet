#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <process_image.h>

static uint8_t image[IMAGE_BUFFER_SIZE] = {0};

void capture_image(void)
{

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
    po8030_advanced_config(FORMAT_RGB565, 0, 10, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
    	dcmi_enable_double_buffering();
    	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
    	dcmi_prepare();

    //starts a capture
	dcmi_capture_start();
	//waits for the capture to be done
	wait_image_ready();
	image_process();
	//for(uint16_t i = (2*300) ; i < (2 * 500) ; i+=2)
	//	chprintf((BaseSequentialStream *)&SD3, "i = %d intensity = %d\n", (i/2), image[i/2]);
}

void image_process(void)
{

	uint8_t *img_buff_ptr;
	for(uint16_t k=0; k<IMAGE_BUFFER_SIZE; k++)
		image[k] = 0;

	bool send_to_computer = true;

		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
		{
			//extracts first 5bits of the first byte
			//takes nothing from the second byte
			image[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;
		}

		if(send_to_computer){
			//sends to the computer the image
			//SendUint8ToComputer(image, IMAGE_BUFFER_SIZE);
		}
		//invert the bool
		send_to_computer = !send_to_computer;
}

uint16_t get_action(void)
{
	uint16_t noir = 0, blanc = 0;
	for(uint16_t i=(2*300); i<(2*500); i+=2)
	{
		if(image[i/2]<32)
		{
			noir++;
			//chprintf((BaseSequentialStream *)&SDU1, "compteur = %d intensity = %d\n", compteur, image[i/2]);
		}
		if(image[i/2]>=40)
			blanc++;
	}
	if(noir>blanc)
		return 1;
	else
		return 0;
}
