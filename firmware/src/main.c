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
#include "../VCU2.0.X/TorqueControl.h"
// #include "../VCU_BANCADA.X/cansart_db_lc.h"
#include "../VCU2.0.X/VCU_config.h"
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

#define BUFFER_SIZE 20  // average to the adcs

// ############################################################
float PDM_Current = 0.0;  // current supplied by the low voltage battery
float PDM_Voltage = 0.0;  // voltage supplied by the low voltage battery

volatile bool IsAutonomous = false;  // when the car receives the autonomous signal, it will be set to true
bool AS_Emergency = false;
bool DrivingMode = false;  // 0 - manual mode, 1 - autonomous mode

HV500 myHV500;  // Stuct to store HV500 data comming from CAN

bool CANRX_ON[4] = {0, 0, 0, 0};       // array to store the status of the CAN RX comming from can_utils
bool CANTX_ON[4] = {0, 0, 0, 0};       // array to store the status of the CAN TX comming from can_utils
bool LED_CANRX_MODE = 0;               // variable to store the mode of the LED, if 1, the LED will blink when the CAN RX is on, if 0, the LED will blink when the CAN TX is on
bool BUZZER_ON = false;                // variable to enable or disable the buzzer
bool R2DS_as_played = false;           // variable to store if the ready to drive sound was played
bool AS_Emergency_as_played = false;   // variable to store if the AS Emergency sound was played
uint8_t RES_AD_Ignition;               // variable to store the ignition state comming from the AD
bool TCU_Autonomous_ignition = false;  // variable to store the ignition state comming from the TCU
uint8_t TCU_Precharge_done = false;    // variable to store the precharge state comming from the TCU
uint16_t Actual_InputVoltage = 0;      // variable to store the input voltage comming from the AD
uint32_t AD_timeout = 0;               // variable to store the timeout of the AD

bool GO_Signal = false;                // indicates if the car is ready to drive in autonomous mode
volatile bool IgnitionSwitch = false;  // indicates if the ignition switch is on

// R2D Struct
typedef struct {
    bool isR2D;
    bool R2D_last_state;
    bool R2DS_as_played;
} R2D_t;
R2D_t R2D;

// ############# CANSART ######################################
#if CANSART
struct frame10 frames10;
struct frame11 frames11;
struct frame121 frames121;
#endif

// ############# ADC ########################################
uint8_t message_ADC[64];      // CAN message to send ADC data
__COHERENT uint16_t ADC[64];  // ADC raw data
uint16_t ADC_Filtered_0 = 0;
uint16_t ADC_Filtered_3 = 0;

__COHERENT uint16_t adc[10][10];  // just to test dma
bool adc_flag[64];                // flag to indicate that the ADC data was updated
/*
The DMABL field can also be thought of as a  ??Left Shift Amount +1 ?? needed for the channel ID to create the
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

void MeasureCurrent(uint16_t channel);        // Measure current from a ADC channel
void MeasureVoltage(uint16_t channel);        // Measure voltage from a ADC channel
void MeasureBrakePressure(uint16_t channel);  // Measure brake pressure from a ADC channel
void CalculateMean(void);                     // Calculate a moving average of the ADC channels

void MissionEmergencyStop(void);

void UpdateR2DState(void);
void SOUND_R2DS(void);
void UpdateIgnitionState(void);
void AutonomousR2D(void);
void check_driving_mode(void);

void DIP_Switch(void);
void BlinkCANLED(void);

#if CANSART
void CANSART_TASKS(void);
void CANSART_SETUP(void);
#endif

// ############# TMR FUNCTIONS ###############################
void TMR1_5ms(uint32_t status, uintptr_t context) {  // 200Hz
    // APPS_Function(ADC[0], ADC[3]);
    APPS_Function(ADC_Filtered_0, ADC_Filtered_3);

    // run in 100Hz
    static uint8_t i = 0;
    if (i < 1) {
        i++;
    } else {
        i = 0;

        /*AUTONOMOUS MODE*/
        if (DrivingMode) {
            /*AUTONOMOUS MODE*/
            if (R2D.isR2D) {
                if (RPM_TOJAL > MAX_AD_RPM) {
                    RPM_TOJAL = MAX_AD_RPM;
                } else if (RPM_TOJAL < 0) {
                    RPM_TOJAL = 0;
                }
                // TODO if autonomous mode is on, start does not need to be activated with the brake
                can_bus_send_HV500_SetDriveEnable(1);
                can_bus_send_HV500_SetERPM(RPM_TOJAL * 10);
            }
        } else {
            /*MANUAL MODE*/
            if (R2D.isR2D) {
                can_bus_send_HV500_SetDriveEnable(1);
                can_bus_send_HV500_SetRelCurrent(APPS_Percentage_1000);
            } else {
                can_bus_send_HV500_SetDriveEnable(0);
            }
        }
    }
}

