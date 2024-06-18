
#include "CAN_utils.h"

// ############# RX CAN FRAME ###############################

CANFD_MSG_RX_ATTRIBUTE msgAttr = CANFD_MSG_RX_DATA_FRAME;  // RX message attribute

uint32_t status[3] = {0, 0, 0};// CAN error status

can_data_t can_bus_read(uint8_t bus) {
    static can_data_t data;
    switch (bus) {
        case CAN_BUS1:
            status[CAN_BUS1] = CAN1_ErrorGet();
            if (status[CAN_BUS1] == CANFD_ERROR_NONE) {
                memset(data.message, 0x00, sizeof(data.message));
                if (CAN1_MessageReceive(&data.id, &data.length, data.message, 0, 2, &msgAttr)) {
                    CANRX_ON[CAN_BUS1] = 1;
                }
            } else {
                CANRX_ON[CAN_BUS1] = 0;
            }
            return data;
        case CAN_BUS2:
            status[CAN_BUS2] = CAN2_ErrorGet();
            if (status[CAN_BUS2] == CANFD_ERROR_NONE) {
                memset(data.message, 0x00, sizeof(data.message));
                if (CAN2_MessageReceive(&data.id, &data.length, data.message, 0, 2, &msgAttr)) {
                    CANRX_ON[CAN_BUS2] = 1;
                }
            } else {
                CANRX_ON[CAN_BUS2] = 0;
            }
            return data;
        case CAN_BUS3:
            status[CAN_BUS3] = CAN3_ErrorGet();
            if (status[CAN_BUS3] == CANFD_ERROR_NONE) {
                memset(data.message, 0x00, sizeof(data.message));
                if (CAN3_MessageReceive(&data.id, &data.length, data.message, 0, 2, &msgAttr)) {
                    CANRX_ON[CAN_BUS3] = 1;
                }
            } else {
                CANRX_ON[CAN_BUS3] = 0;
            }
            return data;
        default:
            break;
    }
    return data;
}

void can_bus_send(uint8_t bus, can_data_t* data) {
    switch (bus) {
        case CAN_BUS1:
            if (CAN1_TxFIFOQueueIsFull(0)) {
                CANTX_ON[CAN_BUS1] = 0;
            } else {
                if (CAN1_MessageTransmit(data->id, data->length, data->message, 0, CANFD_MODE_NORMAL, CANFD_MSG_TX_DATA_FRAME)) {
                    CANTX_ON[CAN_BUS1] = 1;
                } else {
                    CANTX_ON[CAN_BUS1] = 0;
                }
            }
            break;
        case CAN_BUS2:
            if (CAN2_TxFIFOQueueIsFull(0)) {
                CANTX_ON[CAN_BUS2] = 0;
            } else {
                if (CAN2_MessageTransmit(data->id, data->length, data->message, 0, CANFD_MODE_NORMAL, CANFD_MSG_TX_DATA_FRAME)) {
                    CANTX_ON[CAN_BUS2] = 1;
                } else {
                    CANTX_ON[CAN_BUS2] = 0;
                }
            }
            break;
        case CAN_BUS3:
            if (CAN3_TxFIFOQueueIsFull(0)) {
                CANTX_ON[CAN_BUS3] = 0;
            } else {
                if (CAN3_MessageTransmit(data->id, data->length, data->message, 0, CANFD_MODE_NORMAL, CANFD_MSG_TX_DATA_FRAME)) {
                    CANTX_ON[CAN_BUS3] = 1;
                } else {
                    CANTX_ON[CAN_BUS3] = 0;
                }
            }
            break;
        default:
            break;
    }
}

/*

░█████╗░░█████╗░███╗░░██╗  ░░███╗░░  ░░░░░░  ██████╗░░█████╗░████████╗░█████╗░██████╗░██╗░░░██╗░██████╗
██╔══██╗██╔══██╗████╗░██║  ░████║░░  ░░░░░░  ██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗██╔══██╗██║░░░██║██╔════╝
██║░░╚═╝███████║██╔██╗██║  ██╔██║░░  █████╗  ██║░░██║███████║░░░██║░░░███████║██████╦╝██║░░░██║╚█████╗░
██║░░██╗██╔══██║██║╚████║  ╚═╝██║░░  ╚════╝  ██║░░██║██╔══██║░░░██║░░░██╔══██║██╔══██╗██║░░░██║░╚═══██╗
╚█████╔╝██║░░██║██║░╚███║  ███████╗  ░░░░░░  ██████╔╝██║░░██║░░░██║░░░██║░░██║██████╦╝╚██████╔╝██████╔╝
░╚════╝░╚═╝░░╚═╝╚═╝░░╚══╝  ╚══════╝  ░░░░░░  ╚═════╝░╚═╝░░╚═╝░░░╚═╝░░░╚═╝░░╚═╝╚═════╝░░╚═════╝░╚═════╝░
 */
