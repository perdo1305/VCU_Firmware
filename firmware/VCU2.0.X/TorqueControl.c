#include "TorqueControl.h"


void InitTorqueControl(float peakTorque, float continuosTorque, float targetCurrent){
    
    torqueControl.PeakTorque = peakTorque;
    torqueControl.ContinuosTorque = continuosTorque;
    torqueControl.Kt = torqueControl.ContinuosTorque/torqueControl.TargetCurrent;
    torqueControl.TargetCurrent = targetCurrent;
}

float ConvertAPPSToTorque(float APPS){
    static uint8_t APPSrange = 100;
    return APPS * (torqueControl.ContinuosTorque)/APPSrange;
}

float ConvertTorqueToCurrent(float torque){
    return torque/torqueControl.Kt;
}