void TMR2_100ms(uint32_t status, uintptr_t context) {
    can_bus_send_databus_1(Current_Power, Target_Power, Brake_Pressure, Throttle);
    can_bus_send_databus_2(myHV500.Actual_TempMotor, myHV500.Actual_TempController);
    can_bus_send_databus_3(VcuState, LMT2, LMT1, Inverter_Faults);
    can_bus_send_databus_4(RPM, Inverter_Voltage);

    // can_bus_send_HV500_SetERPM((RPM_TOJAL * 20));
    
    can_data_t data;
    data.id = 0x23; //0b100011
    data.length = 8;
    for (int i = 0; i < 8; i++) {
        data.message[i] = 0;
    }
    data.message[4] = R2D.isR2D;
    data.message[5] = IgnitionSwitch;

    can_bus_send(CAN_BUS2, &data);
}

void TMR4_500ms(uint32_t status, uintptr_t context) {  // 2Hz
    GPIO_RC11_LED_HeartBeat_Toggle();

    if (R2D.isR2D) {
        GPIO_RB10_LED_Set();  // PCB LED
    } else {
        GPIO_RB10_LED_Clear();  // PCB LED
        // external pin on pwm12H
    }
}

/*R2D SOUND*/
void TMR5_1200ms(uint32_t status, uintptr_t context) {
    GPIO_RF0_pin_Set();
    TMR5_Stop();
}

/*AS Emergency*/
void TMR6_500ms(uint32_t status, uintptr_t context) {
    static uint8_t i = 0;
    if (i < 18) {
        GPIO_RF0_pin_Toggle();
        i++;
    } else {
        GPIO_RF0_pin_Set();
        TMR6_Stop();
    }
}

// ############# ADC CALLBACKS ###############################

void ADCHS_CH0_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    ADC[channel] = ADCHS_ChannelResultGet(channel);
    adc_flag[channel] = 1;
}