void CAN_Send_VCU_Datadb_1(uint16_t consumed_power, uint16_t target_power, uint8_t brake_pressure, uint8_t throttle_position) {
    can_data_t data;
    data.id = CAN_VCU_ID_1;
    data.length = 8;

    MAP_ENCODE_CONSUMED_POWER(data.message, consumed_power);
    MAP_ENCODE_TARGET_POWER(data.message, target_power);
    MAP_ENCODE_BRAKE_PRESSURE(data.message, brake_pressure);
    MAP_ENCODE_THROTTLE_POSITION(data.message, throttle_position);

    can_bus_send(CAN_BUS1, &data);
}

void CAN_Send_VCU_Datadb_2(uint16_t motor_temperature, uint16_t inverter_temperature) {
    can_data_t data;
    data.id = CAN_VCU_ID_2;
    data.length = 8;

    MAP_ENCODE_MOTOR_TEMPERATURE(data.message, motor_temperature);
    MAP_ENCODE_INVERTER_TEMPERATURE(data.message, inverter_temperature);

    can_bus_send(CAN_BUS1, &data);
}

void CAN_Send_VCU_Datadb_3(uint8_t vcu_state, uint8_t lmt2, uint8_t lmt1, uint16_t inverter_error) {
    can_data_t data;
    data.id = CAN_VCU_ID_3;
    data.length = 8;

    MAP_ENCODE_VCU_STATE(data.message, vcu_state);
    MAP_ENCODE_LMT2(data.message, lmt2);
    MAP_ENCODE_LMT1(data.message, lmt1);

    MAP_ENCODE_INVERTER_ERROR(data.message, inverter_error);

    can_bus_send(CAN_BUS1, &data);
}

void CAN_Send_VCU_Datadb_4(uint16_t rpm, uint16_t inverter_voltage) {
    can_data_t data;
    data.id = CAN_VCU_ID_4;
    data.length = 8;

    MAP_ENCODE_RPM(data.message, rpm);
    MAP_ENCODE_INVERTER_VOLTAGE(data.message, inverter_voltage);

    can_bus_send(CAN_BUS1, &data);
}

/*

░█████╗░░█████╗░███╗░░██╗  ██████╗░  ░░░░░░██████╗░░█████╗░░██╗░░░░░░░██╗███████╗██████╗░████████╗██████╗░░█████╗░██╗███╗░░██╗
██╔══██╗██╔══██╗████╗░██║  ╚════██╗  ░░░░░░██╔══██╗██╔══██╗░██║░░██╗░░██║██╔════╝██╔══██╗╚══██╔══╝██╔══██╗██╔══██╗██║████╗░██║
██║░░╚═╝███████║██╔██╗██║  ░░███╔═╝  █████╗██████╔╝██║░░██║░╚██╗████╗██╔╝█████╗░░██████╔╝░░░██║░░░██████╔╝███████║██║██╔██╗██║
██║░░██╗██╔══██║██║╚████║  ██╔══╝░░  ╚════╝██╔═══╝░██║░░██║░░████╔═████║░██╔══╝░░██╔══██╗░░░██║░░░██╔══██╗██╔══██║██║██║╚████║
╚█████╔╝██║░░██║██║░╚███║  ███████╗  ░░░░░░██║░░░░░╚█████╔╝░░╚██╔╝░╚██╔╝░███████╗██║░░██║░░░██║░░░██║░░██║██║░░██║██║██║░╚███║
░╚════╝░╚═╝░░╚═╝╚═╝░░╚══╝  ╚══════╝  ░░░░░░╚═╝░░░░░░╚════╝░░░░╚═╝░░░╚═╝░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝╚═╝░░╚══╝
 */
