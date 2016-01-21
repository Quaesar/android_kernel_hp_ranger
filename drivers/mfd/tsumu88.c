/*
 * TI tsumu88 MFD Driver
 *
 * Copyright 2011-2012 Texas Instruments Inc.
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * Author: Graeme Gregory <gg@slimlogic.co.uk>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/regmap.h>
#include <linux/err.h>
#include <linux/mfd/core.h>
#include <linux/mfd/tsumu88.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include "foxconn.h"
#include "wolouartdrv.h"

void	enter_ISP_mode();
int	Detect_ChipID();	

#define 	TEGRA_PINGROUP_GMI_CS3_N  60

static const struct regmap_config tsumu88_regmap_config[tsumu88_NUM_CLIENTS] = {
	{
		.reg_bits = 8,
		.val_bits = 8,
		.max_register = tsumu88_BASE_TO_REG(tsumu88_PU_PD_OD_BASE,
					tsumu88_PRIMARY_SECONDARY_PAD3),
//		.cache_type  = REGCACHE_RBTREE,
	},
	{
		.reg_bits = 8,
		.val_bits = 8,
		.max_register = tsumu88_BASE_TO_REG(tsumu88_GPADC_BASE,
					tsumu88_GPADC_SMPS_VSEL_MONITORING),
	}
};
#define tsumu88_MAX_INTERRUPT_MASK_REG	4
#define tsumu88_MAX_INTERRUPT_EDGE_REG	8

struct tsumu88_regs {
	int reg_base;
	int reg_add;
};

#define tsumu88_REGS(base, add)	{ .reg_base = base, .reg_add = add, }

//display

static struct tsumu88 *tsumu88_dev;



static int __devinit tsumu88_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct tsumu88 *tsumu88;
	//struct tsumu88_platform_data *pdata;
	int ret = 0, i;
	unsigned int reg, addr;
	int slave;
	int irq_flag;

	printk("into to tsumu88 set up +++++++++++++++++++\r\n");
	//pdata = dev_get_platdata(&i2c->dev);
	//if (!pdata)
	//	return -EINVAL;
	printk("into to tsumu88 set up +++++++++++++++++++1\r\n");
	tsumu88 = devm_kzalloc(&i2c->dev, sizeof(struct tsumu88), GFP_KERNEL);
	if (tsumu88 == NULL)
		return -ENOMEM;
	printk("into to tsumu88 set up +++++++++++++++++++2\r\n");
	i2c_set_clientdata(i2c, tsumu88);
	tsumu88->dev = &i2c->dev;
	tsumu88->id = id->driver_data;
	tsumu88->i2c_clients[0] = i2c;
	tsumu88->i2c_clients[1] = i2c_new_dummy(i2c->adapter,i2c->addr +0x10);
	
	printk("into to tsumu88 set up +++++++++++++++++++3\r\n");
/*
	for (i = 0; i < tsumu88_NUM_CLIENTS; i++) {
		if (i == 0)
			tsumu88->i2c_clients[i] = i2c;
		else {
			tsumu88->i2c_clients[i] =
					i2c_new_dummy(i2c->adapter,
							i2c->addr + 0x20*i);
			if (!tsumu88->i2c_clients[i]) {
				printk("can't attach client %d\n", i);
				ret = -ENOMEM;
				goto err;
			}
		}
#if 0		
		tsumu88->regmap[i] = devm_regmap_init_i2c(tsumu88->i2c_clients[i],
				&tsumu88_regmap_config[i]);
		if (IS_ERR(tsumu88->regmap[i])) {
			ret = PTR_ERR(tsumu88->regmap[i]);
			dev_err(tsumu88->dev,
				"Failed to allocate regmap %d, err: %d\n",
				i, ret);
			goto err;
		}
#endif		
	}
*/

	tsumu88_dev = tsumu88;
	printk(" tsumu88 set up ----------------finish\r\n");
	//enter_ISP_mode();
	//Detect_ChipID();	
	return ret;

err:
	mfd_remove_devices(tsumu88->dev);
	kfree(tsumu88);
	return ret;
}

#if 0
static const struct file_operations i2cdev_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.read		= i2cdev_read,
	.write		= i2cdev_write,
	.unlocked_ioctl	= i2cdev_ioctl,
	.open		= i2cdev_open,
	.release	= i2cdev_release,
};
#endif