void ADCHS_CH3_Callback(ADCHS_CHANNEL_NUM channel, uintptr_t context) {
    ADC[channel] = ADCHS_ChannelResultGet(channel);
    adc_flag[channel] = 1;
}

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
    GPIO_RF0_pin_Set();  // pino buzzer

    printf("\r\n------RESET------");

    ADCHS_ModulesEnable(ADCHS_MODULE0_MASK);  // AN0
    ADCHS_ModulesEnable(ADCHS_MODULE3_MASK);  // AN3
    // ADCHS_ModulesEnable(ADCHS_MODULE4_MASK);  // AN9
    ADCHS_ModulesEnable(ADCHS_MODULE7_MASK);  // AN8, AN14, AN15

    // ADCHS_DMAResultBaseAddrSet((uint32_t) KVA_TO_PA(adc));

    ADCHS_CallbackRegister(ADCHS_CH0, ADCHS_CH0_Callback, (uintptr_t)NULL);  // APPS1
    ADCHS_CallbackRegister(ADCHS_CH3, ADCHS_CH3_Callback, (uintptr_t)NULL);  // APPS2

    ADCHS_CallbackRegister(ADCHS_CH8, ADCHS_CH8_Callback, (uintptr_t)NULL);  // Voltage Measurement
    // ADCHS_CallbackRegister(ADCHS_CH9, ADCHS_CH9_Callback, (uintptr_t)NULL);  // Current Measurement

    ADCHS_CallbackRegister(ADCHS_CH14, ADCHS_CH14_Callback, (uintptr_t)NULL);  // Brake Pressure
    // ADCHS_CallbackRegister(ADCHS_CH15, ADCHS_CH15_Callback, (uintptr_t) NULL); //extra ADC channel

    // ADCHS_ChannelResultInterruptEnable(ADCHS_CH0);
    // ADCHS_ChannelResultInterruptEnable(ADCHS_CH3);
    ADCHS_ChannelResultInterruptEnable(ADCHS_CH8);
    // ADCHS_ChannelResultInterruptEnable(ADCHS_CH9);

    TMR1_CallbackRegister(TMR1_5ms, (uintptr_t)NULL);     // 200Hz
    TMR2_CallbackRegister(TMR2_100ms, (uintptr_t)NULL);   // 10Hz
    TMR4_CallbackRegister(TMR4_500ms, (uintptr_t)NULL);   // 2Hz heartbeat led
    TMR5_CallbackRegister(TMR5_1200ms, (uintptr_t)NULL);  // USED for 3seg R2D SOUND
    TMR6_CallbackRegister(TMR6_500ms, (uintptr_t)NULL);   // 1.5s para AS Emergency

    fflush(stdout);

    APPS_Init(__APPS_MIN, __APPS_MAX, __APPS_TOLERANCE);  // Initialize APPS

    CORETIMER_DelayMs(1500);

    startupSequence();     // led sequence
    check_driving_mode();  // check the driving mode

    VcuState = 1;  // Set VCU state to 1

    TMR1_Start();
    TMR2_Start();
    TMR3_Start();  // Used trigger source for ADC conversion
    TMR4_Start();

    WDT_Enable();
    can_open_init();
    MCPWM_Start();

    // the car does not start until the start button is pressed a the brake is pressed
    /*
     do{
        WDT_Clear();
    }while( !GPIO_RB5_IGN_Get() || (Brake_Pressure>=100) );
    R2D.isR2D = true; // the car is ready to drive
    IGNITION_R2D(); //ready to drive sound
     */

#if CANSART
    CANSART_SETUP();
