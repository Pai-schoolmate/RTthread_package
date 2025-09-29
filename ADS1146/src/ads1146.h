/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-28     H1567       the first version
 */
#ifndef APPLICATIONS_ADS1146_H_
#define APPLICATIONS_ADS1146_H_

#include <board.h>
#include <rtdevice.h>
#include "drv_spi.h"
#include <math.h>
/*-------------------------- ADS1146 CONFIG BEGIN --------------------------*/
//需要在board.h中开启对于的SPI预编译
#define BSP_USING_SPI1_ADS1146
#define BSP_USING_SPI2_ADS1146
//#define BSP_USING_SPI3_ADS1146

#ifdef BSP_USING_SPI1_ADS1146
    #define     SPI1_NAME    "spi10"
    #define     SPI1_BUS_NAME    "spi1"
extern struct rt_spi_device *ads1146_handle1;
#endif
#ifdef BSP_USING_SPI2_ADS1146
    #define     SPI2_NAME    "spi20"
    #define     SPI2_BUS_NAME    "spi2"
extern struct rt_spi_device *ads1146_handle2;
#endif
#ifdef BSP_USING_SPI3_ADS1146
    #define     SPI3_NAME    "spi30"
    #define     SPI3_BUS_NAME    "spi3"
extern struct rt_spi_device *ads1146_handle3;
#endif

// 更改为自己实际的引脚
#define     ADS1_start      GET_PIN(B,0)
#define     ADS1_drdy      GET_PIN(B,1)
#define     ADS2_start      GET_PIN(B,10)
#define     ADS2_drdy      GET_PIN(B,11)
#define     ADS3_start      GET_PIN(B,10)
#define     ADS3_drdy      GET_PIN(B,11)

#define     ADS_SPS     0x01    //10 SPS
#define     ADS_range_half   32768
#define     reference_voltage   1250

void SPI_device_creation(void);
void ADS1146_init(void);
void ADS1146_setPGA(struct rt_spi_device *device,short ADS_setPGA);
short ADS1146_readPGA(struct rt_spi_device *device);
short ADS1146_PGA_AUTO(struct rt_spi_device *device);
void ADS1146_output(struct rt_spi_device *device,float calibration_k,float calibration_b,short PGA_flag);

#endif /* APPLICATIONS_ADS1146_H_ */
