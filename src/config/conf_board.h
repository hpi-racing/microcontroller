#ifndef CONF_BOARD_H
#define CONF_BOARD_H

/**********************************
 * Definitionen für das Debugging *
 **********************************/
// __DEBUG_BOARD__ spezifiziert, dass die Leiterplatte über einen Debug-Port verfügt
// Ist dem nicht so, muss die folgende Zeile auskommentiert werden!
#define __DEBUG_BOARD__


/******************************************
 * Kennungen der angeschlossenen Sensoren *
 ******************************************/
// Schienensensor
#define LANE_SONSOR_ID 0x60


/**********************
 * Pufferdefinitionen *
 **********************/
// Länge des Schreibpuffers der USB-Schnittstelle
#define USB_TRANSMIT_BUFFER_LENGTH		64
// Länge des Datenpuffers des Schienensensors
#define LANE_BUFFER_LENGTH				16


#endif // CONF_BOARD_H
