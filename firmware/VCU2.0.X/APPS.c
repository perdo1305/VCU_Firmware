
#include "APPS.h"

#include <stdbool.h>  // Defines true
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

float APPS_MIN_Volts = 0.0;  // Minimum voltage of the APPS
uint16_t APPS_MIN_bits = 0;  // Minimum bits of the APPS

float APPS_MAX_Volts = 0.0;  // Maximum voltage of the APPS
uint16_t APPS_MAX_bits = 0;  // Maximum bits of the APPS

float APPS_Tolerance_Volts = 0.0;  // Tolerance in volts of the APPS
uint16_t APPS_Tolerance_bits = 0;  // Tolerance in bits of the APPS

uint16_t APPS_Bit_Resolution = 4095;  // 12 bits
float APPS_Voltage = 3.3;             // power supply voltage

uint16_t APPS_Delta = 0;  // Delta value of the APPS

uint16_t APPS1 = 0;                   // Value of the APPS1
uint16_t APPS2 = 0;                   // Value of the APPS2
uint16_t APPS_Mean = 0;               // Mean value of the APPS
uint16_t APPS_Percentage = 0;         // Percentage of the APPS 0-100
uint16_t APPS_Percentage_1000 = 0;    // Value of the APPS 0-1000
uint16_t APPS_functional_region = 0;  // Range of the APPS ((max - tolerance) - (min + tolerance))

bool APPS_Error = false;  // Error of the APPS

void APPS_CalculateFunctionalRegion(void);
uint16_t APPS_InvertValue(uint16_t apps2);
uint16_t APPS_VoltsToBits(float volts);
uint16_t APPS_MeanValue(uint16_t apps1, uint16_t apps2);
bool APPS_IsInTolerance(uint16_t value);
void APPS_UpdateAPPS1(uint16_t apps1);
void APPS_UpdateAPPS2(uint16_t apps2);
bool APPS_Is10PercentApart(uint16_t apps1, uint16_t apps2);
bool APPS_CheckError(uint16_t apps1, uint16_t apps2);
uint16_t APPS_ToPercentage(uint16_t apps_mean);
uint16_t APPS_ToPercentage_1000(uint16_t apps_mean);

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/// @brief Update the APPS values
/// @param min_volts minimum voltage that the APPS can reach
/// @param max_volts maximum voltage that the APPS can reach
/// @param APPS_Tolerance_Volts tolerance constante to add in the min and max values

void APPS_Init(float min_volts, float max_volts, float APPS_Tolerance_Volts, uint16_t APPS_delta) {
    APPS_MIN_bits = APPS_VoltsToBits(min_volts);
    APPS_MAX_bits = APPS_VoltsToBits(max_volts);
    APPS_Tolerance_bits = APPS_VoltsToBits(APPS_Tolerance_Volts);
    APPS_Delta = APPS_delta;
    APPS_CalculateFunctionalRegion();
}

/// @brief Calculate the functional region of the APPS
/// @param void

void APPS_CalculateFunctionalRegion(void) {
    APPS_functional_region = ((APPS_MAX_bits - APPS_Tolerance_bits) - (APPS_MIN_bits + APPS_Tolerance_bits));
}

/// @brief Invert the value of the APPS, because the APPS2 is inverted
/// @param apps2 value of the APPS2 to be inverted
/// @return the inverted value of the APPS2

uint16_t APPS_InvertValue(uint16_t apps2) {
    return APPS_Bit_Resolution - apps2;
}

/// @brief convert the voltage to bits based on the resolution of the ADC and the power supply voltage
/// @param volts voltage to be converted to bits
/// @return the value in bits

uint16_t APPS_VoltsToBits(float volts) {
    return ((volts * APPS_Bit_Resolution) / APPS_Voltage);
}

/// @brief Calculate the mean value of the APPS (apps2 needs to be inverted to be compared with apps1)
/// @param apps1 app1 sensor value
/// @param apps2 app2 sensor value
/// @return mean value of the APPS sensors

uint16_t APPS_MeanValue(uint16_t apps1, uint16_t apps2) {
    return (apps1 + apps2) / 2;
}

/// @brief check if the value is in the tolerance range, between the min and max values + the tolerance
/// @param value value to be checked (bits)
/// @return true if the value is in the tolerance range, false otherwise

bool APPS_IsInTolerance(uint16_t value) {
    return (value >= (APPS_MIN_bits - APPS_Tolerance_bits)) && (value <= (APPS_MAX_bits + APPS_Tolerance_bits));
}

