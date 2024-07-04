/* 
 * File:   TorqueControl.h
 * Author: pedro
 *
 * Created on 1 de Julho de 2024, 22:44
 */


#ifndef TORQUECONTROL_H
#define	TORQUECONTROL_H


float ConvertAPPSToTorque(float APPS);
float ConvertTorqueToCurrent(float torque);

typedef struct {
    /*SetPoint*/
    float APPS;  // Accelerator Pedal Position Sensor

    /*Current*/
    float TargetCurrent;  // used for control
    float ActualCurrent;  // used for feedback

    /*Torque*/
    float ContinuosTorque;
    float PeakTorque;

    /*KT constant [Nm/ARMS]*/
    float Kt;

} TorqueControl;



#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* TORQUECONTROL_H */

