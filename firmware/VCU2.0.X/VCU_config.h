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

#define MAX_RPM 420U
#define MAX_AD_RPM 420U  // 10km/h
#define AD_Timeout 1000U

#define MAX_CURRENT 40U
#define MAX_TORQUE 100U
#define Kt_CONST 0.94  // Nm/A

/*APPS CONFIG*/
#define __APPS_MIN 0.944469f
#define __APPS_MAX 1.75275f
#define __APPS_TOLERANCE 0.035f  // offset de tolerancia para prevenir
#define __APPS_DELTA 420U        // usado para normalizar o valor do APPS

/*BRAKE CONFIG*/
#define __BRAKE_THRESHOLD 15U

/*DEFINE POWER LIMTIS*/

#ifdef __cplusplus
}
#endif

#endif /* VCU_CONFIG_H */