/*
//TODO e neccessario rever o excel com estas cenas!!
void CAN_Send_VCU_PWTDB_1(uint16_t ac_current, uint16_t brake_current) {
    MAP_ENCODE_CMD_AcCurrent(tx.message, ac_current);
    MAP_ENCODE_CMD_BrakeCurrent(tx.message, brake_current);

    Send_CAN_BUS_2(CAN_HV500_SetAcCurrent_ID, tx.message, 8);
}

void CAN_Send_VCU_PWTDB_2(uint32_t erpm, uint32_t position) {
    MAP_ENCODE_CMD_ERPM(tx.message, erpm);
    MAP_ENCODE_CMD_Position(tx.message, position);

    Send_CAN_BUS_2(CAN_HV500_SetERPM_ID, tx.message, 8);
}

void CAN_Send_VCU_PWTDB_3(uint32_t rel_current, uint32_t rel_brake_current) {
    MAP_ENCODE_CMD_RelCurrent(tx.message, rel_current);
    MAP_ENCODE_CMD_RelBrakeCurrent(tx.message, rel_brake_current);

    Send_CAN_BUS_2(CAN_HV500_SetRelCurrent_ID, tx.message, 8);
}

void CAN_Send_VCU_PWTDB_4(uint32_t max_ac_current, uint32_t max_ac_brake_current) {
    MAP_ENCODE_CMD_MaxAcCurrent(tx.message, max_ac_current);
    MAP_ENCODE_CMD_MaxAcBrakeCurrent(tx.message, max_ac_brake_current);

    Send_CAN_BUS_2(CAN_HV500_SetMaxAcCurrent_ID, tx.message, 8);
}
 */

// TODO ACABAR ISTO
uint32_t WheelSpeed_RL = 0;
uint32_t WheelSpeed_RR = 0;
uint32_t SusPOS_RL = 0;
uint32_t SusPOS_RR = 0;
uint32_t BRK_State = 0;
uint32_t DY_REAR_STATE = 0;

void CAN_Filter_IDS_BUS1(can_data_t* data) {
}

// ############# PWTDB CAN VARS ###################################

// Receive
/*
void CAN_Filter_IDS_BUS2(uint32_t id) {

    switch (id) {
        case CAN_HV500_ERPM_DUTY_VOLTAGE_ID:
            myHV500.Actual_ERPM = MAP_DECODE_Actual_ERPM(rx[CAN_BUS2].message);
            myHV500.Actual_Duty = MAP_DECODE_Actual_Duty(rx[CAN_BUS2].message);
            myHV500.Actual_InputVoltage = MAP_DECODE_Actual_InputVoltage(rx[CAN_BUS2].message);
            break;
        case CAN_HV500_AC_DC_current_ID:

            myHV500.Actual_ACCurrent = MAP_DECODE_Actual_ACCurrent(rx[CAN_BUS2].message);
            myHV500.Actual_DCCurrent = MAP_DECODE_Actual_DCCurrent(rx[CAN_BUS2].message);
            break;
        case CAN_HV500_Temperatures_ID:

            myHV500.Actual_TempController = MAP_DECODE_Actual_TempController(rx[CAN_BUS2].message);
            myHV500.Actual_TempMotor = MAP_DECODE_Actual_TempMotor(rx[CAN_BUS2].message);
            myHV500.Actual_FaultCode = MAP_DECODE_Actual_FaultCode(rx[CAN_BUS2].message);

            break;
        case CAN_HV500_FOC_ID:
            myHV500.Actual_FOC_id = MAP_DECODE_Actual_FOC_id(rx[CAN_BUS2].message);
            myHV500.Actual_FOC_iq = MAP_DECODE_Actual_FOC_iq(rx[CAN_BUS2].message);
            break;
        case CAN_HV500_MISC_ID:

            myHV500.Actual_Throttle = MAP_DECODE_Actual_Throttle(rx[CAN_BUS2].message);
            myHV500.Actual_Brake = MAP_DECODE_Actual_Brake(rx[CAN_BUS2].message);
            myHV500.Digital_input_1 = MAP_DECODE_Digital_input_1(rx[CAN_BUS2].message);
            myHV500.Digital_input_2 = MAP_DECODE_Digital_input_2(rx[CAN_BUS2].message);
            myHV500.Digital_input_3 = MAP_DECODE_Digital_input_3(rx[CAN_BUS2].message);
            myHV500.Digital_input_4 = MAP_DECODE_Digital_input_4(rx[CAN_BUS2].message);
            myHV500.Digital_output_1 = MAP_DECODE_Digital_output_1(rx[CAN_BUS2].message);
            myHV500.Digital_output_2 = MAP_DECODE_Digital_output_2(rx[CAN_BUS2].message);
            myHV500.Digital_output_3 = MAP_DECODE_Digital_output_3(rx[CAN_BUS2].message);
            myHV500.Digital_output_4 = MAP_DECODE_Digital_output_4(rx[CAN_BUS2].message);
            myHV500.Drive_enable = MAP_DECODE_Drive_enable(rx[CAN_BUS2].message);
            myHV500.Capacitor_temp_limit = MAP_DECODE_Capacitor_temp_limit(rx[CAN_BUS2].message);
            myHV500.DC_current_limit = MAP_DECODE_DC_current_limit(rx[CAN_BUS2].message);
            myHV500.Drive_enable_limit = MAP_DECODE_Drive_enable_limit(rx[CAN_BUS2].message);
            myHV500.IGBT_accel_limit = MAP_DECODE_IGBT_accel_limit(rx[CAN_BUS2].message);
            myHV500.IGBT_temp_limit = MAP_DECODE_IGBT_temp_limit(rx[CAN_BUS2].message);
            myHV500.Input_voltage_limit = MAP_DECODE_Input_voltage_limit(rx[CAN_BUS2].message);
            myHV500.Motor_accel_limit = MAP_DECODE_Motor_accel_limit(rx[CAN_BUS2].message);
            myHV500.Motor_temp_limit = MAP_DECODE_Motor_temp_limit(rx[CAN_BUS2].message);
            myHV500.RPM_min_limit = MAP_DECODE_RPM_min_limit(rx[CAN_BUS2].message);
            myHV500.RPM_max_limit = MAP_DECODE_RPM_max_limit(rx[CAN_BUS2].message);
            myHV500.Power_limit = MAP_DECODE_Power_limit(rx[CAN_BUS2].message);
            myHV500.CAN_map_version = MAP_DECODE_CAN_map_version(rx[CAN_BUS2].message);

            break;
        default:
            break;
    }
}*/

