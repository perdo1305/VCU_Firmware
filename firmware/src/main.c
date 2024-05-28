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

float PDM_Current = 0;

// ############# CANSART ######################################
#if CANSART
struct frame10 frames10;
struct frame11 frames11;
struct frame121 frames121;
#endif

// ############# ADC ########################################

uint8_t message_ADC[64];      // CAN message to send ADC data
__COHERENT uint16_t ADC[64];  // ADC raw data

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
// bool APPS_Function(uint16_t APPS1, uint16_t APPS2); // APPS function to calculate average and percentage

void startupSequence(void);    // Startup sequence
void PrintToConsole(uint8_t);  // Print data to console
bool IGNITION_R2D(void);
void MeasureCurrent(void);

#if CANSART
void CANSART_TASKS(void);
void CANSART_SETUP(void);
#endif

// ############# TMR FUNCTIONS ###############################

void TMR1_5ms(uint32_t status, uintptr_t context) {                                // 200Hz
    CAN_Send_VCU_Datadb_1(Current_Power, Target_Power, Brake_Pressure, Throttle);  // ID 0x20 to DATA_BUS
    CAN_Send_VCU_Datadb_4(RPM, Inverter_Voltage);                                  // ID 0x23 to DATA_BUS
    static uint8_t data[8] = {0};
    Send_CAN_BUS_2(0x20, data, 8);
    Send_CAN_BUS_2(0x21, data, 8);
    Send_CAN_BUS_2(0x22, data, 8);
    Send_CAN_BUS_2(0x23, data, 8);
}

void TMR2_100ms(uint32_t status, uintptr_t context) {                // 10Hz
    CAN_Send_VCU_Datadb_2(myHV500.Actual_TempMotor, myHV500.Actual_TempController);  // ID 0x21 to DATA_BUS
    CAN_Send_VCU_Datadb_3(VcuState, LMT2, LMT1, Inverter_Faults);    // ID 0x22 to DATA_BUS
    static uint8_t data[8] = {0};
    Send_CAN_BUS_3(0x23, data, 8);
    Send_CAN_BUS_3(0x24, data, 8);
}

void TMR4_500ms(uint32_t status, uintptr_t context) {  // 2Hz
    GPIO_RB1_ONBOARDLED_Toggle();                      // Heartbeat led
    GPIO_RA10_LED1_Toggle();
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

void ADCHS_CH0_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    static int samples[4] = {0};
    static int i = 0;

    samples[i] = ADCHS_ChannelResultGet(channel);
    i = (i + 1) % 4;

    int sum = 0;
    for (int j = 0; j < 4; j++) {
        sum += samples[j];
    }
    ADC[channel] = sum / 4;
}

void ADCHS_CH10_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    static int samples[4] = {0};
    static int i = 0;

    samples[i] = ADCHS_ChannelResultGet(channel);
    i = (i + 1) % 4;

    int sum = 0;
    for (int j = 0; j < 4; j++) {
        sum += samples[j];
    }
    ADC[channel] = sum / 4;
}

void ADCHS_CH15_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    static int samples[4] = {0};
    static int i = 0;

    samples[i] = ADCHS_ChannelResultGet(channel);
    i = (i + 1) % 4;

    int sum = 0;
    for (int j = 0; j < 4; j++) {
        sum += samples[j];
    }
    ADC[channel] = sum / 4;
}

