#ifndef DEBUG_H_
#define DEBUG_H_

#include <asf.h>


#ifdef __DEBUG_BOARD__
#define DBG_OUT(Mask, Value) {PORTB_OUT = (PORTB_OUT & (~(Mask) << 4)) | (((Value) & (Mask)) << 4);}
#else
#define DBG_OUT(Mask, Value)
#endif // __DEBUG_BOARD__


// öffentliche Funktionen
void debug_init (void);

#endif /* DEBUG_H_ */