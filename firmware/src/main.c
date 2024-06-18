/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "../VCU2.0.X/main.h"

#include <stdbool.h>  // Defines true
#include <stddef.h>   // Defines NULL
#include <stdlib.h>   // Defines EXIT_FAILURE

// #include "../../../CANSART/Controller Library/PIC32/Library/cansart.h"
#include "../VCU2.0.X/CAN_utils.h"
// #include "../VCU_BANCADA.X/cansart_db_lc.h"
#include "../VCU2.0.X/utils.h"

#define AUTONOMOUS_MODE 0U
#define MANUAL_MODE 1U

#define DRIVING_MODE MANUAL_MODE

// Define a macro DEBUG para ativar ou desativar o debug_printf
#define DEBUG 0
#define LABVIEW 0
#define INVERTED_APPS 0
#define CANSART 0

// Define o debug_printf para ser ativado ou desativado com base na macro DEBUG
#if DEBUG
#define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...)
#endif

#if LABVIEW
#define labview_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define labview_printf(fmt, ...)
#endif

// ############################################################
float PDM_Current = 0.0;  // current supplied by the low voltage battery
float PDM_Voltage = 0.0;  // voltage supplied by the low voltage battery

HV500 myHV500;
bool CANRX_ON[3] = {0, 0, 0};
bool CANTX_ON[3] = {0, 0, 0};

bool Ready2Drive = 0;  // indicates if the car is ready to drive

// ############# CANSART ######################################
#if CANSART
struct frame10 frames10;
struct frame11 frames11;
struct frame121 frames121;
#endif

// ############# ADC ########################################
uint8_t message_ADC[64];      // CAN message to send ADC data
__COHERENT uint16_t ADC[64];  // ADC raw data
bool adc_flag[64] = {0};      // ADC flag

__COHERENT uint16_t adc[10][10];
/*
The DMABL field can also be thought of as a “Left Shift Amount +1” needed for the channel ID to create the
DMA byte address offset to be added to the contents of ADDMAB in order to obtain the byte address of the
beginning of the System RAM buffer area allocated for the given channel.
*/
// ############# MILIS #######################################
unsigned int previousMillis[10] = {};
unsigned int currentMillis[10] = {};

// ############# MILIS #######################################
unsigned int millis() {
    return (unsigned int)(CORETIMER_CounterGet() / (CORE_TIMER_FREQUENCY / 1000));
}

// ############# VCU DATABUS VARS ############################
/* ID 0x20 */
uint8_t Throttle = 0;        // 0-100   |Byte 0
uint8_t Brake_Pressure = 0;  // 0-50    |Byte 1
uint32_t Target_Power = 0;   // 0-85000 |Byte 2-4
uint32_t Current_Power = 0;  // 0-85000 |Byte 5-7
/* ID 0x21 */
uint16_t Inverter_Temperature = 0;  // 0-300  |Byte 0-12
uint16_t Motor_Temperature = 0;     // 0 -100 |Byte 2-3
/* ID 0x22 */
uint16_t Inverter_Faults = 0;  // 0-65535  |Byte 0-1
uint8_t LMT1 = 0;              // 0-255    |Byte 2
uint8_t LMT2 = 0;              // 0-255    |Byte 3
uint8_t VcuState = 0;          // 0-255    |Byte 4
/* ID 0x23 */
uint16_t Inverter_Voltage = 0;  // 0-65535 |Byte 0-1
uint16_t RPM = 0;               // 0-65535 |Byte 2-32

// ############# FUNCTIONS ##################################
void startupSequence(void);    // Startup sequence
void PrintToConsole(uint8_t);  // Print data to console

void MeasureCurrent(uint16_t channel);
void MeasureVoltage(uint16_t channel);
void MeasureBrakePressure(uint16_t channel);

void MissionEmergencyStop(void);
bool AS_Emergency = false;
void IsR2D(void);
void SOUND_R2DS(void);

#if CANSART
void CANSART_TASKS(void);
void CANSART_SETUP(void);
#endif

// ############# TMR FUNCTIONS ###############################
void TMR1_5ms(uint32_t status, uintptr_t context) {                                // 200Hz
    CAN_Send_VCU_Datadb_1(Current_Power, Target_Power, Brake_Pressure, Throttle);  // ID 0x20 to DATA_BUS
    CAN_Send_VCU_Datadb_4(RPM, Inverter_Voltage);
}

