/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-10     1309       the first version
 */
#ifndef APPLICATIONS_ADS_SPI_H_
#define APPLICATIONS_ADS_SPI_H_

#include <board.h>
void SPI_device_creation(void);
void ADS1146_init(void);
void ADS1146_read(void);

void ADS1146_RREG(void);

#endif /* APPLICATIONS_ADS_SPI_H_ */
