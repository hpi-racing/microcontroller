#include <asf.h>
#include "interrupts.h"

void interrupts_enable (void)
{	
	// Interruptlevel High, Medium und Low freigeben
	PMIC_CTRL |= PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	
	// globale Interrupts aktivieren
	sei ();
}
