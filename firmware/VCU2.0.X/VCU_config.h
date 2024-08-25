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

#define AUTONOMOUS_MODE 1

#define MAX_RPM 416U
#define MAX_AD_RPM 416U //10km/h
#define AD_Timeout 1000U

#define MAX_CURRENT 40U
#define MAX_TORQUE 100U
#define Kt_CONST 0.94 // Nm/A


/*APPS CONFIG*/
#define __APPS_MIN 1.23055f
#define __APPS_MAX 2.06159f
#define __APPS_TOLERANCE 0.2f // offset de tolerancia para prevenir
#define __APPS_DELTA 445U // usado para normalizar o valor do APPS

/*BRAKE CONFIG*/
#define __BRAKE_THRESHOLD 10U

/*DEFINE POWER LIMTIS*/


#ifdef __cplusplus
}
#endif

#endif /* VCU_CONFIG_H */
