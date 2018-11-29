#ifndef USB_H_
#define USB_H_

// öffentliche Funktionen
void usb_init(void);
void usb_send(unsigned char);
void usb_process_queue(void);

#endif
