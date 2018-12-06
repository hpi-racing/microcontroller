#ifndef CONF_BOARD_H
#define CONF_BOARD_H

/**********************************
 * Definitionen f�r das Debugging *
 **********************************/
// __DEBUG_BOARD__ spezifiziert, dass die Leiterplatte �ber einen Debug-Port verf�gt
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
// L�nge des Schreibpuffers der USB-Schnittstelle
#define USB_TRANSMIT_BUFFER_LENGTH		64
// L�nge des Datenpuffers des Schienensensors
#define LANE_BUFFER_LENGTH				16


#endif // CONF_BOARD_H