#endif

    /*#############################################################################################################################*/
    /*######################################################### WHILE LOOP ########################################################*/
    /*#############################################################################################################################*/

    while (true) {
        WDT_Clear();      // Clear the watchdog timer
        BlinkCANLED();    // blink the CAN LED according to the CAN RX and TX status
        DIP_Switch();     // check the DIP switch state
        CalculateMean();  // calculate the moving average of the APPS adc values

        SOUND_R2DS();            // ready to drive sound (1.2sec)
        MissionEmergencyStop();  // emergency stop sound (8sec)

        UpdateIgnitionState();  // read the ignition Switch
        UpdateR2DState();       // check if the car is ready to drive, autonomous and manual mode
        AutonomousR2D();

        MeasureCurrent(ADCHS_CH9);
        MeasureVoltage(ADCHS_CH8);
        MeasureBrakePressure(ADCHS_CH14);

        can_bus_read(CAN_BUS1);
        can_bus_read(CAN_BUS2);
        can_bus_read(CAN_BUS3);
        can_bus_read(CAN_BUS4);

#if !CANSART                 /*                                                     \
                         if (UART1_ReceiverIsReady()) {                             \
                             uint8_t data2[8];                                      \
                             UART1_Read(data2, 8);                                  \
                             // TODO need to do a little protection on this         \
                             // read until gets a \0                                \
                             float apps_min = ((data2[0] << 8) | data2[1]) / 10;    \
                             float apps_max = (data2[2] << 8) | data2[3] / 10;      \
                             float apps_error = (data2[4] << 8) | data2[5] / 10;    \
                                                                                  \ \
                             APPS_Init(apps_min, apps_max, apps_error);             \
                         }                                                          \
                                                                                  \ \
                                                                                  */
        PrintToConsole(50);  // Print data to console time in ms

#else
        CANSART_TASKS();
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
    CORETIMER_DelayMs(500);
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

        // calculate apps voltage
        float apps1_volts = (float)ADC[0] * 3.3 / 4095.0;
        float apps2_volts = (float)ADC[3] * 3.3 / 4095.0;

        printf("APPSA%dAPPSB%dAPPST%dAPPS_ERROR%dAPPS_Perc%d", ADC[0], ADC[3], APPS_Mean, APPS_Error, APPS_Percentage);
        printf("APPS1V%dAPPS2V%d", (uint16_t)(apps1_volts * 100), (uint16_t)(apps2_volts * 100));

        // printf("APPS_MIN%dAPPS_MAX%dAPPS_TOL%d", APPS_MIN_bits, APPS_MAX_bits, APPS_Tolerance_bits);

        printf("C1R%dC2R%dC3R%dC4R%d", CANRX_ON[CAN_BUS1], CANRX_ON[CAN_BUS2], CANRX_ON[CAN_BUS3], CANRX_ON[CAN_BUS4]);
        printf("C1T%dC2T%dC3T%dC4T%d", CANTX_ON[CAN_BUS1], CANTX_ON[CAN_BUS2], CANTX_ON[CAN_BUS3], CANTX_ON[CAN_BUS4]);

        printf("I%d", (uint16_t)(PDM_Current * 100));
        printf("V%d", (uint16_t)(PDM_Voltage * 100));
        printf("BP%d", Brake_Pressure);

        // Ready to drive & is autonomous

        printf("ERPM%ld", myHV500.Actual_ERPM);
        printf("Tmp_INV%d", myHV500.Actual_TempController / 10);
        printf("Tmp_MOT%d", myHV500.Actual_TempMotor / 10);
        printf("INVVTG%d", myHV500.Actual_InputVoltage);
        printf("ACC%d", myHV500.Actual_ACCurrent / 10);
        printf("DCC%d", myHV500.Actual_DCCurrent / 10);

        printf("RPM_TOJAL%d", RPM_TOJAL);

        printf("R2D%d", R2D.isR2D);
        printf("IGN%d", IgnitionSwitch);
        printf("AD%d", IsAutonomous);
        printf("TCU_PRECHARGE%d", TCU_Precharge_done);

        printf("AD_IGN%d", RES_AD_Ignition);
        printf("HV%d", Actual_InputVoltage);

        printf("DM%d", DrivingMode);

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
    if (PDM_Current < 0) {
        PDM_Current = 0;
    }
}

void MeasureVoltage(uint16_t channel) {
    PDM_Voltage = ((float)ADC[channel] * 3.30 / 4095.000) / 0.118;
}

void MissionEmergencyStop(void) {
    // TODO apitar buzzer quando receber o comando de emergencia

    /*
      The status  ??AS Emergency ?? has to be indicated by an intermittent sound with the following
    parameters:
     ?  on-/off-frequency: 1 Hz to 5 Hz
     ?  duty cycle 50 %
     ?  sound level between 80 dBA and 90 dBA, fast weighting in a radius of 2 m around the
    vehicle.
     ?  duration between 8 s and 10 s after entering  ??AS Emergency ??
     */

    if (AS_Emergency) {
        if (!AS_Emergency_as_played) {
            // MCPWM_Start();
            AS_Emergency_as_played = true;
            R2D.isR2D = false;
            TMR6_Start();
        }
    }
}

