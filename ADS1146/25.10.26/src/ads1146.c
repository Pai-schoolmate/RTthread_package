/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-28     H1567       the first version
 */

#include "ads1146.h"

#define DBG_TAG "ads1146"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

struct rt_spi_device *ads1146_handle1;
struct rt_spi_device *ads1146_handle2;
struct rt_spi_device *ads1146_handle3;

/***************************************************************
  *  @brief     ADS1146的SPI设备创建
  *  @param
  *  @return
 **************************************************************/
void SPI_device_creation(void)
{
#ifdef BSP_USING_SPI1_ADS1146
    rt_err_t spi10_flag;
    struct rt_spi_configuration ads1146_cfg1;
    spi10_flag = rt_hw_spi_device_attach(SPI1_BUS_NAME, SPI1_NAME, GPIOA, GPIO_PIN_4);//更改为实际的片选引脚
    if(spi10_flag != RT_EOK){
        LOG_D("failed to spi10 register");
        return;
        }
    ads1146_handle1 = (struct rt_spi_device *)rt_device_find(SPI1_NAME);
    if(ads1146_handle1 != RT_NULL){
        ads1146_cfg1.mode = RT_SPI_MSB | RT_SPI_MASTER | RT_SPI_MODE_1;
        ads1146_cfg1.data_width = 8;
        ads1146_cfg1.max_hz = 1125*1000;
        rt_spi_configure(ads1146_handle1, &ads1146_cfg1);
    }
#endif
#ifdef BSP_USING_SPI2_ADS1146
    rt_err_t spi20_flag;
    struct rt_spi_configuration ads1146_cfg2;
    spi20_flag = rt_hw_spi_device_attach(SPI2_BUS_NAME, SPI2_NAME, GPIOB, GPIO_PIN_12);//更改为实际的片选引脚
    if(spi20_flag != RT_EOK){
        LOG_D("failed to spi20 register");
        return;
        }
    ads1146_handle2 = (struct rt_spi_device *)rt_device_find(SPI2_NAME);
    if(ads1146_handle2 != RT_NULL){
        ads1146_cfg2.mode = RT_SPI_MSB | RT_SPI_MASTER | RT_SPI_MODE_1;
        ads1146_cfg2.data_width = 8;
        ads1146_cfg2.max_hz = 1125*1000;
        rt_spi_configure(ads1146_handle2, &ads1146_cfg2);
    }
#endif
#ifdef BSP_USING_SPI3_ADS1146
    rt_err_t spi30_flag;
    struct rt_spi_configuration ads1146_cfg3;
    spi30_flag = rt_hw_spi_device_attach(SPI3_BUS_NAME, SPI3_NAME, GPIOB, GPIO_PIN_12);//更改为实际的片选引脚
    if(spi30_flag != RT_EOK){
        LOG_D("failed to spi30 register");
        return;
        }
    ads1146_handle3 = (struct rt_spi_device *)rt_device_find(SPI3_NAME);
    if(ads1146_handle3 != RT_NULL){
        ads1146_cfg3.mode = RT_SPI_MSB | RT_SPI_MASTER | RT_SPI_MODE_1;
        ads1146_cfg3.data_width = 8;
        ads1146_cfg3.max_hz = 1125*1000;
        rt_spi_configure(ads1146_handle3, &ads1146_cfg3);
    }
#endif
    LOG_D("succeed to ADS1146 SPI device creation");
}
/***************************************************************
  *  @brief     ADS1146的初始化
  *  @param
  *  @return
 **************************************************************/
void ADS1146_init(void)
{
    uint8_t INIT_control_1[2] = {0x01,0x06};
    uint8_t INIT_control_2 = 0x16;
#ifdef BSP_USING_SPI1_ADS1146
    rt_pin_mode(ADS1_start, PIN_MODE_OUTPUT);
    rt_pin_write(ADS1_start, PIN_HIGH);
    rt_pin_mode(ADS1_drdy, PIN_MODE_INPUT_PULLUP);
    rt_spi_send(ads1146_handle1, INIT_control_1, 2);
    rt_thread_mdelay(10);
    rt_spi_send(ads1146_handle1, &INIT_control_2, 1);
    rt_thread_mdelay(10);
#endif
#ifdef BSP_USING_SPI2_ADS1146
    rt_pin_mode(ADS2_start, PIN_MODE_OUTPUT);
    rt_pin_write(ADS2_start, PIN_HIGH);
    rt_pin_mode(ADS2_drdy, PIN_MODE_INPUT_PULLUP);
    rt_spi_send(ads1146_handle2, INIT_control_1, 2);
    rt_thread_mdelay(10);
    rt_spi_send(ads1146_handle2, &INIT_control_2, 1);
    rt_thread_mdelay(10);
#endif
#ifdef BSP_USING_SPI3_ADS1146
    rt_pin_mode(ADS3_start, PIN_MODE_OUTPUT);
    rt_pin_write(ADS3_start, PIN_HIGH);
    rt_pin_mode(ADS3_drdy, PIN_MODE_INPUT_PULLUP);
    rt_spi_send(ads1146_handle2, INIT_control_1, 2);
    rt_thread_mdelay(10);
    rt_spi_send(ads1146_handle2, &INIT_control_2, 1);
    rt_thread_mdelay(10);
#endif
}
/***************************************************************
  *  @brief     ADS1146的PGA设置函数
  *  @param     device：传入的设备名；ADS_setPGA：需要设置的PGA十进制值
  *  @return
 **************************************************************/
