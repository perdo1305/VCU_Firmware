
/* 
 * File:   APPS.h
 * Author: pedro
 *
 * Created on 24 de Fevereiro de 2024, 20:29
 */


#ifndef APPS_H
#define	APPS_H

#include <stdint.h>
#include <stdbool.h>  // Defines true

extern float APPS_MIN_Volts;
extern uint16_t APPS_MIN_bits;
extern float APPS_MAX_Volts;
extern uint16_t APPS_MAX_bits;
extern float APPS_Tolerance_Volts;
extern uint16_t APPS_Tolerance_bits;
extern uint16_t APPS_Bit_Resolution;
extern float APPS_Voltage;
extern uint16_t APPS1;
extern uint16_t APPS2;
extern uint16_t APPS_Mean;
extern uint16_t APPS_Percentage;
extern uint16_t APPS_Percentage_1000;
extern uint16_t APPS_functional_region;
extern bool APPS_Error;


void APPS_Init(float min_volts, float max_volts, float APPS_Tolerance_Volts, uint16_t APPS_Delta);
bool APPS_Function(uint16_t apps1, uint16_t apps2);
bool APPS_TimedOut(uint16_t apps1, uint16_t apps2);
void APPS_PrintValues(void);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* APPS_H */

