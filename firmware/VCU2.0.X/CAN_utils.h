/*
 * File:   CAN_utils.h
 * Author: pedro
 *
 * Created on 5 de Mar�o de 2024, 21:55
 */

#ifndef CAN_UTILS_H
#define CAN_UTILS_H

#define CAN_BUS1 0U
#define CAN_BUS2 1U
#define CAN_BUS3 2U

#include <stdbool.h>  // Defines true
#include <stddef.h>   // Defines NULL
#include <stdint.h>
#include <stdlib.h>  // Defines EXIT_FAILURE

#include "../../../../2_FIRMWARE/VCU_BANCADA/firmware/VCU_BANCADA.X/Can-Header-Map/CAN_asdb.h"
#include "../../../../2_FIRMWARE/VCU_BANCADA/firmware/VCU_BANCADA.X/Can-Header-Map/CAN_datadb.h"
#include "../../../../2_FIRMWARE/VCU_BANCADA/firmware/VCU_BANCADA.X/Can-Header-Map/CAN_pwtdb.h"
#include "definitions.h"

typedef struct {
    uint32_t id;
    uint8_t message[8];
    uint8_t length;
} can_data_t;

can_data_t can_bus_read(uint8_t bus);
void can_bus_send(uint8_t bus, can_data_t* data);

void CAN_Send_VCU_Datadb_1(uint16_t consumed_power, uint16_t target_power, uint8_t brake_pressure, uint8_t throttle_position);
void CAN_Send_VCU_Datadb_2(uint16_t motor_temperature, uint16_t inverter_temperature);
void CAN_Send_VCU_Datadb_3(uint8_t vcu_state, uint8_t lmt2, uint8_t lmt1, uint16_t inverter_error);
void CAN_Send_VCU_Datadb_4(uint16_t rpm, uint16_t inverter_voltage);
void CAN_Filter_IDS_BUS1(can_data_t* data);
void CAN_Filter_IDS_BUS2(can_data_t* data);

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

    uint8_t SetCurrent[2];
    uint8_t SetBrakeCurrent[2];
    uint8_t SetERPM[4];
    uint8_t SetPosition[2];
    uint8_t SetRelativeCurrent[2];
    uint8_t SetRelativeBrakeCurrent[2];
    uint8_t SetDigitalOutput[4];
    uint8_t SetMaxACCurrent[2];
    uint8_t SetMaxACBrakeCurrent[2];
    uint8_t SetMaxDCCurrent[2];
    uint8_t SetMaxDCBrakeCurrent[2];
    uint8_t DriveEnable[1];
} HV500;
extern HV500 myHV500;

typedef struct {
    bool VCU;
    bool PDM;
    bool IMU;
    bool DynamicsRear;
    bool DynamicsFront;
} CAN1;

typedef struct {
    bool VCU;
    bool Inverter;
    bool TCU;
    bool BMS;
} CAN2;

typedef struct {
    bool ACU;
    bool FSG_Datalogger;
    bool ALC;
    bool SteeringWheel;
} CAN3;

typedef struct {
    CAN1 can1;
    CAN2 can2;
    CAN3 can3;
} BoardStatus;
extern BoardStatus myboardStatus;

extern bool CANRX_ON[3];  // flag to check if CAN is receiving
extern bool CANTX_ON[3];  // flag to check if CAN is transmitting



#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CAN_UTILS_H */
