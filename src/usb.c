#include <asf.h>
#include "usb.h"
#include "debug.h"
#include "buffer.h"


// USB-Anschlüsse
//	Datenbus	Port A - alle Pins
//	RD#			Port B - Pin 0			(Steuert den Lesezugriff auf die USB-Schnittstelle)
//	WR			Port B - Pin 1			(Steuert den Schreibzugriff auf die USB-Schnittstelle)
//	RXF#		Port B - Pin 2			(Zeigt an, dass sich Daten im Empfangspuffer der USB-Schnittstelle befinden)
//	TXE#		Port B - Pin 3			(Zeigt an, dass Daten in den Sendepuffer der USB-Schnittstelle geschrieben werden können)
//	PWREN#		Port R - Pin 1			(Zeigt an, dass die USB-Schnittstelle betriebsbereit ist)
#define USB_RD_bm			PIN0_bm
#define USB_RD_activate		(PORTB_OUT &= ~USB_RD_bm)
#define USB_RD_deactivate	(PORTB_OUT |= USB_RD_bm)
#define USB_WR_bm			PIN1_bm
#define USB_WR_activate		(PORTB_OUT |= USB_WR_bm)
#define USB_WR_deactivate	(PORTB_OUT &= ~USB_WR_bm)
#define USB_RXF_bm			PIN2_bm
#define USB_TXE_bm			PIN3_bm
#define USB_TXE_isActive	!(PORTB_IN & USB_TXE_bm)
#define USB_PWREN_bm		PIN1_bm
#define USB_PWREN_isActive	!(PORTR_IN & USB_PWREN_bm)

DECLARE_BUFFER(UsbTransmit,unsigned char,USB_TRANSMIT_BUFFER_LENGTH)


// Initialisiert die für USB-Übertragungen nötigen Ports und Pins
void usb_init(void)
{
	USB_RD_deactivate;							// Standardpegel der Ausgabepins festlegen
	USB_WR_deactivate;
	PORTB_DIR |= USB_RD_bm | USB_WR_bm;			// Pin 0 und 1 von Port B als Ausgabe definieren (USB RD, USB WR)
	PORTB_DIR &= ~(USB_RXF_bm | USB_TXE_bm);	// Pin 2 und 3 von Port B als Eingabe definieren (USB RXF, USB TXE)
	PORTR_DIR &= ~USB_PWREN_bm;					// Pin 1 von Port R als Eingabe definieren (USB PWREN)

	// Interrupt 0 zum Lesen von Daten von der USB-Schnittstelle benutzen
	PORTB_INT0MASK = USB_RXF_bm;
	// Interrupt-Level festlegen
	//	Lese-Interrupt:		niedriges Level
	PORTB_INTCTRL |= PORT_INT0LVL_LO_gc;
}


/**
 * Reiht das angegebene Byte in die Warteschlange zum Versenden an die USB-Schnittstelle ein
 */
void usb_send(unsigned char data)
{
	if (BUFFER_CANWRITE(UsbTransmit))	
		BUFFER_WRITE(UsbTransmit,data);
}


/**
 * Arbeitet die Warteschlange ab und sendet die enthaltenenen Bytes an die USB-Schnittstelle
 */
void usb_process_queue()
{
	if (USB_PWREN_isActive)	// USB-Schnittstelle ist betriebsbereit
	if (USB_TXE_isActive)	// es können Daten geschrieben werden
	if (BUFFER_CANREAD(UsbTransmit)) // Sind noch Daten im Schreibpuffer?
	{
		DBG_OUT(PIN1_bm,0xff);
		
		// Daten ausgeben
		PORTA_DIR = 0xFF;						// Port A als Ausgabe definieren (USB DATA)
		USB_WR_activate;						// WR aktivieren
		PORTA_OUT = BUFFER_READ(UsbTransmit);	// Daten aktivieren
		USB_WR_deactivate;						// WR deaktivieren
		PORTA_DIR = 0x00;						// Port A als Eingabe definieren

		BUFFER_MOVENEXT(UsbTransmit)			// nächtes Element im Schreibpuffer anspringen
		
		DBG_OUT(PIN1_bm,0x00);
	}
}


// Interrupt Service Routine zum Lesen von Daten von der USB-Schnittstelle
ISR (PORTB_INT0_vect)
{
}