// send
/*

void Send_CAN_HV500_SetAcCurrent(uint16_t ac_current) {
    MAP_ENCODE_CMD_AcCurrent(tx[CAN_BUS2].message, ac_current);
    Send_CAN_BUS_1(CAN_HV500_SetAcCurrent_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetBrakeCurrent(uint16_t brake_current) {
    MAP_ENCODE_CMD_BrakeCurrent(tx[CAN_BUS2].message, brake_current);
    Send_CAN_BUS_1(CAN_HV500_SetBrakeCurrent_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetERPM(uint32_t erpm) {
    MAP_ENCODE_CMD_ERPM(tx[CAN_BUS2].message, erpm);
    Send_CAN_BUS_1(CAN_HV500_SetERPM_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetPosition(uint32_t position) {
    MAP_ENCODE_CMD_Position(tx[CAN_BUS2].message, position);
    Send_CAN_BUS_1(CAN_HV500_SetPosition_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetRelCurrent(uint32_t rel_current) {
    MAP_ENCODE_CMD_RelCurrent(tx[CAN_BUS2].message, rel_current);
    Send_CAN_BUS_1(CAN_HV500_SetRelCurrent_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetRelBrakeCurrent(uint32_t rel_brake_current) {
    MAP_ENCODE_CMD_RelBrakeCurrent(tx[CAN_BUS2].message, rel_brake_current);
    Send_CAN_BUS_1(CAN_HV500_SetRelBrakeCurrent_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetMaxAcCurrent(uint32_t max_ac_current) {
    MAP_ENCODE_CMD_MaxAcCurrent(tx[CAN_BUS2].message, max_ac_current);
    Send_CAN_BUS_1(CAN_HV500_SetMaxAcCurrent_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetMaxAcBrakeCurrent(uint32_t max_ac_brake_current) {
    MAP_ENCODE_CMD_MaxAcBrakeCurrent(tx[CAN_BUS2].message, max_ac_brake_current);
    Send_CAN_BUS_1(CAN_HV500_SetMaxAcBrakeCurrent_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetMaxDcCurrent(uint32_t max_dc_current) {
    MAP_ENCODE_CMD_MaxDcCurrent(tx[CAN_BUS2].message, max_dc_current);
    Send_CAN_BUS_1(CAN_HV500_SetMaxDcCurrent_ID, tx[CAN_BUS2].message, 8);
}

void Send_CAN_HV500_SetDriveEnable(uint32_t drive_enable) {
    MAP_ENCODE_CMD_DriveEnable(tx[CAN_BUS2].message, drive_enable);
    Send_CAN_BUS_1(CAN_HV500_SetDriveEnable_ID, tx[CAN_BUS2].message, 8);
}*/
// #####################################################################