void TMR2_100ms(uint32_t status, uintptr_t context) {                                // 10Hz
    CAN_Send_VCU_Datadb_2(myHV500.Actual_TempMotor, myHV500.Actual_TempController);  // ID 0x21 to DATA_BUS
    CAN_Send_VCU_Datadb_3(VcuState, LMT2, LMT1, Inverter_Faults);                    // ID 0x22 to DATA_BUS
}

void TMR4_500ms(uint32_t status, uintptr_t context) {  // 2Hz
    GPIO_RC11_LED_HeartBeat_Toggle();
}

void TMR5_100ms(uint32_t status, uintptr_t context) {
    // apps_error = APPS_Function(ADC[0], ADC[3]);  // checks if there is an error in the APPS and calculates the average and percentage
}

void TMR6_5ms(uint32_t status, uintptr_t context) {
    APPS_Function(ADC[0], ADC[10]);
    // setSetERPM(250 * APPS_Percentage);  // Send APPS_percent to inverter
    // setSetERPM(25 * APPS_Percentage_1000);

    // setDriveEnable(1);
}

// ############# ADC CALLBACKS ###############################
/*
void ADCHS_CH0_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    ADC[channel] = ADCHS_ChannelResultGet(channel);
    adc_flag[channel] = 1;
}
*/
/*
void ADCHS_CH3_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    ADC[channel] = ADCHS_ChannelResultGet(channel);
    adc_flag[channel] = 1;
}
*/

void ADCHS_CH8_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    ADC[channel] = ADCHS_ChannelResultGet(channel);
    adc_flag[channel] = 1;
}

void ADCHS_CH9_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    ADC[channel] = ADCHS_ChannelResultGet(channel);
    adc_flag[channel] = 1;
}

void ADCHS_CH14_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    ADC[channel] = ADCHS_ChannelResultGet(channel);
    adc_flag[channel] = 1;
}

/*#############################################################################################################################*/
/*######################################################### SETUP #############################################################*/
/*#############################################################################################################################*/
int main(void) {
    /* Initialize all modules */
    SYS_Initialize(NULL);

    printf("\r\n------RESET------");

    ADCHS_ModulesEnable(ADCHS_MODULE0_MASK);  // AN0
    ADCHS_ModulesEnable(ADCHS_MODULE3_MASK);  // AN3
    //ADCHS_ModulesEnable(ADCHS_MODULE4_MASK);  // AN9
    ADCHS_ModulesEnable(ADCHS_MODULE7_MASK);  // AN8, AN14, AN15

    ADCHS_DMAResultBaseAddrSet((uint32_t) KVA_TO_PA(adc));
    
    //ADCHS_CallbackRegister(ADCHS_CH0, ADCHS_CH0_Callback, (uintptr_t)NULL);  // APPS1
    //ADCHS_CallbackRegister(ADCHS_CH3, ADCHS_CH3_Callback, (uintptr_t)NULL);  // APPS2

    ADCHS_CallbackRegister(ADCHS_CH8, ADCHS_CH8_Callback, (uintptr_t)NULL);  // Voltage Measurement
    //ADCHS_CallbackRegister(ADCHS_CH9, ADCHS_CH9_Callback, (uintptr_t)NULL);  // Current Measurement

    ADCHS_CallbackRegister(ADCHS_CH14, ADCHS_CH14_Callback, (uintptr_t)NULL);  // Brake Pressure
    // ADCHS_CallbackRegister(ADCHS_CH15, ADCHS_CH15_Callback, (uintptr_t) NULL); //extra ADC channel

    //ADCHS_ChannelResultInterruptEnable(ADCHS_CH0);
    //ADCHS_ChannelResultInterruptEnable(ADCHS_CH3);
    ADCHS_ChannelResultInterruptEnable(ADCHS_CH8);
    //ADCHS_ChannelResultInterruptEnable(ADCHS_CH9);

    TMR1_CallbackRegister(TMR1_5ms, (uintptr_t)NULL);    // 200Hz
    TMR2_CallbackRegister(TMR2_100ms, (uintptr_t)NULL);  // 10Hz
    TMR4_CallbackRegister(TMR4_500ms, (uintptr_t)NULL);  // 2Hz heartbeat led
    TMR5_CallbackRegister(TMR5_100ms, (uintptr_t)NULL);  // 10Hz
    TMR6_CallbackRegister(TMR6_5ms, (uintptr_t)NULL);    // 200Hz to send data to the inverter

    fflush(stdout);

    APPS_Init(0.3, 3.0, 0.2);  // Initialize APPS

    startupSequence();  // led sequence
    VcuState = 1;       // Set VCU state to 1

    TMR1_Start();
    TMR2_Start();
    TMR3_Start();  // Used trigger source for ADC conversion
    TMR4_Start();
    TMR5_Start();
    TMR6_Start();

    WDT_Enable();

    // the car does not start until the start button is pressed a the brake is pressed
    /*
     do{
        WDT_Clear();
    }while( !GPIO_RB5_IGN_Get() || (Brake_Pressure>=100) );
    Ready2Drive = true; // the car is ready to drive
    IGNITION_R2D(); //ready to drive sound
     */

#if CANSART
    CANSART_SETUP();
#endif

    /*#############################################################################################################################*/
    /*######################################################### WHILE LOOP ########################################################*/
    /*#############################################################################################################################*/

    while (true) {
        WDT_Clear();

        IsR2D();
        SOUND_R2DS();            // ready to drive sound (3sec)
        MissionEmergencyStop();  // emergency stop sound (8sec)

        MeasureCurrent(ADCHS_CH9);
        MeasureVoltage(ADCHS_CH8);
        MeasureBrakePressure(ADCHS_CH14);

#if CANSART
        CANSART_TASKS();
#endif
        can_bus_read(CAN_BUS1);
        can_bus_read(CAN_BUS2);
        can_bus_read(CAN_BUS3);
#if !CANSART
        if (UART1_ReceiverIsReady()) {
            uint8_t data2[8];
            UART1_Read(data2, 8);
            // TODO need to do a little protection on this
            // read until gets a \0
            float apps_min = ((data2[0] << 8) | data2[1]) / 10;
            float apps_max = (data2[2] << 8) | data2[3] / 10;
            float apps_error = (data2[4] << 8) | data2[5] / 10;

            APPS_Init(apps_min, apps_max, apps_error);
        }

        PrintToConsole(50);  // Print data to console time in ms
#endif
        SYS_Tasks();
    }
    /* Execution should not come here during normal operation */
    return (EXIT_FAILURE);
}