void UpdateR2DState(void) {
    static bool previous_state = false;
    static bool current_state = false;
    static uint32_t lastDebounceTime = 0;
    const uint16_t debounceDelay = 30;
    static bool start_button = false;
    /*Debounce*/
    // current_state = GPIO_RB6_START_BUTTON_Get();
    current_state = GPIO_RE14_R2D_BT_Get();

    if (current_state != previous_state) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
        start_button = current_state;
    }
    previous_state = current_state;

    // TODO VERIFICAR ISTO TUDO
    static bool hv_on = true;
    // bool hv_on = GPIO_RG9_Get();

    /*
    CORETIMER_DelayUs(2);
    bool start_button = GPIO_RB6_START_BUTTON_Get();
    CORETIMER_DelayUs(2);
    */

    if (DrivingMode) {
        /*AUTONOMOUS MODE*/
        if (IgnitionSwitch) {
            if (GO_Signal) {
                if (hv_on) {
                    R2D.isR2D = true;
                }
            }
        }
    } else {
        /*MANUAL MODE*/
        if (IgnitionSwitch) {
            static int brightness2000;
            static int brightness = 0;
            static int fadeAmount = 5;
            static uint8_t wait = 0;
            // if (hv_on) {
            if (start_button) {
                // if (GPIO_RB6_START_BUTTON_Get() && (Brake_Pressure >= __BRAKE_THRESHOLD)) {
                R2D.isR2D = true;
            }

            /* Fade R2D LED */
            static uint32_t previous_millis = 0;
            uint32_t milisegungos = millis();
            if (milisegungos - previous_millis >= 10) {
                if (!R2D.isR2D) {
                    // fade led

                    if (wait) {
                        static uint8_t i = 0;
                        MCPWM_ChannelPrimaryDutySet(MCPWM_CH_12, 2000);
                        if (i < 20) {
                            i++;
                        } else {
                            i = 0;
                            wait = 0;
                        }
                    } else {
                        brightness2000 = brightness * 7.843;

                        MCPWM_ChannelPrimaryDutySet(MCPWM_CH_12, brightness2000);
                        if (brightness == 250) {
                            wait = 1;
                        }
                        brightness = brightness + fadeAmount;

                        if (brightness == 0 || brightness == 250) {
                            fadeAmount = -fadeAmount;
                        }
                    }

                } else {
                    // MCPWM_Start();
                    MCPWM_ChannelPrimaryDutySet(MCPWM_CH_12, 0);
                }
                previous_millis = milisegungos;
            }

            // }
        } else {
            R2D.isR2D = false;
            R2D.R2DS_as_played = false;
            MCPWM_ChannelPrimaryDutySet(MCPWM_CH_12, 2000);
        }
    }
}

void SOUND_R2DS(void) {
    if (R2D.isR2D) {
        if (!BUZZER_ON) {
            if (!R2D.R2DS_as_played) {
                TMR5_Start();
                GPIO_RF0_pin_Clear();
                R2D.R2DS_as_played = true;
            }
        }
    }
}