void ADS1146_setPGA(struct rt_spi_device *device,short ADS_setPGA)
{
    if(ADS_setPGA == 1 || ADS_setPGA == 2 || ADS_setPGA == 4
            || ADS_setPGA == 8 || ADS_setPGA == 16 || ADS_setPGA == 32
            || ADS_setPGA == 64 || ADS_setPGA == 128)
    {
    uint8_t INIT_control[5] = {0x43,0x00,0x00,0x04,0x04};
    INIT_control[2] = (uint8_t)log2(ADS_setPGA)<<4 | ADS_SPS;
    rt_spi_send(device, INIT_control, 5);
    }
    else {
        rt_kprintf("ADS PGA set error!\n");
    }
}
/***************************************************************
  *  @brief     ADS1146的当前PGA读取函数
  *  @param     device：传入的设备名
  *  @return    读取到的该设备PGA十进制值
 **************************************************************/
short ADS1146_readPGA(struct rt_spi_device *device)
{
    uint8_t read_control[2] = {0x23,0x00};
    uint8_t read_datas;
    short ADS_readPGA;
    rt_spi_send_then_recv(device, &read_control, 2, &read_datas, 1);
    ADS_readPGA = (short)powf(2,((read_datas&0xf0)>>4));
    return ADS_readPGA;
}
/***************************************************************
  *  @brief     实现PGA的自动调节
  *  @param     device：传入的设备名
  *  @return    0：未超量程，1：超量程
 **************************************************************/
short ADS1146_PGA_AUTO(struct rt_spi_device *device)
{
    short ADS_PGA;
    static short ADC_PGA_old = 1;
    short PGA_flag = 0;
    int ADS_value_init;
    uint16_t read_value;
    short ads_gain,PGA_gain;
    uint8_t read_control = 0x12;
    uint8_t read_datas[2] = {0};
    do
    {
        rt_spi_send_then_recv(device, &read_control, 1, read_datas, 2);
        read_value = ((uint16_t)read_datas[0])<<8 | read_datas[1];
        //数值绝对值化
        if(read_value&0x8000){
            ADS_value_init = 65536 - (int)read_value;
        }
        else{
            ADS_value_init = (int)read_value;
        }
        if(ADS_value_init<1)
            ADS_value_init = 1;
        ADS_PGA = ADS1146_readPGA(device);
        if(ADS_value_init > 32765){
            if(ADS_PGA == 1){
                PGA_flag = 1;
            }
            else {
                ADS_PGA = 1;
            }
        }
        if(ADS_value_init<16382){
            ads_gain = 32767/ADS_value_init;
            if(ads_gain>128){
                ads_gain = 128;
            }
            PGA_gain = 1;
            do{
                PGA_gain *= 2;
            }while(PGA_gain<ads_gain);
            if(PGA_gain>ads_gain){
                PGA_gain /= 2;
            }
            ADS_PGA *= PGA_gain;
            if(ADS_PGA>128){
                ADS_PGA = 128;
            }
        }
        if(ADS_PGA != ADC_PGA_old){
            ADS1146_setPGA(device,ADS_PGA);
            ADC_PGA_old = ADS_PGA;
            rt_thread_mdelay(1500/(5*powf(2,ADS_SPS)));
        }
    }while(ADS_value_init > 32765 && PGA_flag == 0);
    return PGA_flag;
}
/***************************************************************
  *  @brief     实现ADS1146采集的电压值输出，单位mV
  *  @param     device：传入的设备名;
  *             calibration_k, calibration_b: 校准参数
  *             校准函数：ADS_CCD_mV = calibration_k*ADS_mV + calibration_b
  *             PGA_flag：量程标志，0：正常，1：超量程。搭配ADS1146_PGA_AUTO使用可实现自动判断
  *  @return
 **************************************************************/
void ADS1146_output(struct rt_spi_device *device, float calibration_k, float calibration_b, short PGA_flag)
{
    int ADS_value_init;
    uint16_t read_value;
    short read_signed;
    uint8_t read_control = 0x12;
    uint8_t read_datas[2] = {0};
    rt_spi_send_then_recv(device, &read_control, 1, read_datas, 2);
    read_value = ((uint16_t)read_datas[0])<<8 | read_datas[1];
    //符号位分离
    if(read_value&0x8000){
        ADS_value_init = 65536 - (int)read_value;
        read_signed = -1;
    }
    else{
        ADS_value_init = (int)read_value;
        read_signed = 1;
    }
    short setPGA = ADS1146_readPGA(device);
    rt_kprintf("ADS1146 set PGA: %d\n",setPGA);
    if(PGA_flag == 1){
        rt_kprintf("ADS1146 Exceeding the range  ");
    }
    else {
        float ADS_mV = (float)ADS_value_init / ADS_range_half *reference_voltage /setPGA*read_signed;
        rt_kprintf("ADS1146 Not calibrated output data: %.3fmV  ",ADS_mV);
        float ADS_CCD_mV = ADS_mV*calibration_k+calibration_b;
        rt_kprintf("ADS1146 calibrated output data: %.3fmV\n",ADS_CCD_mV);
    }
}