/// @brief Update the APPS1 value
/// @param apps1 value of the APPS1

void APPS_UpdateAPPS1(uint16_t apps1) {
    APPS1 = apps1;
}

/// @brief Update the APPS2 value
/// @param apps2 value of the APPS2

void APPS_UpdateAPPS2(uint16_t apps2) {
    // APPS2 = APPS_InvertValue(apps2);
    APPS2 = apps2 - APPS_Delta;
}

/// @brief Check if the APPS is 10% apart
/// @param apps1 value of the APPS1
/// @param apps2 value of the APPS2
/// @return true if the APPS is 10% apart, false otherwise

bool APPS_Is10PercentApart(uint16_t apps1, uint16_t apps2) {
    // uint16_t total = apps1 + apps2;
    // return (total <= 3686.4 || total >= 4505.6);

    return (apps1 < (apps2 * 0.9)) || (apps1 > (apps2 * 1.1));
}

/// @brief identify if there is a short circuit in the APPS to gnd or vcc
/// @param apps1
/// @param apps2
/// @return
bool APPS_ShortCircuit(uint16_t apps1, uint16_t apps2) {
    return (apps1 < 100) || (apps2 < 100) || (apps1 > 4000) || (apps2 > 4000);
}

/// @brief identify if the APPS are shorted together
/// @param apps1
/// @param apps2
/// @return
bool APPS_ShortedTogether(uint16_t apps1, uint16_t apps2) {
    return ((abs(apps1 - (apps2 + APPS_Delta))) < 20);
}

/// @brief Check if the APPS has an error based on the 10% apart and the tolerance
/// @param apps1 value of the APPS1
/// @param apps2 value of the APPS2
/// @return true if the values are 10% apart or not in the tolerance range, false otherwise

bool APPS_CheckError(uint16_t apps1, uint16_t apps2) {
    if (APPS_Is10PercentApart(apps1, apps2)) {
        // Error
        // printf("APPS_Is10PercentApart %d\r\n ", APPS_Is10PercentApart(apps1, apps2));

        return 1;
    } else if (!APPS_IsInTolerance(apps1) || !APPS_IsInTolerance(apps2)) {
        // printf("APPS_IsInTolerance %d\r\n ", APPS_IsInTolerance(apps1));
        // printf("APPS_IsInTolerance %d\r\n ", APPS_IsInTolerance(apps2));
        //  Error
        return 1;
    } else if (APPS_ShortCircuit(apps1, apps2)) {
        return 1;
    } else if (APPS_ShortedTogether(apps1, apps2)) {
        return 1;
    } else {
        // No Error
        return 0;
    }
}

/// @brief Check if the APPS has an error more than 100ms
/// @param apps1 value of the APPS1
/// @param apps2 value of the APPS2
/// @return true if the APPS has an error more than 100ms, false otherwise

bool APPS_TimedOut(uint16_t apps1, uint16_t apps2) {
    static unsigned long lastTime = 0;
    unsigned long currentTime;
    uint16_t timeout = 100;  // ms

    if (APPS_CheckError(apps1, apps2)) {
        // Error
        if (APPS_Error == 0) {
            // update the last time
            // lastTime = millis();
            APPS_Error = 1;
        } else {
            // check if the time is greater than 100ms
            // currentTime = millis();
            if ((currentTime - lastTime) > timeout) {
                // set the error
                APPS_Error = 1;
            }
        }
        return 1;
    } else {
        // No Error
        lastTime = 0;
        APPS_Error = 0;
        return 0;
    }
}

/// @brief Calculate the percentage of the APPS
/// @param apps_mean value of the mean of the APPS
/// @return value of the percentage of the APPS 0-100

uint16_t APPS_ToPercentage(uint16_t apps_mean) {
    /*NOT USED*/
    return (apps_mean * 100) / APPS_functional_region;
}

/// @brief Calculate the value of the APPS 0-1000
/// @param apps_mean value of the mean of the APPS
/// @return value of the APPS 0-1000

uint16_t APPS_ToPercentage_1000(uint16_t apps_mean) {
    /*NOT USED*/
    return (apps_mean * 1000) / APPS_functional_region;
}

/// @brief Main function of the APPS that updates the values and check if there is an error
/// @param apps1 value of the APPS1
/// @param apps2 value of the APPS2
/// @return true if there is an error, false otherwise

