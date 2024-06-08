/* 
 * File:   TorqueControl.h
 * Author: pedro
 *
 * Created on 8 de Junho de 2024, 13:58
 */

#include <stdbool.h>  
#include <stddef.h> 
#include <stdint.h>
#include <stdlib.h>  

#ifndef TORQUECONTROL_H
#define	TORQUECONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    /*SetPoint*/
    float APPS; //Accelerator Pedal Position Sensor

    /*Current*/
    float TargetCurrent; //used for control
    float ActualCurrent; //used for feedback

    /*Torque*/
    float ContinuosTorque;
    float PeakTorque;

    /*KT constant [Nm/ARMS]*/
    float Kt;


} TorqueControl;
TorqueControl torqueControl;


#ifdef	__cplusplus
}
#endif

#endif	/* TORQUECONTROL_H */

