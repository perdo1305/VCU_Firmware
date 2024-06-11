#include "TorqueControl.h"


void InitTorqueControl(TorqueControl *torqueControl){
    torqueControl->APPS = 0f;
    torqueControl->TargetCurrent = 0f;
    torqueControl->ActualCurrent = 0f;
    torqueControl->ContinuosTorque = 0f;
    torqueControl->PeakTorque = 0f;
    torqueControl->Kt = 0f;
}

float ConvertAPPSToTorque(float APPS){
    static uint8_t APPSrange = 100;
    return APPS * (torqueControl.ContinuosTorque)/APPSrange;
}

float ConvertTorqueToCurrent(float torque){
    return torque/torqueControl.Kt;
}