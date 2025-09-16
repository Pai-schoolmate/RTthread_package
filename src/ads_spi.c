/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-10     1309       the first version
 */
#include "ads_spi.h"
#include "drv_spi.h"

#define DBG_TAG "ads_spi"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define     SPI1_NAME    "spi10"
#define     SPI1_BUS_NAME    "spi1"
#define     SPI2_NAME    "spi20"
#define     SPI2_BUS_NAME    "spi2"

#define     ADS1_start      GET_PIN(B,0)
#define     ADS1_drdy      GET_PIN(B,1)
#define     ADS2_start      GET_PIN(B,10)
#define     ADS2_drdy      GET_PIN(B,11)

struct rt_spi_device *ads1146_handle1;
struct rt_spi_device *ads1146_handle2;

void SPI_device_creation(void)
{
    rt_err_t spi10_flag;
    struct rt_spi_configuration ads1146_cfg1;
    spi10_flag = rt_hw_spi_device_attach(SPI1_BUS_NAME, SPI1_NAME, GPIOA, GPIO_PIN_4);
    if(spi10_flag != RT_EOK){
        LOG_D("failed to spi10 register");
        return;
        }
    rt_err_t spi20_flag;
    struct rt_spi_configuration ads1146_cfg2;
    spi20_flag = rt_hw_spi_device_attach(SPI2_BUS_NAME, SPI2_NAME, GPIOB, GPIO_PIN_12);
    if(spi20_flag != RT_EOK){
        LOG_D("failed to spi20 register");
        return;
        }
    ads1146_handle1 = (struct rt_spi_device *)rt_device_find(SPI1_NAME);
    if(ads1146_handle1 != RT_NULL){
        ads1146_cfg1.mode = RT_SPI_MSB | RT_SPI_MASTER | RT_SPI_MODE_1;
        ads1146_cfg1.data_width = 8;
        ads1146_cfg1.max_hz = 1125*1000;
        rt_spi_configure(ads1146_handle1, &ads1146_cfg1);
    }
    ads1146_handle2 = (struct rt_spi_device *)rt_device_find(SPI2_NAME);
    if(ads1146_handle2 != RT_NULL){
        ads1146_cfg2.mode = RT_SPI_MSB | RT_SPI_MASTER | RT_SPI_MODE_1;
        ads1146_cfg2.data_width = 8;
        ads1146_cfg2.max_hz = 1125*1000;
        rt_spi_configure(ads1146_handle2, &ads1146_cfg2);
    }
    LOG_D("succeed to spi10 and spi20 device creation");
}
void ADS1146_init(void)
{
    rt_pin_mode(ADS1_start, PIN_MODE_OUTPUT);
    rt_pin_write(ADS1_start, PIN_HIGH);
    rt_pin_mode(ADS2_start, PIN_MODE_OUTPUT);
    rt_pin_write(ADS2_start, PIN_HIGH);

    rt_pin_mode(ADS1_drdy, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(ADS2_drdy, PIN_MODE_INPUT_PULLUP);
    rt_thread_mdelay(1);

    uint8_t INIT_control_1[2] = {0x01,0x06};
    uint8_t INIT_control_2[6] = {0x16,0x43,0x00,0x31,0x04,0x04};
    struct rt_spi_message msg1,msg2;
    msg1.send_buf   = &INIT_control_1;
    msg1.recv_buf   = RT_NULL;
    msg1.length     = 2;
    msg1.cs_take    = 1;
    msg1.cs_release = 0;
    msg1.next       = RT_NULL;
    rt_spi_transfer_message(ads1146_handle1, &msg1);//WAKEUP
    rt_spi_transfer_message(ads1146_handle2, &msg1);//WAKEUP
    rt_thread_mdelay(10);
    msg2.send_buf   = INIT_control_2;
    msg2.recv_buf   = RT_NULL;
    msg2.length     = 6;
    msg2.cs_take    = 0;
    msg2.cs_release = 1;
    msg2.next       = RT_NULL;
    // SYS0 PGA = 8 ; DR = 160SPS
    rt_spi_transfer_message(ads1146_handle1, &msg2);
    rt_spi_transfer_message(ads1146_handle2, &msg2);
    rt_thread_mdelay(1);
}
void ADS1146_RREG(void)
{
    uint8_t rreg_control[2] = {0x22,0x00};
    uint8_t rreg1_datas[1] = {0};
    uint8_t eerg2_datas[1] = {0};
    rt_spi_send_then_recv(ads1146_handle1, &rreg_control, 2, rreg1_datas, 1);
    rt_spi_send_then_recv(ads1146_handle2, &rreg_control, 2, eerg2_datas, 1);
    rt_kprintf("ADS1 output rreg: %X  ",rreg1_datas[0]);
    rt_kprintf("ADS2 output rreg: %X\n",eerg2_datas[0]);
}
void ADS1146_read(void)
{
    uint16_t read1_value,read2_value;
    int ADS1_value,ADS2_value;
    short read1_signed,read2_signed;
    uint8_t read_control = 0x12;
    uint8_t read1_datas[2] = {0};
    uint8_t read2_datas[2] = {0};
    rt_spi_send_then_recv(ads1146_handle1, &read_control, 1, read1_datas, 2);
    rt_spi_send_then_recv(ads1146_handle2, &read_control, 1, read2_datas, 2);
    read1_value = ((uint16_t)read1_datas[0])<<8 | read1_datas[1];
    read2_value = ((uint16_t)read2_datas[0])<<8 | read2_datas[1];
    rt_kprintf("ADS1 output data: %X  ",read1_value);
    rt_kprintf("ADS2 output data: %X\n",read2_value);
    if(read1_value&0x8000)
    {
        ADS1_value = 65536 - (int)read1_value;
        read1_signed = -1;
    }
    else
    {
        ADS1_value = (int)read1_value;
        read1_signed = 1;
    }
    if(read2_value&0x8000)
    {
        ADS2_value = 65536 - (int)read2_value;
        read2_signed = -1;
    }
    else
    {
        ADS2_value = (int)read2_value;
        read2_signed = 1;
    }
    float ADS1_mV = (float)ADS1_value / 32768 *1250 /8*read1_signed;
    float ADS2_mV = (float)ADS2_value / 32768 *1250 /8*read2_signed;
    rt_kprintf("ADS1 output data: %.2fmV  ",ADS1_mV);
    rt_kprintf("ADS2 output data: %.2fmV\n",ADS2_mV);
}
