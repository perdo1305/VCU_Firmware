/*
 * File:   VCU_config.h
 * Author: pedro
 *
 * Created on June 19, 2024, 11:58 PM
 */

#ifndef VCU_CONFIG_H
#define VCU_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RPM 5000U
#define MAX_AD_RPM 416U //10km/h

#define MAX_CURRENT 40U
#define MAX_TORQUE 100U
#define Kt_CONST 0.94 // Nm/A


/*APPS CONFIG*/
#define __APPS_MIN 0.68f
#define __APPS_MAX 2.0f
#define __APPS_TOLERANCE 0.2f

/*BRAKE CONFIG*/
#define __BRAKE_THRESHOLD 10U



#ifdef __cplusplus
}
#endif

#endif /* VCU_CONFIG_H */
