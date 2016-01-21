/*
 * box2tv serial communication driver
 *
 * Copyright (c) Foxconn
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/serio.h>
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/io.h>

#include "wolouartdrv.h"

// global variables
//static struct dev_data dev;

struct wolo_reg_dev g_reg_dev [32];
//wolo_vh
void RegUartDevFunc(struct wolo_reg_dev func)
{
    int reg_uart_dev_index = 0;

    reg_uart_dev_index = func.dev_num;
    g_reg_dev[reg_uart_dev_index].dev_num = func.dev_num;
    g_reg_dev[reg_uart_dev_index].wolo_reg_uart_func = func.wolo_reg_uart_func;

}



