

#include "CAN_utils.h"

// ############# RX CAN FRAME ###############################

#define CAN_BUS1 0U
#define CAN_BUS2 1U
#define CAN_BUS3 2U

CANFD_MSG_RX_ATTRIBUTE msgAttr = CANFD_MSG_RX_DATA_FRAME;  // RX message attribute

typedef struct {
    uint8_t message[8];
    uint32_t messageID;
    uint8_t messageLength;
} CAN_Buffer;

CAN_Buffer rx[3] = {
    {.message =
         {0},
     .messageID = 0,
     .messageLength = 0},
    {.message =
         {0},
     .messageID = 0,
     .messageLength = 0},
    {.message =
         {0},
     .messageID = 0,
     .messageLength = 0}};

CAN_Buffer tx[3] = {
    {.message =
         {0},
     .messageID = 0,
     .messageLength = 0},
    {.message =
         {0},
     .messageID = 0,
     .messageLength = 0},
    {.message =
         {0},
     .messageID = 0,
     .messageLength = 0}};

uint32_t status[3] = {0, 0, 0};  // CAN error status

// ############# TX CAN FRAME ###############################
uint8_t message_CAN_TX[8] = {};  // TX message buffer
uint8_t cantx_message[8] = {};   // TX message buffer

/*
█▀▀ ▄▀█ █▄░█ ▄█ ▄▄ █▀▄ ▄▀█ ▀█▀ ▄▀█ █▄▄ █░█ █▀   █▀▀ █▀█ █▄░█ █▀▀ █ █▀▀ █░█ █▀█ ▄▀█ ▀█▀ █ █▀█ █▄░█
█▄▄ █▀█ █░▀█ ░█ ░░ █▄▀ █▀█ ░█░ █▀█ █▄█ █▄█ ▄█   █▄▄ █▄█ █░▀█ █▀░ █ █▄█ █▄█ █▀▄ █▀█ ░█░ █ █▄█ █░▀█
 */
void Read_CAN_BUS_1() {
    uint8_t Z = CAN_BUS1;
    status[Z] = CAN1_ErrorGet();
    if (status[Z] == CANFD_ERROR_NONE) {
        memset(rx[Z].message, 0x00, sizeof(rx[Z].message));
        if (CAN1_MessageReceive(&rx[Z].messageID, &rx[Z].messageLength, rx[Z].message, 0, 2, &msgAttr)) {
            CANRX_ON[Z] = 1;
            CAN_Filter_IDS_BUS1(rx[Z].messageID);
            GPIO_RA10_LED_CAN1_Toggle();
        }
    } else {
        GPIO_RA10_LED_CAN1_Clear();
        CANRX_ON[Z] = 0;
    }
}

void Send_CAN_BUS_1(uint32_t id, uint8_t* message, uint8_t size) {
    uint8_t Z = CAN_BUS1;

    if (CAN1_TxFIFOQueueIsFull(0)) {
        CANTX_ON[Z] = 0;
    } else {
        if (CAN1_MessageTransmit(id, size, message, 0, CANFD_MODE_NORMAL, CANFD_MSG_TX_DATA_FRAME)) {
            CANTX_ON[Z] = 1;

        } else {
            CANTX_ON[Z] = 0;
        }
    }
}

/*
█▀▀ ▄▀█ █▄░█ ▀█ ▄▄ █▀█ █▀█ █░█░█ █▀▀ █▀█ ▀█▀ █▀█ ▄▀█ █ █▄░█   █▀▀ █▀█ █▄░█ █▀▀ █ █▀▀ █░█ █▀█ ▄▀█ ▀█▀ █ █▀█ █▄░█
█▄▄ █▀█ █░▀█ █▄ ░░ █▀▀ █▄█ ▀▄▀▄▀ ██▄ █▀▄ ░█░ █▀▄ █▀█ █ █░▀█   █▄▄ █▄█ █░▀█ █▀░ █ █▄█ █▄█ █▀▄ █▀█ ░█░ █ █▄█ █░▀█
 */

void Read_CAN_BUS_2() {
    uint8_t Z = CAN_BUS2;
    status[Z] = CAN2_ErrorGet();
    if (status[Z] == CANFD_ERROR_NONE) {
        memset(rx[Z].message, 0x00, sizeof(rx[Z].message));
        if (CAN2_MessageReceive(&rx[Z].messageID, &rx[Z].messageLength, rx[Z].message, 0, 2, &msgAttr)) {
            CANRX_ON[Z] = 1;
            CAN_Filter_IDS_BUS2(rx[Z].messageID);
            GPIO_RB13_LED_CAN2_Toggle();
        }
    } else {
        GPIO_RB13_LED_CAN2_Clear();
        CANRX_ON[Z] = 0;
    }
}

