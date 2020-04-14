#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

void capture_image(void);
void image_process(void);
uint16_t extract_line_width(uint8_t *buffer);
uint16_t get_action(void);


#endif /* PROCESS_IMAGE_H */
