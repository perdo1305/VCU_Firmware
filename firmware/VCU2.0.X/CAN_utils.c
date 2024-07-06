
#include "CAN_utils.h"

void can_filter_id_bus1(can_data_t* data);
void can_filter_id_bus2(can_data_t* data);
void can_filter_id_bus3(can_data_t* data);
void can_filter_id_bus4(can_data_t* data);

uint16_t TOJAL_RX_RPM = 0;
uint32_t RPM_TOJAL = 0;

// ############# RX CAN FRAME ###############################

CANFD_MSG_RX_ATTRIBUTE msgAttr = CANFD_MSG_RX_DATA_FRAME;  // RX message attribute

uint32_t status[4] = {0, 0, 0, 0};  // CAN error status

can_data_t can_bus_read(uint8_t bus) {
    static can_data_t data;
    uint8_t tempvar = 0;
    switch (bus) {
        case CAN_BUS1:
            status[CAN_BUS1] = CAN1_ErrorGet();
            //if (status[CAN_BUS1] == CANFD_ERROR_NONE) {
            
            if (!tempvar) {
                memset(data.message, 0x00, sizeof(data.message));
                if (CAN1_MessageReceive(&data.id, &data.length, data.message, 0, 2, &msgAttr)) {
                    CANRX_ON[CAN_BUS1] = 1;

                    can_filter_id_bus1(&data);
                }
            } else {
                CANRX_ON[CAN_BUS1] = 0;
            }
            return data;
        case CAN_BUS2:
            status[CAN_BUS2] = CAN2_ErrorGet();
            //if (status[CAN_BUS2] == CANFD_ERROR_NONE) {
            if (!tempvar) {
                memset(data.message, 0x00, sizeof(data.message));
                if (CAN2_MessageReceive(&data.id, &data.length, data.message, 0, 2, &msgAttr)) {
                    CANRX_ON[CAN_BUS2] = 1;
                    can_filter_id_bus2(&data);
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
                    // can_filter_id_bus3(&data);
                }
            } else {
                CANRX_ON[CAN_BUS3] = 0;
            }
            return data;

        case CAN_BUS4:
            status[CAN_BUS4] = CAN4_ErrorGet();
            if (status[CAN_BUS4] == CANFD_ERROR_NONE) {
                memset(data.message, 0x00, sizeof(data.message));
                if (CAN4_MessageReceive(&data.id, &data.length, data.message, 0, 2, &msgAttr)) {
                    CANRX_ON[CAN_BUS4] = 1;

                    can_filter_id_bus4(&data);
                }
            } else {
                CANRX_ON[CAN_BUS4] = 0;
            }
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

        case CAN_BUS4:
            if (CAN4_TxFIFOQueueIsFull(0)) {
                CANTX_ON[CAN_BUS4] = 0;
            } else {
                if (CAN4_MessageTransmit(data->id, data->length, data->message, 0, CANFD_MODE_NORMAL, CANFD_MSG_TX_DATA_FRAME)) {
                    CANTX_ON[CAN_BUS4] = 1;

                } else {
                    CANTX_ON[CAN_BUS4] = 0;
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

void can_bus_send_databus_1(uint16_t consumed_power, uint16_t target_power, uint8_t brake_pressure, uint8_t throttle_position) {
    can_data_t data;
    data.id = CAN_VCU_ID_1;
    data.length = 8;

    MAP_ENCODE_CONSUMED_POWER(data.message, consumed_power);
    MAP_ENCODE_TARGET_POWER(data.message, target_power);
    MAP_ENCODE_BRAKE_PRESSURE(data.message, brake_pressure);
    MAP_ENCODE_THROTTLE_POSITION(data.message, throttle_position);

    can_bus_send(CAN_BUS1, &data);
}

void can_bus_send_databus_2(uint16_t motor_temperature, uint16_t inverter_temperature) {
    can_data_t data;
    data.id = CAN_VCU_ID_2;
    data.length = 8;

    MAP_ENCODE_MOTOR_TEMPERATURE(data.message, motor_temperature);
    MAP_ENCODE_INVERTER_TEMPERATURE(data.message, inverter_temperature);

    can_bus_send(CAN_BUS1, &data);
}

void can_bus_send_databus_3(uint8_t vcu_state, uint8_t lmt2, uint8_t lmt1, uint16_t inverter_error) {
    can_data_t data;
    data.id = CAN_VCU_ID_3;
    data.length = 8;

    MAP_ENCODE_VCU_STATE(data.message, vcu_state);
    MAP_ENCODE_LMT2(data.message, lmt2);
    MAP_ENCODE_LMT1(data.message, lmt1);

    MAP_ENCODE_INVERTER_ERROR(data.message, inverter_error);

    can_bus_send(CAN_BUS1, &data);
}

void can_bus_send_databus_4(uint16_t rpm, uint16_t inverter_voltage) {
    can_data_t data;
    data.id = CAN_VCU_ID_4;
    data.length = 8;

    MAP_ENCODE_RPM(data.message, rpm);
    MAP_ENCODE_INVERTER_VOLTAGE(data.message, inverter_voltage);

    can_bus_send(CAN_BUS1, &data);
}

void can_filter_id_bus1(can_data_t* data) {
    switch (data->id) {
        case CAN_VCU_ID_1:
            TOJAL_RX_RPM = MAP_DECODE_TOJAL_RPM(data->message);
            break;
        default:
            break;
    }
}

/*
░█████╗░░█████╗░███╗░░██╗  ██████╗░  ░░░░░░██████╗░░█████╗░░██╗░░░░░░░██╗███████╗██████╗░████████╗██████╗░░█████╗░██╗███╗░░██╗
██╔══██╗██╔══██╗████╗░██║  ╚════██╗  ░░░░░░██╔══██╗██╔══██╗░██║░░██╗░░██║██╔════╝██╔══██╗╚══██╔══╝██╔══██╗██╔══██╗██║████╗░██║
██║░░╚═╝███████║██╔██╗██║  ░░███╔═╝  █████╗██████╔╝██║░░██║░╚██╗████╗██╔╝█████╗░░██████╔╝░░░██║░░░██████╔╝███████║██║██╔██╗██║
██║░░██╗██╔══██║██║╚████║  ██╔══╝░░  ╚════╝██╔═══╝░██║░░██║░░████╔═████║░██╔══╝░░██╔══██╗░░░██║░░░██╔══██╗██╔══██║██║██║╚████║
╚█████╔╝██║░░██║██║░╚███║  ███████╗  ░░░░░░██║░░░░░╚█████╔╝░░╚██╔╝░╚██╔╝░███████╗██║░░██║░░░██║░░░██║░░██║██║░░██║██║██║░╚███║
░╚════╝░╚═╝░░╚═╝╚═╝░░╚══╝  ╚══════╝  ░░░░░░╚═╝░░░░░░╚════╝░░░░╚═╝░░░╚═╝░░╚══════╝╚═╝░░╚═╝░░░╚═╝░░░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝╚═╝░░╚══╝
 */
/*Receive*/

/*Send*/

/*Send_CAN_HV500_SetAcCurrent*/
void can_bus_send_HV500_SetAcCurrent(uint16_t ac_current) {
    can_data_t data;
    data.id = CAN_HV500_SetAcCurrent_ID;
    data.length = 8;

    MAP_ENCODE_CMD_AcCurrent(data.message, ac_current);

    can_bus_send(CAN_BUS2, &data);
}

/*Send_CAN_HV500_SetBrakeCurrent*/
void can_bus_send_HV500_SetBrakeCurrent(uint16_t brake_current) {
    can_data_t data;
    data.id = CAN_HV500_SetBrakeCurrent_ID;
    data.length = 8;

    MAP_ENCODE_CMD_BrakeCurrent(data.message, brake_current);

    can_bus_send(CAN_BUS2, &data);
}

/*Send_CAN_HV500_SetERPM*/
void can_bus_send_HV500_SetERPM(uint32_t erpm) {
    can_data_t data;
    data.id = CAN_HV500_SetERPM_ID;
    data.length = 8;

    MAP_ENCODE_CMD_ERPM(data.message, erpm);

    can_bus_send(CAN_BUS2, &data);
}

/*Send_CAN_HV500_SetPosition*/
void can_bus_send_HV500_SetPosition(uint32_t position) {
    can_data_t data;
    data.id = CAN_HV500_SetPosition_ID;
    data.length = 8;

    MAP_ENCODE_CMD_Position(data.message, position);

    can_bus_send(CAN_BUS2, &data);
}

/*Send_CAN_HV500_SetRelCurrent*/
void can_bus_send_HV500_SetRelCurrent(uint32_t rel_current) {
    can_data_t data;
    data.id = CAN_HV500_SetRelCurrent_ID;
    data.length = 8;

    MAP_ENCODE_CMD_RelCurrent(data.message, rel_current);

    can_bus_send(CAN_BUS2, &data);
}

/*Send_CAN_HV500_SetRelBrakeCurrent*/
void can_bus_send_HV500_SetRelBrakeCurrent(uint32_t rel_brake_current) {
    can_data_t data;
    data.id = CAN_HV500_SetRelBrakeCurrent_ID;
    data.length = 8;

    MAP_ENCODE_CMD_RelBrakeCurrent(data.message, rel_brake_current);

    can_bus_send(CAN_BUS2, &data);
}

/*Send_CAN_HV500_SetMaxAcCurrent*/
void can_bus_send_HV500_SetMaxAcCurrent(uint32_t max_ac_current) {
    can_data_t data;
    data.id = CAN_HV500_SetMaxAcCurrent_ID;
    data.length = 8;

    MAP_ENCODE_CMD_MaxAcCurrent(data.message, max_ac_current);

    can_bus_send(CAN_BUS2, &data);
}

/*Send_CAN_HV500_SetMaxAcBrakeCurrent*/
void can_bus_send_HV500_SetMaxAcBrakeCurrent(uint32_t max_ac_brake_current) {
    can_data_t data;
    data.id = CAN_HV500_SetMaxAcBrakeCurrent_ID;
    data.length = 8;

    MAP_ENCODE_CMD_MaxAcBrakeCurrent(data.message, max_ac_brake_current);

    can_bus_send(CAN_BUS2, &data);
}

/*CAN_HV500_SetMaxDcCurrent_ID*/
void can_bus_send_HV500_SetMaxDcCurrent(uint32_t max_dc_current) {
    can_data_t data;
    data.id = CAN_HV500_SetMaxDcCurrent_ID;
    data.length = 8;

    MAP_ENCODE_CMD_MaxDcCurrent(data.message, max_dc_current);

    can_bus_send(CAN_BUS2, &data);
}

/*CAN_HV500_SetMaxDcBrakeCurrent_ID*/
void can_bus_send_HV500_SetMaxDcBrakeCurrent(uint32_t max_dc_brake_current) {
    can_data_t data;
    data.id = CAN_HV500_SetMaxDcBrakeCurrent_ID;
    data.length = 8;

    MAP_ENCODE_CMD_MaxDcBrakeCurrent(data.message, max_dc_brake_current);

    can_bus_send(CAN_BUS2, &data);
}

/*Send_CAN_HV500_SetDriveEnable*/
void can_bus_send_HV500_SetDriveEnable(uint32_t drive_enable) {
    can_data_t data;
    data.id = CAN_HV500_SetDriveEnable_ID;
    data.length = 8;

    MAP_ENCODE_CMD_DriveEnable(data.message, drive_enable);

    can_bus_send(CAN_BUS2, &data);
}

// ############# PWTDB CAN VARS ###################################

// Receive

void can_filter_id_bus2(can_data_t* data) {
    switch (data->id) {
        case CAN_HV500_ERPM_DUTY_VOLTAGE_ID:
            myHV500.Actual_ERPM = MAP_DECODE_Actual_ERPM(data->message);
            myHV500.Actual_Duty = MAP_DECODE_Actual_Duty(data->message);
            myHV500.Actual_InputVoltage = MAP_DECODE_Actual_InputVoltage(data->message);
            break;
        case CAN_HV500_AC_DC_current_ID:

            myHV500.Actual_ACCurrent = MAP_DECODE_Actual_ACCurrent(data->message);
            myHV500.Actual_DCCurrent = MAP_DECODE_Actual_DCCurrent(data->message);
            break;
        case CAN_HV500_Temperatures_ID:

            myHV500.Actual_TempController = MAP_DECODE_Actual_TempController(data->message);
            myHV500.Actual_TempMotor = MAP_DECODE_Actual_TempMotor(data->message);
            myHV500.Actual_FaultCode = MAP_DECODE_Actual_FaultCode(data->message);

            break;
        case CAN_HV500_FOC_ID:
            myHV500.Actual_FOC_id = MAP_DECODE_Actual_FOC_id(data->message);
            myHV500.Actual_FOC_iq = MAP_DECODE_Actual_FOC_iq(data->message);
            break;
        case CAN_HV500_MISC_ID:
            myHV500.Actual_Throttle = MAP_DECODE_Actual_Throttle(data->message);
            myHV500.Actual_Brake = MAP_DECODE_Actual_Brake(data->message);
            myHV500.Digital_input_1 = MAP_DECODE_Digital_input_1(data->message);
            myHV500.Digital_input_2 = MAP_DECODE_Digital_input_2(data->message);
            myHV500.Digital_input_3 = MAP_DECODE_Digital_input_3(data->message);
            myHV500.Digital_input_4 = MAP_DECODE_Digital_input_4(data->message);
            myHV500.Digital_output_1 = MAP_DECODE_Digital_output_1(data->message);
            myHV500.Digital_output_2 = MAP_DECODE_Digital_output_2(data->message);
            myHV500.Digital_output_3 = MAP_DECODE_Digital_output_3(data->message);
            myHV500.Digital_output_4 = MAP_DECODE_Digital_output_4(data->message);
            myHV500.Drive_enable = MAP_DECODE_Drive_enable(data->message);
            myHV500.Capacitor_temp_limit = MAP_DECODE_Capacitor_temp_limit(data->message);
            myHV500.DC_current_limit = MAP_DECODE_DC_current_limit(data->message);
            myHV500.Drive_enable_limit = MAP_DECODE_Drive_enable_limit(data->message);
            myHV500.IGBT_accel_limit = MAP_DECODE_IGBT_accel_limit(data->message);
            myHV500.IGBT_temp_limit = MAP_DECODE_IGBT_temp_limit(data->message);
            myHV500.Input_voltage_limit = MAP_DECODE_Input_voltage_limit(data->message);
            myHV500.Motor_accel_limit = MAP_DECODE_Motor_accel_limit(data->message);
            myHV500.Motor_temp_limit = MAP_DECODE_Motor_temp_limit(data->message);
            myHV500.RPM_min_limit = MAP_DECODE_RPM_min_limit(data->message);
            myHV500.RPM_max_limit = MAP_DECODE_RPM_max_limit(data->message);
            myHV500.Power_limit = MAP_DECODE_Power_limit(data->message);
            myHV500.CAN_map_version = MAP_DECODE_CAN_map_version(data->message);
            break;

        case 0x61:
            TCU_Autonomous_ignition = data->message[2];
            TCU_Precharge_done = data->message[5];
            break;

        default:
            break;
    }
}

// #####################################################################
// AUTONOUOMOUS FILTER IDS

/*Receive*/
void can_filter_id_bus4(can_data_t* data) {
    switch (data->id) {
        case 0x500:
            RPM_TOJAL = MAP_DECODE_TOJAL_RPM(data->message);
            break;
        case 0x50:
            AS_Emergency = data->message[0];
            break;
        case 0x191:
            RES_AD_Ignition = data->message[0];
            // printf("Ignition: %d\n", AD_Ignition);
            break;
    }
}

/*Send*/
void can_bus_send_AdBus_RPM(uint32_t rpm) {
    can_data_t data;
    memset(data.message, 0x00, sizeof(data.message));
    data.id = 0x510;
    data.length = 8;
    rpm = rpm / 20;
    MAP_ENCODE_TOJAL_RPM(data.message, rpm);

    can_bus_send(CAN_BUS4, &data);
}

void can_open_init() {
    can_data_t data;

    data.id = NMT_ADDRESS_COBID();
    data.message[0] = 0x80;
    data.message[1] = 0x00;
    data.length = 2;
    can_bus_send(CAN_BUS4, &data);

    CORETIMER_DelayMs(10);

    data.id = NMT_ADDRESS_COBID();
    data.message[0] = 0x01;
    data.message[1] = 0x00;
    data.length = 2;
    can_bus_send(CAN_BUS4, &data);
}