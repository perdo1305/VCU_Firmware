/*
 * File:   cansart_db_lc.h
 * Author: pedro
 *
 * Created on 6 de Mar�o de 2024, 18:24
 */

#ifndef CANSART_DB_LC_H
#define CANSART_DB_LC_H

#include <stdint.h>

#define SLAVEMODE 1
// READ
struct frame10 {
    uint8_t ID;
    uint8_t DATA1;
    uint8_t DATA2;
    uint8_t DATA3;
    uint8_t DATA4;
    uint8_t DATA5;
    uint8_t DATA6;
    uint8_t DATA7;
    uint8_t DATA8;
    uint8_t LENGHT;
};

struct frame11 {
    uint8_t ID;
    uint8_t DATA1;
    uint8_t DATA2;
    uint8_t DATA3;
    uint8_t DATA4;
    uint8_t DATA5;
    uint8_t DATA6;
    uint8_t DATA7;
    uint8_t DATA8;
    uint8_t LENGHT;
};

// WRITE
struct frame121 {
    uint8_t ID;
    uint8_t DATA1;
    uint8_t DATA2;
    uint8_t DATA3;
    uint8_t DATA4;
    uint8_t DATA5;
    uint8_t DATA6;
    uint8_t DATA7;
    uint8_t DATA8;
    uint8_t LENGHT;
};

#endif