bool APPS_Function(uint16_t apps1, uint16_t apps2) {
    // from  [0,APPS_MIN_bits ] the percentage is 0
    // from  [APPS_MIN_bits,APPS_MIN_bits + APPS_Tolerance_bits] the percentage is 0
    // from  [APPS_MIN_bits + APPS_Tolerance_bits,APPS_MAX_bits - APPS_Tolerance_bits] the percentage is 0-100
    // from  [APPS_MAX_bits - APPS_Tolerance_bits,APPS_MAX_bits] the percentage is 100
    // from  [APPS_MAX_bits,4095] the percentage is 0

    APPS_UpdateAPPS1(apps1);
    APPS_UpdateAPPS2(apps2);

    if (APPS_TimedOut(APPS1, APPS2)) {
        APPS_Percentage = 0;
        APPS_Percentage_1000 = 0;
        APPS_Mean = 0;

        return 1;
    } else {
        // No Error
        APPS_Mean = APPS_MeanValue(APPS1, APPS2);

        // if ((APPS_Mean >= APPS_MIN_bits) && (APPS_Mean <= (APPS_MIN_bits + APPS_Tolerance_bits))) {
        if (APPS_Mean <= (APPS_MIN_bits + APPS_Tolerance_bits)) {
            APPS_Percentage = 0;
            APPS_Percentage_1000 = 0;
            //} else if ((APPS_Mean <= APPS_MAX_bits) && (APPS_Mean >= (APPS_MAX_bits - APPS_Tolerance_bits))) {
        } else if (APPS_Mean >= (APPS_MAX_bits - APPS_Tolerance_bits)) {
            APPS_Percentage = 100;
            APPS_Percentage_1000 = 1000;
        } else {
            APPS_Percentage = map(APPS_Mean, APPS_MIN_bits + APPS_Tolerance_bits, APPS_MAX_bits - APPS_Tolerance_bits, 0, 100);
            APPS_Percentage_1000 = map(APPS_Mean, APPS_MIN_bits + APPS_Tolerance_bits, APPS_MAX_bits - APPS_Tolerance_bits, 0, 1000);
        }

        // limit the percentage
        if (APPS_Percentage > 100) {
            APPS_Percentage = 100;
        } else if (APPS_Percentage < 0) {
            APPS_Percentage = 0;
        }

        if (APPS_Percentage_1000 > 999) {
            APPS_Percentage_1000 = 999;
        } else if (APPS_Percentage_1000 < 0) {
            APPS_Percentage_1000 = 0;
        }

        APPS_ToPercentage(APPS_Mean);
        APPS_ToPercentage_1000(APPS_Mean);

        return 0;
    }
}

void APPS_PrintValues(void) {
    printf("APPSA %d ", APPS1);
    printf("APPSB %d ", APPS2);
    printf("APPS_Mean %d ", APPS_Mean);
    printf("APPS_Percentage %d ", APPS_Percentage);
    printf("APPS_Percentage_mil %d ", APPS_Percentage_1000);
    printf("APPS_Error %d ", APPS_Error);
    printf("APPS_MIN_bits %d ", APPS_MIN_bits);
    printf("APPS_MAX_bits %d ", APPS_MAX_bits);
    printf("APPS_Tolerance_bits %d ", APPS_Tolerance_bits);
    printf("APPS_functional_region %d ", APPS_functional_region);
    printf("\r\n");
}

// TODO auto calibration mode
void AUTO_CALIBRATION(uint16_t APPS1, uint16_t APPS2) {
    static uint16_t APPS1_MIN = 0;
    static uint16_t APPS1_MAX = 0;
    static uint16_t APPS2_MIN = 0;
    static uint16_t APPS2_MAX = 0;

    // tolerance 5%
    // static float TOLERANCE = 0.05;
    // static uint16_t APPS1_TOLERANCE;
    // static uint16_t APPS2_TOLERANCE;

    if (APPS1 < APPS1_MIN) {
        APPS1_MIN = APPS1;
    }
    if (APPS1 > APPS1_MAX) {
        APPS1_MAX = APPS1;
    }
    if (APPS2 < APPS2_MIN) {
        APPS2_MIN = APPS2;
    }
    if (APPS2 > APPS2_MAX) {
        APPS2_MAX = APPS2;
    }

    // calculate the values to be used in the APPS_Init

    printf("APPS1_MIN %d ", APPS1_MIN);
    printf("APPS1_MAX %d ", APPS1_MAX);
    printf("APPS2_MIN %d ", APPS2_MIN);
    printf("APPS2_MAX %d ", APPS2_MAX);

    // return values
}
