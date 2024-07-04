#include "TorqueControl.h"
#include "VCU_config.h"
#include <stdbool.h>  // Defines true
#include <stddef.h>   // Defines NULL
#include <stdlib.h>   // Defines EXIT_FAILURE

TorqueControl InitTorqueControl(float APPS,float TargetCurrent,float ActualCurrent,float ContinuosTorque,float PeakTorque,float Kt) {
    TorqueControl torqueControl;

    torqueControl.APPS = APPS;

    torqueControl.TargetCurrent = TargetCurrent;
    torqueControl.ActualCurrent = ActualCurrent;

    torqueControl.ContinuosTorque = ContinuosTorque;
    torqueControl.PeakTorque = PeakTorque;

    torqueControl.Kt = Kt;

    return torqueControl;
}

float ConvertAPPSToTorque(float APPS) {
    const int APPSrange = 1000;

    float torque = APPS * (MAX_TORQUE) / APPSrange;

    if (torque > MAX_TORQUE) {
        return MAX_TORQUE;
    } else if (torque < 0) {
        return 0;
    }
    return torque;
}

float ConvertTorqueToCurrent(float torque) {
    return torque / Kt_CONST;
}
