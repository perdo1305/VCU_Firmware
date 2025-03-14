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
#define CAN_BUS4 3U

#include <stdbool.h>  // Defines true
#include <stddef.h>   // Defines NULL
#include <stdint.h>
#include <stdlib.h>  // Defines EXIT_FAILURE

#include "Can-Header-Map/CANOPEN_db.h"
#include "Can-Header-Map/CAN_asdb.h"
#include "Can-Header-Map/CAN_datadb.h"
#include "Can-Header-Map/CAN_pwtdb.h"
#include "definitions.h"

typedef struct {
    uint32_t id;
    uint8_t message[64];
    uint8_t length;
} can_data_t;

can_data_t can_bus_read(uint8_t bus);
void can_bus_send(uint8_t bus, can_data_t* data);

void can_open_init(void);

/* Send to DataBus*/
void can_bus_send_databus_1(uint16_t consumed_power, uint16_t target_power, uint8_t brake_pressure, uint8_t throttle_position);
void can_bus_send_databus_2(uint16_t motor_temperature, uint16_t inverter_temperature, uint16_t hv_voltage, uint8_t hv_soc);
void can_bus_send_databus_3(uint8_t vcu_state, uint8_t lmt2, uint8_t lmt1, uint16_t inverter_error, uint8_t apps_error, uint8_t power_plan);
void can_bus_send_databus_4(uint16_t rpm, uint16_t inverter_voltage, uint8_t ignition_state, uint8_t ready2drive_state);
void can_bus_send_databus_5(uint8_t tcu_state, uint8_t acu_state, uint8_t alc_state, uint16_t lv_soc, uint16_t lv_voltage);

/* Send to PowerTrainBus*/
void can_bus_send_HV500_SetAcCurrent(uint16_t ac_current);
void can_bus_send_HV500_SetBrakeCurrent(uint16_t brake_current);
void can_bus_send_HV500_SetERPM(uint32_t erpm);
void can_bus_send_HV500_SetPosition(uint32_t position);
void can_bus_send_HV500_SetRelCurrent(uint32_t rel_current);
void can_bus_send_HV500_SetRelBrakeCurrent(uint32_t rel_brake_current);
void can_bus_send_HV500_SetMaxAcCurrent(uint32_t max_ac_current);
void can_bus_send_HV500_SetMaxAcBrakeCurrent(uint32_t max_ac_brake_current);
void can_bus_send_HV500_SetMaxDcCurrent(uint32_t max_dc_current);
void can_bus_send_HV500_SetMaxDcBrakeCurrent(uint32_t max_dc_brake_current);
void can_bus_send_HV500_SetDriveEnable(uint32_t drive_enable);

void can_bus_send_pwtbus_1(uint8_t r2d, uint8_t ignition);

/*Send Directly to AutonomousBus*/
void can_bus_send_AdBus_RPM(uint32_t rpm);

/*data stucture for interating with the HV500 inverter*/
typedef struct {
    // Received data
    long Actual_ERPM;
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

    uint16_t SetCurrent;
    uint16_t SetBrakeCurrent;
    uint32_t SetERPM;
    uint16_t SetPosition;
    uint16_t SetRelativeCurrent;
    uint16_t SetRelativeBrakeCurrent;
    uint32_t SetDigitalOutput;
    uint16_t SetMaxACCurrent;
    uint16_t SetMaxACBrakeCurrent;
    uint16_t SetMaxDCCurrent;
    uint16_t SetMaxDCBrakeCurrent;
    uint8_t DriveEnable;
} HV500;
extern HV500 myHV500;

typedef struct {
    uint8_t BatteryFansTop;
    uint8_t BatteryFansBottom;
    uint8_t RadFan_Left;
    uint8_t RadFan_Right;
    uint8_t Liquid_temp_1;
    uint8_t Liquid_temp_2;
    uint8_t Liquid_temp_3;

    uint8_t Contactors_State;
    uint8_t Contactors_Error;
    uint8_t Autonomous_Ignition;
    uint8_t Precharge_done;
    uint8_t SDC_State;
    uint8_t TCU_STATE;
} TCUvars_t;
extern TCUvars_t tcu;

typedef struct {
    uint16_t instant_voltage;
    uint16_t open_voltage;
    uint8_t soc;
    uint16_t pack_current;

    uint16_t high_cell_voltage;
    uint8_t high_cell_voltage_id;
    uint16_t low_cell_voltage;
    uint8_t low_cell_voltage_id;
    uint16_t avg_cell_voltage;

    uint16_t high_cell_temp;
    uint8_t high_cell_temp_id;
    uint16_t low_cell_temp;
    uint8_t low_cell_temp_id;
    uint8_t ambient_temp;

    uint16_t max_discharge_current;

} BMSvars_t;
extern BMSvars_t bms;
typedef struct {
} ALCvars_t;
extern ALCvars_t alc;

extern bool CANRX_ON[4];  // flag to check if CAN is receiving
extern bool CANTX_ON[4];  // flag to check if CAN is transmitting

extern bool AS_Emergency;
extern uint16_t TOJAL_RX_RPM;
extern uint32_t RPM_TOJAL;
extern uint8_t RES_AD_Ignition;
extern bool ACU_Autonomous_ignition;
extern uint8_t TCU_Precharge_done;
extern uint16_t Actual_InputVoltage;
extern uint8_t powerPlan_volante;
extern uint8_t AS_Status;  // 1- OFF, 2-Ready, 3-Driving, 4-Emergency, 5-Finished
extern uint32_t AD_timeout;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CAN_UTILS_H */
