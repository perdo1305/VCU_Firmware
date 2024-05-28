/*
 * File:   CAN_utils.h
 * Author: pedro
 *
 * Created on 5 de Mar�o de 2024, 21:55
 */

#ifndef CAN_UTILS_H
#define CAN_UTILS_H

#include <stdbool.h>  // Defines true
#include <stddef.h>   // Defines NULL
#include <stdint.h>
#include <stdlib.h>  // Defines EXIT_FAILURE

#include "../../../../Documents/git/VCU_BANCADA/firmware/VCU_BANCADA.X/Can-Header-Map/CAN_asdb.h"
#include "../../../../Documents/git/VCU_BANCADA/firmware/VCU_BANCADA.X/Can-Header-Map/CAN_datadb.h"
#include "../../../../Documents/git/VCU_BANCADA/firmware/VCU_BANCADA.X/Can-Header-Map/CAN_pwtdb.h"
#include "definitions.h"

void Read_CAN_BUS_1(void);                                         // Read CAN 1 function
void Send_CAN_BUS_1(uint32_t id, uint8_t* message, uint8_t size);  // Send CAN 1 function

void Read_CAN_BUS_2(void);                                         // Read CAN 2 function
void Send_CAN_BUS_2(uint32_t id, uint8_t* message, uint8_t size);  // Send CAN 2 function

void Read_CAN_BUS_3(void);                                         // Read CAN 2 function
void Send_CAN_BUS_3(uint32_t id, uint8_t* message, uint8_t size);  // Send CAN 2 function

void CAN_Send_VCU_Datadb_1(uint16_t consumed_power, uint16_t target_power, uint8_t brake_pressure, uint8_t throttle_position);
void CAN_Send_VCU_Datadb_2(uint16_t motor_temperature, uint16_t inverter_temperature);
void CAN_Send_VCU_Datadb_3(uint8_t vcu_state, uint8_t lmt2, uint8_t lmt1, uint16_t inverter_error);
void CAN_Send_VCU_Datadb_4(uint16_t rpm, uint16_t inverter_voltage);
void CAN_Filter_IDS_BUS1(uint32_t id);
void CAN_Filter_IDS_BUS2(uint32_t id);

typedef struct {
    uint32_t Actual_ERPM;
    uint32_t Actual_Duty;
    uint32_t Actual_InputVoltage;
    uint32_t Actual_ACCurrent;
    uint32_t Actual_DCCurrent;
    uint32_t Actual_TempController;
    uint32_t Actual_TempMotor;
    uint32_t Actual_FaultCode;
    uint32_t Actual_FOC_id;
    uint32_t Actual_FOC_iq;
    uint32_t Actual_Throttle;
    uint32_t Actual_Brake;
    uint32_t Digital_input_1;
    uint32_t Digital_input_2;
    uint32_t Digital_input_3;
    uint32_t Digital_input_4;
    uint32_t Digital_output_1;
    uint32_t Digital_output_2;
    uint32_t Digital_output_3;
    uint32_t Digital_output_4;
    uint32_t Drive_enable;
    uint32_t Capacitor_temp_limit;
    uint32_t DC_current_limit;
    uint32_t Drive_enable_limit;
    uint32_t IGBT_accel_limit;
    uint32_t IGBT_temp_limit;
    uint32_t Input_voltage_limit;
    uint32_t Motor_accel_limit;
    uint32_t Motor_temp_limit;
    uint32_t RPM_min_limit;
    uint32_t RPM_max_limit;
    uint32_t Power_limit;
    uint32_t CAN_map_version;

    uint8_t SetCurrent[2] = {0, 0};
    uint8_t SetBrakeCurrent[2] = {0, 0};
    uint8_t SetERPM[4] = {0, 0, 0, 0};
    uint8_t SetPosition[2] = {0, 0};
    uint8_t SetRelativeCurrent[2] = {0, 0};
    uint8_t SetRelativeBrakeCurrent[2] = {0, 0};
    uint8_t SetDigitalOutput[4] = {0, 0, 0, 0};
    uint8_t SetMaxACCurrent[2] = {0, 0};
    uint8_t SetMaxACBrakeCurrent[2] = {0, 0};
    uint8_t SetMaxDCCurrent[2] = {0, 0};
    uint8_t SetMaxDCBrakeCurrent[2] = {0, 0};
    uint8_t DriveEnable[1] = {0};
} HV500;
extern HV500 myHV500;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CAN_UTILS_H */