int main(void) {
    /* Initialize all modules */
    SYS_Initialize(NULL);

    printf("\r\n------RESET------");

    ADCHS_ModulesEnable(ADCHS_MODULE4_MASK);
    ADCHS_ModulesEnable(ADCHS_MODULE7_MASK);

    ADCHS_CallbackRegister(ADCHS_CH0, ADCHS_CH0_Callback, (uintptr_t)NULL);    // APPS1 callback
    ADCHS_CallbackRegister(ADCHS_CH10, ADCHS_CH10_Callback, (uintptr_t)NULL);  // APPS2 callback
    ADCHS_CallbackRegister(ADCHS_CH15, ADCHS_CH15_Callback, (uintptr_t)NULL);  // APPS3 callback

    ADCHS_ChannelResultInterruptEnable(ADCHS_CH9);
    ADCHS_ChannelResultInterruptEnable(ADCHS_CH10);

    TMR1_CallbackRegister(TMR1_5ms, (uintptr_t)NULL);    // 200Hz
    TMR2_CallbackRegister(TMR2_100ms, (uintptr_t)NULL);  // 10Hz
    TMR4_CallbackRegister(TMR4_500ms, (uintptr_t)NULL);  // 2Hz heartbeat led
    TMR5_CallbackRegister(TMR5_100ms, (uintptr_t)NULL);  // 10Hz
    TMR6_CallbackRegister(TMR6_5ms, (uintptr_t)NULL);    // 200Hz to send data to the inverter

    fflush(stdout);

    APPS_Init(0.3, 3.0, 0.2);  // Initialize APPS

    startupSequence();  // led sequence
    VcuState = 1;       // Set VCU state to 1

    GPIO_RD5_Set();
    IGNITION_R2D();

    TMR1_Start();
    TMR2_Start();
    TMR3_Start();  // Used trigger source for ADC conversion
    TMR4_Start();
    TMR5_Start();
    TMR6_Start();

#if CANSART
    CANSART_SETUP();
#endif

    /*#############################################################################################################################*/
    /*######################################################### WHILE LOOP ########################################################*/
    /*#############################################################################################################################*/

    while (true) {
        if (!GPIO_RB5_IGN_Get()) {
            GPIO_RB10_LED5_Set();
        } else {
            GPIO_RB10_LED5_Clear();
        }

        SYS_Tasks();
        MeasureCurrent();

#if CANSART
        CANSART_TASKS();
#endif
        Read_CAN_BUS_1();  // Read DataBus
        Read_CAN_BUS_2();  // Read PowerTrainBus
        Read_CAN_BUS_3();  // Read AutonomousBus

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
        PrintToConsole(33);  // Print data to console time in ms
#endif
    }
    /* Execution should not come here during normal operation */
    return (EXIT_FAILURE);
}

void startupSequence() {
    /*LEDs Start up sequence*/
    GPIO_RA10_LED1_Set();
    CORETIMER_DelayMs(75);
    GPIO_RB13_LED2_Set();
    CORETIMER_DelayMs(75);
    GPIO_RB12_LED3_Set();
    CORETIMER_DelayMs(75);
    GPIO_RB11_LED4_Set();
    CORETIMER_DelayMs(75);
    GPIO_RB10_LED5_Set();
    CORETIMER_DelayMs(75);
    GPIO_RA10_LED1_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RB13_LED2_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RB12_LED3_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RB11_LED4_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RB10_LED5_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RA10_LED1_Set();
    CORETIMER_DelayMs(75);
    GPIO_RB13_LED2_Set();
    CORETIMER_DelayMs(75);
    GPIO_RB12_LED3_Set();
    CORETIMER_DelayMs(75);
    GPIO_RB11_LED4_Set();
    CORETIMER_DelayMs(75);
    GPIO_RB10_LED5_Set();
    CORETIMER_DelayMs(75);
    GPIO_RA10_LED1_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RB13_LED2_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RB12_LED3_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RB11_LED4_Clear();
    CORETIMER_DelayMs(75);
    GPIO_RB10_LED5_Clear();
    CORETIMER_DelayMs(75);
}

void PrintToConsole(uint8_t time) {
    // Print data-----
    currentMillis[3] = millis();
    if (currentMillis[3] - previousMillis[3] >= time) {
        // APPS_PrintValues();
        printf("APPSA%dAPPSB%dAPPST%dAPPS_ERROR%dAPPS_Perc%d", ADC[0], ADC[10], APPS_Mean, APPS_Error, APPS_Percentage);
        printf("APPS_MIN%dAPPS_MAX%dAPPS_TOL%d", APPS_MIN_bits, APPS_MAX_bits, APPS_Tolerance_bits);
        printf("CURRENT%fADC%d", PDM_Current, ADC[15]);
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

bool IGNITION_R2D(void) {
    GPIO_RD5_Clear();
    CORETIMER_DelayMs(10);
    GPIO_RD5_Set();
    CORETIMER_DelayMs(200);
    GPIO_RD5_Clear();
    CORETIMER_DelayMs(100);
    GPIO_RD5_Set();
    CORETIMER_DelayMs(100);
    GPIO_RD5_Clear();
    CORETIMER_DelayMs(1000);
    GPIO_RD5_Set();
    return true;
}

void MeasureCurrent(void) {
    static float voltsperamp = 0.0125;
    static uint8_t N = 3;
    static float volts = 0;
    static voltageDivider = 0.5;

    volts = (float)ADC[15]*3.3 / 4095.0;
    PDM_Current = (float) (volts/voltsperamp)/N;
}