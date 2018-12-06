#ifndef USB_H_
#define USB_H_

#include <asf.h>
#include "packet_type.h"

void usb_init(void);
void usb_process_queue(void);

void usb_send_packet(packet_type_t type,  uint8_t* payload, uint8_t payload_length);

#endif /* USB_H_ */