/*Enter ISP mode
a.send ISP mode 			S+0x92+0x4d+0x53+0x54+0x41+0x52+P
b.send i2c bus ctrl		S+0xb2+0x35+P
											S+0xb2+0x71+P
c.send ISP mode 			S+0x92+0x4d+0x53+0x54+0x41+0x52+P	
int i2c_master_send(const struct i2c_client *client, const char *buf, int count)
int i2c_master_recv(const struct i2c_client *client, char *buf, int count)				
*/
void enter_ISP_mode()
{
	unsigned char ispmode_buffer[] = {0x4d,0x53,0x54,0x41,0x52};	
	unsigned char i2c_busA_buffer[] = {0x35};
	unsigned char i2c_busB_buffer[] = {0x71};
	i2c_master_send(tsumu88_dev->i2c_clients[0], ispmode_buffer, sizeof(ispmode_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[1], i2c_busA_buffer, sizeof(i2c_busA_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[1], i2c_busB_buffer, sizeof(i2c_busB_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], ispmode_buffer, sizeof(ispmode_buffer));		
	printk("enter_ISP_mode++++++++ \r\n");
}	
EXPORT_SYMBOL_GPL(enter_ISP_mode);
/*
Detect Flash brand and type
a.ReadChip ID 
S+0x92+0x10+0x9F+P+
S+0x92+0x11+P
S+0x93+0xc2+0x20+0x14+P
S+0x92+0x12+P
*/
int Detect_ChipID()
{
	unsigned char readIDA_buffer[]={0x10,0x9F};	
	unsigned char readIDB_buffer[]={0x11};	
	unsigned char tmp[4];
	unsigned char stop_buffer[]={0x12};	

	i2c_master_send(tsumu88_dev->i2c_clients[0], readIDA_buffer, sizeof(readIDA_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], readIDB_buffer, sizeof(readIDB_buffer));
	i2c_master_recv(tsumu88_dev->i2c_clients[0],tmp, 3);
	printk("Manufacture ID:%x \r\n",tmp[0]);
	printk("Device IDA:%x \r\n",tmp[1]);
	printk("Device IDB:%x \r\n",tmp[2]);
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));
	return 0;				
}	

EXPORT_SYMBOL_GPL(Detect_ChipID);

int Stop_Mstar()
{
	unsigned char stop_buffer[]={0x12};		
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));
	return 0;	
}	

EXPORT_SYMBOL_GPL(Stop_Mstar);
/*
Chip Erase:
a. Read SPI status register setting value and keep it
ReadStatus()
S+0x92+0x10+0x05+P+S+0x92+0x11+P+S+0x93+data+P
S+0x92+0x12+P

b.Write 0x00 to status register to disable write protect in flash
writeEnable()
S+0x92+0x10+0x06+P
S+0x92+0x12+P

writeStatus()
S+0x92+0x10+0x01+0x00+P
S+0x92+0x12+P
writeDisable
S+0x92+0x10+0x04+P
S+0x92+0x12+P

c.ReadStatus()
data =0 bit0 =0

d.chip erase
S+0x92+0x10+0xC7+P
S+0x92+0x12+P

e.ReadStatus()

*/
void Erase_Chip()
{
	unsigned char readStatusA_buffer[]={0x10,0x05};	
	unsigned char readStatusB_buffer[]={0x11};	
	unsigned char tmp[4];
	unsigned char stop_buffer[]={0x12};
	
	unsigned char EN_PRTT_buffer[]={0x10,0x06};
	unsigned char WR_PRTT_buffer[]={0x10,0x01,0x00};
	unsigned char DIS_PRTT_buffer[]={0x10,0x04};
	
	unsigned char ChipErase_buffer[]={0x10,0xC7};
	
		
	//read Status	
	i2c_master_send(tsumu88_dev->i2c_clients[0], readStatusA_buffer, sizeof(readStatusA_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], readStatusB_buffer, sizeof(readStatusB_buffer));	
	i2c_master_recv(tsumu88_dev->i2c_clients[0],tmp, 1);
	printk("Busy Status:%x \r\n",tmp[0]);
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));

	//disable write protect in flash
	i2c_master_send(tsumu88_dev->i2c_clients[0], EN_PRTT_buffer, sizeof(EN_PRTT_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));
	
	i2c_master_send(tsumu88_dev->i2c_clients[0], WR_PRTT_buffer, sizeof(WR_PRTT_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));
	
	i2c_master_send(tsumu88_dev->i2c_clients[0], DIS_PRTT_buffer, sizeof(DIS_PRTT_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));

	//read Status	
	i2c_master_send(tsumu88_dev->i2c_clients[0], readStatusA_buffer, sizeof(readStatusA_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], readStatusB_buffer, sizeof(readStatusB_buffer));	
	i2c_master_recv(tsumu88_dev->i2c_clients[0],tmp, 1);
	printk("Busy Status:%x \r\n",tmp[0]);
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));
	
	//Erase Chip
	i2c_master_send(tsumu88_dev->i2c_clients[0], ChipErase_buffer, sizeof(ChipErase_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));	

	//read Status	
	i2c_master_send(tsumu88_dev->i2c_clients[0], readStatusA_buffer, sizeof(readStatusA_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0],readStatusB_buffer, sizeof(readStatusB_buffer));	
	i2c_master_recv(tsumu88_dev->i2c_clients[0],tmp, 1);
	printk("Busy Status:%x \r\n",tmp[0]);
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));	

}
/*
Blanking Check
S+0x92+0x10+0x03+AddrH+AddrM+AddrL+P
S+0x92+0x11+S+0x93+Data0+Data1+Data2+...+DataN+P

S+0x92+0x12+P
*/