/*(28.57mV/bar  + 500mv)*/
void MeasureBrakePressure(uint16_t channel) {
    static float volts = 0;
    static float pressure = 0;
    volts = (float)ADC[channel] * 3.300 / 4095.000;
    volts = volts / 0.667;  // conversao 3.3 para 5

    if (volts < 0) {
        volts = 0;
    } else if (volts > 5) {
        volts = 5;
    }

    if (volts < 0.5) {
        pressure = 0;
    } else if (volts > 3.0) {
        pressure = 50;
    }

    // pressure = (volts - 0.5) / 0.02857;

    Brake_Pressure = (uint8_t)pressure;
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

void DIP_Switch(void) {
    bool dipswitch[4];
    dipswitch[0] = GPIO_RB8_DIP1_Get();
    dipswitch[1] = GPIO_RC13_DIP2_Get();
    dipswitch[2] = GPIO_RB7_DIP3_Get();
    dipswitch[3] = GPIO_RC10_DIP4_Get();
    /*
        vou correr isto so uma vez ao inicio do codigo evita que se esteja sempre a verificar
        if (!dipswitch[3]) {
            // LED_CANRX_MODE = 1;  // Blick the LED to indicate that the CAN RX is on
            DrivingMode = true;  // Autonomous mode
        } else {
            DrivingMode = false;  // Manual mode
            // LED_CANRX_MODE = 0;
        }
        */
    if (!dipswitch[2]) {
        BUZZER_ON = true;
    } else {
        BUZZER_ON = false;
    }
    // printf("DIP1 %d DIP2 %d DIP3 %d DIP4 %d\r\n", dipswitch[0], dipswitch[1], dipswitch[2], dipswitch[3]);
}

/*calculate mean of adc channels*/
void CalculateMean() {
    // TODO CALCULATE MEAN OF ADC CHANNELS
    if (adc_flag[0]) {
        static uint16_t buffer[BUFFER_SIZE];
        static uint16_t sum = 0;
        static int oldestIndex = 0;

        sum -= buffer[oldestIndex];
        buffer[oldestIndex] = ADC[0];
        sum += buffer[oldestIndex];
        oldestIndex = (oldestIndex + 1) % BUFFER_SIZE;

        ADC_Filtered_0 = sum / BUFFER_SIZE;
        adc_flag[0] = 0;
    }

    if (adc_flag[3]) {
        static uint16_t buffer[BUFFER_SIZE];
        static uint16_t sum = 0;
        static int oldestIndex = 0;
        // Subtract the oldest value from the sum
        sum -= buffer[oldestIndex];
        // Overwrite the oldest value with the new one
        buffer[oldestIndex] = ADC[3];
        // Add the new value to the sum
        sum += buffer[oldestIndex];
        // Move the oldest index forward, wrapping around if necessary
        oldestIndex = (oldestIndex + 1) % BUFFER_SIZE;
        // Calculate the filtered ADC value
        ADC_Filtered_3 = sum / BUFFER_SIZE;
        adc_flag[3] = 0;
    }
}

/*CAN Status by blinking the LEDs*/
void BlinkCANLED(void) {
    if (LED_CANRX_MODE) {
        CANRX_ON[CAN_BUS1] == 1 ? GPIO_RA10_LED_CAN1_Toggle() : GPIO_RA10_LED_CAN1_Clear();
        CANRX_ON[CAN_BUS2] == 1 ? GPIO_RB13_LED_CAN2_Toggle() : GPIO_RB13_LED_CAN2_Clear();
        CANRX_ON[CAN_BUS3] == 1 ? GPIO_RB12_LED_CAN3_Toggle() : GPIO_RB12_LED_CAN3_Clear();
        CANRX_ON[CAN_BUS4] == 1 ? GPIO_RB11_LED_CAN4_Toggle() : GPIO_RB11_LED_CAN4_Clear();
    } else {
        CANTX_ON[CAN_BUS1] == 1 ? GPIO_RA10_LED_CAN1_Toggle() : GPIO_RA10_LED_CAN1_Clear();
        CANTX_ON[CAN_BUS2] == 1 ? GPIO_RB13_LED_CAN2_Toggle() : GPIO_RB13_LED_CAN2_Clear();
        CANTX_ON[CAN_BUS3] == 1 ? GPIO_RB12_LED_CAN3_Toggle() : GPIO_RB12_LED_CAN3_Clear();
        CANTX_ON[CAN_BUS4] == 1 ? GPIO_RB11_LED_CAN4_Toggle() : GPIO_RB11_LED_CAN4_Clear();
    }
}

/*Get the Ignition Switch Status*/
void UpdateIgnitionState(void) {
    static bool previous_state = false;
    static bool current_state = false;
    static uint32_t lastDebounceTime = 0;
    const uint16_t debounceDelay = 30;

    // current_state = GPIO_RD6_IGN_SWITCH_Get();
    current_state = !GPIO_RA8_IGN_SW_Get();

    if (current_state != previous_state) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        IgnitionSwitch = current_state;
    }

    previous_state = current_state;

    /*
    if (GPIO_RD6_IGN_SWITCH_Get()) {
        IgnitionSwitch = true;

        //data.id = 0x23;
        //data.message[5] = IgnitionSwitch;
        //can_bus_send(CAN_BUS2, &data);
    } else {
        IgnitionSwitch = false;
    }
    */
}

/*Activate R2D Autonomous mode by receiving the GO signal from the AD*/
void AutonomousR2D(void) {
    if (RES_AD_Ignition == 5 || RES_AD_Ignition == 7) {
        GO_Signal = true;
    }
}

void check_driving_mode(void) {
   //check the switch RB4 TWO times, if they are diferent check again
   do {
       DrivingMode = GPIO_RC10_DIP4_Get();
   } while (DrivingMode != GPIO_RC10_DIP4_Get());
}