void startupSequence() {
    /*LEDs Start up sequence*/
    GPIO_RA10_LED_CAN1_Set();
    GPIO_RB13_LED_CAN2_Set();
    GPIO_RB12_LED_CAN3_Set();
    GPIO_RB11_LED_CAN4_Set();
    GPIO_RC11_LED_HeartBeat_Set();
    GPIO_RB10_LED_Set();
    GPIO_RF1_LED_Set();
    CORETIMER_DelayMs(250);
    GPIO_RA10_LED_CAN1_Clear();
    GPIO_RB13_LED_CAN2_Clear();
    GPIO_RB12_LED_CAN3_Clear();
    GPIO_RB11_LED_CAN4_Clear();
    GPIO_RC11_LED_HeartBeat_Clear();
    GPIO_RB10_LED_Clear();
    GPIO_RF1_LED_Clear();
}

void PrintToConsole(uint8_t time) {
    // Print data-----
    currentMillis[3] = millis();
    if (currentMillis[3] - previousMillis[3] >= time) {
        // APPS_PrintValues();

        printf("APPSA%dAPPSB%dAPPST%dAPPS_ERROR%dAPPS_Perc%d", ADC[0], ADC[10], APPS_Mean, APPS_Error, APPS_Percentage);
        printf("APPS_MIN%dAPPS_MAX%dAPPS_TOL%d", APPS_MIN_bits, APPS_MAX_bits, APPS_Tolerance_bits);
        printf("I%f", PDM_Current);
        printf("ACtualDUTTY%d", myHV500.Actual_Duty);
        printf("C1R%dC2R%dC3R%d", CANRX_ON[1], CANRX_ON[2], CANRX_ON[3]);
        printf("C1T%dC2T%dC3T%d", CANTX_ON[1], CANTX_ON[2], CANTX_ON[3]);
        printf("\r\n");

        previousMillis[3] = currentMillis[3];
    }
}

#if CANSART

void CANSART_SETUP() {
    frames10.ID = 10;
    frames10.LENGHT = 8;

    frames11.ID = 11;
    frames11.LENGHT = 8;

    frames121.ID = 121;
    frames121.LENGHT = 8;
}