/*
Program
A.Check flash
ReadStatus () bit0 = 0
B.WriteData
S+0x92+0x10+0x02+AddrH+AddrM+AddrL+Data0+Data2+....Data255
C.Continue process A and B until all code
*/

/*
Verify:
Blanking Check
*/
/*
Write Protect:
WriteStatus()Write status register with flash protect setting value
*/
void EN_Write_Protect()
{
	unsigned char EN_PRTT_buffer[]={0x10,0x06};
	unsigned char WR_PRTT_buffer[]={0x10,0x01,0x01};
	unsigned char DIS_PRTT_buffer[]={0x10,0x04};
	unsigned char stop_buffer[]={0x12};
	
	i2c_master_send(tsumu88_dev->i2c_clients[0], EN_PRTT_buffer, sizeof(EN_PRTT_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));
	
	i2c_master_send(tsumu88_dev->i2c_clients[0], WR_PRTT_buffer, sizeof(WR_PRTT_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));
	
	i2c_master_send(tsumu88_dev->i2c_clients[0], DIS_PRTT_buffer, sizeof(DIS_PRTT_buffer));
	i2c_master_send(tsumu88_dev->i2c_clients[0], stop_buffer, sizeof(stop_buffer));
	
}
/*
Exit ISP mode:
S+0x92+0x24+P
*/
void exit_ISP_mode()
{
	char ispmode_buffer[] = {0x24};	
	i2c_master_send(tsumu88_dev->i2c_clients[0], ispmode_buffer, sizeof(ispmode_buffer));
}	




static int tsumu88_i2c_remove(struct i2c_client *i2c)
{
	struct tsumu88 *tsumu88 = i2c_get_clientdata(i2c);

	mfd_remove_devices(tsumu88->dev);


	return 0;
}



static const struct i2c_device_id tsumu88_i2c_id[] = {
	{ "tsumu88", },
	{ /* end */ }
};
MODULE_DEVICE_TABLE(i2c, tsumu88_i2c_id);

static struct of_device_id __devinitdata of_tsumu88_match_tbl[] = {
	{ .compatible = "ti,tsumu88", },
	{ /* end */ }
};


static struct i2c_driver tsumu88_i2c_driver = {
	.driver = {
		   .name = "tsumu88",
		   .of_match_table = of_tsumu88_match_tbl,
		   .owner = THIS_MODULE,
	},
	.probe = tsumu88_i2c_probe,
	.remove = tsumu88_i2c_remove,
	.id_table = tsumu88_i2c_id,
};

static int __init tsumu88_i2c_init(void)
{
	return i2c_add_driver(&tsumu88_i2c_driver);
}
/* init early so consumer devices can complete system boot */
subsys_initcall(tsumu88_i2c_init);

static void __exit tsumu88_i2c_exit(void)
{
	i2c_del_driver(&tsumu88_i2c_driver);
}
module_exit(tsumu88_i2c_exit);

MODULE_AUTHOR("Graeme Gregory <gg@slimlogic.co.uk>");
MODULE_DESCRIPTION("tsumu88 chip family multi-function driver");
MODULE_LICENSE("GPL");