void Send_CAN_BUS_2(uint32_t id, uint8_t* message, uint8_t size) {
    uint8_t Z = CAN_BUS2;
    if (CAN2_TxFIFOQueueIsFull(0)) {
        CANTX_ON[Z] = 0;
    } else {
        if (CAN2_MessageTransmit(id, size, message, 0, CANFD_MODE_FD_WITH_BRS, CANFD_MSG_TX_DATA_FRAME)) {
            // if (CAN2_MessageTransmit(id, size, message, 0, CANFD_MODE_NORMAL, CANFD_MSG_TX_DATA_FRAME)) {
            CANTX_ON[Z] = 1;
            // GPIO_RB10_LED5_Toggle();
        } else {
            CANTX_ON[Z] = 0;
            // GPIO_RB10_LED5_Clear();
        }
    }
}

void Read_CAN_BUS_3() {
    uint8_t Z = CAN_BUS3;
    status[Z] = CAN3_ErrorGet();
    if (status[Z] == CANFD_ERROR_NONE) {
        memset(rx[Z].message, 0x00, sizeof(rx[Z].message));
        if (CAN3_MessageReceive(&rx[Z].messageID, &rx[Z].messageLength, rx[Z].message, 0, 2, &msgAttr)) {
            CANRX_ON[Z] = 1;
            // CAN_Filter_IDS_BUS2(rx[Z].messageID);
            GPIO_RB12_LED_CAN3_Toggle();
        }
    } else {
        GPIO_RB12_LED_CAN3_Clear();
        CANRX_ON[Z] = 0;
    }
}

void Send_CAN_BUS_3(uint32_t id, uint8_t* message, uint8_t size) {
    uint8_t Z = CAN_BUS3;
    if (CAN3_TxFIFOQueueIsFull(0)) {
        CANTX_ON[Z] = 0;
    } else {
        if (CAN3_MessageTransmit(id, size, message, 0, CANFD_MODE_NORMAL, CANFD_MSG_TX_DATA_FRAME)) {
            // if (CAN3_MessageTransmit(id, size, message, 0, CANFD_MODE_NORMAL, CANFD_MSG_TX_DATA_FRAME)) {
            CANTX_ON[Z] = 1;
            // GPIO_RB10_LED5_Toggle();
        } else {
            CANTX_ON[Z] = 0;
            // GPIO_RB10_LED5_Clear();
        }
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
    MAP_ENCODE_CONSUMED_POWER(tx[CAN_BUS1].message, consumed_power);
    MAP_ENCODE_TARGET_POWER(tx[CAN_BUS1].message, target_power);
    MAP_ENCODE_BRAKE_PRESSURE(tx[CAN_BUS1].message, brake_pressure);
    MAP_ENCODE_THROTTLE_POSITION(tx[CAN_BUS1].message, throttle_position);

    Send_CAN_BUS_1(CAN_VCU_ID_1, tx[CAN_BUS1].message, 8);
}

void CAN_Send_VCU_Datadb_2(uint16_t motor_temperature, uint16_t inverter_temperature) {
    MAP_ENCODE_MOTOR_TEMPERATURE(tx[CAN_BUS1].message, motor_temperature);
    MAP_ENCODE_INVERTER_TEMPERATURE(tx[CAN_BUS1].message, inverter_temperature);

    Send_CAN_BUS_1(CAN_VCU_ID_2, tx[CAN_BUS1].message, 8);
}

void CAN_Send_VCU_Datadb_3(uint8_t vcu_state, uint8_t lmt2, uint8_t lmt1, uint16_t inverter_error) {
    MAP_ENCODE_VCU_STATE(tx[CAN_BUS1].message, vcu_state);
    MAP_ENCODE_LMT2(tx[CAN_BUS1].message, lmt2);
    MAP_ENCODE_LMT1(tx[CAN_BUS1].message, lmt1);

    MAP_ENCODE_INVERTER_ERROR(tx[CAN_BUS1].message, inverter_error);

    Send_CAN_BUS_1(CAN_VCU_ID_3, tx[CAN_BUS1].message, 8);
}

void CAN_Send_VCU_Datadb_4(uint16_t rpm, uint16_t inverter_voltage) {
    MAP_ENCODE_RPM(tx[CAN_BUS1].message, rpm);
    MAP_ENCODE_INVERTER_VOLTAGE(tx[CAN_BUS1].message, inverter_voltage);

    Send_CAN_BUS_1(CAN_VCU_ID_4, tx[CAN_BUS1].message, 8);
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

void CAN_Filter_IDS_BUS1(uint32_t id) {
    switch (id) {
        case CAN_PDM_ID_1:
            break;
        case CAN_PDM_ID_2:
            break;
        case CAN_PDM_ID_3:
            break;
        case CAN_PDM_ID_4:
            break;
        case CAN_IMU_ID_1:
            break;
        case CAN_IMU_ID_2:
            break;
        case CAN_DYNAMICS_REAR_ID_1:
            break;
        case CAN_DYNAMICS_REAR_ID_2:
            break;
        default:
            break;
    }
}

// ############# PWTDB CAN VARS ###################################

// Receive

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
}

// send

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
}
// #####################################################################