void CANSART_TASKS() {
    frames10.DATA1 = ADC[0] >> 8;
    frames10.DATA2 = ADC[0];
    frames10.DATA3 = ADC[3] >> 8;
    frames10.DATA4 = ADC[3];
    frames10.DATA5 = APPS_Mean >> 8;
    frames10.DATA6 = APPS_Mean;
    frames10.DATA7 = APPS_Error;
    frames10.DATA8 = APPS_Percentage;

    frames11.DATA1 = APPS_MIN_bits >> 8;
    frames11.DATA2 = APPS_MIN_bits;
    frames11.DATA3 = APPS_MAX_bits >> 8;
    frames11.DATA4 = APPS_MAX_bits;
    frames11.DATA5 = APPS_Tolerance_bits >> 8;
    frames11.DATA6 = APPS_Tolerance_bits;
    frames11.DATA7 = status1 >> 8;
    frames11.DATA8 = status1;

    float apps_min = (frames121.DATA1) / 100;
    float apps_max = (frames121.DATA2) / 100;
    float apps_error = (frames121.DATA3) / 100;

    APPS_Init(apps_min, apps_max, apps_error);

    updateDB(&frames10);
    updateDB(&frames11);
    updateDB(&frames121);
}
#endif

void MeasureCurrent(uint16_t channel) {
    static float voltsperamp = 0.0125;
    static uint8_t N = 5;
    static float volts = 0;
    // static float voltageDivider = 0.5;

    volts = (float)ADC[channel] * 3.300 / 4095.000;
    volts = volts - 2.5;
    PDM_Current = (float)(volts / voltsperamp) / N;
}

void MeasureVoltage(uint16_t channel) {
    PDM_Voltage = (float)ADC[channel] * 3.300 / 4095.000;
}

void MissionEmergencyStop(void) {
    // TODO apitar buzzer quando receber o comando de emergencia

    /*
      The status “AS Emergency” has to be indicated by an intermittent sound with the following
    parameters:
    • on-/off-frequency: 1 Hz to 5 Hz
    • duty cycle 50 %
    • sound level between 80 dBA and 90 dBA, fast weighting in a radius of 2 m around the
    vehicle.
    • duration between 8 s and 10 s after entering “AS Emergency”
     */

    if (AS_Emergency) {
        static bool buzzer_as_played = false;
        if (!buzzer_as_played) {
            //MCPWM_Start();
            static unsigned int previousMillis = 0;
            unsigned int currentMillis = millis();
            if (currentMillis - previousMillis >= 8000) {
                previousMillis = currentMillis;
                buzzer_as_played = true;
            }
        } else {
           // MCPWM_Stop();
        }
    }
}

void IsR2D(void) {
    if (!GPIO_RB5_START_BUTTON_Get() || (Brake_Pressure >= 100)) {
        Ready2Drive = true;  // the car is ready to drive
    }
}

void SOUND_R2DS(void) {
    if (Ready2Drive) {
        static bool R2DS_as_played = false;
        static unsigned int previousMillis = 0;
        static unsigned int currentMillis = 0;

        if (!R2DS_as_played) {
            currentMillis = millis();
           // MCPWM_Start();
            if (currentMillis - previousMillis >= 3000) {
                previousMillis = currentMillis;
                R2DS_as_played = true;
                //MCPWM_Stop();
            }
        }
    }
}

/*(28.57mV/bar  + 500mv)*/
void MeasureBrakePressure(uint16_t channel) {
    static float volts = 0;
    static float pressure = 0;
    volts = (float)ADC[channel] * 3.300 / 4095.000;
    pressure = (volts - 0.5) / 0.02857;
    Brake_Pressure = (uint8_t)pressure;
}

void Is_Autonomous(void) {
    if (DRIVING_MODE == AUTONOMOUS_MODE) {
        // TODO read TCU msg to switch between manual and autonomous mode
    }
}

// when received AT command to autocalibrate APPS
/*
void Autocalibrate(void) {
    static bool autocalibrate = false;
    if (UART1_ReceiverIsReady()) {
        uint8_t byte;
        byte = UART1_ReadByte();
        UART1_WriteByte(byte);
        if (byte == 'A') {
            autocalibrate = true;
        } else if (byte == 'B') {
            autocalibrate = false;
        }
    }
    if (autocalibrate) {
        // APPS_Autocalibrate(ADC[0], ADC[3]);
    }
}
*/

