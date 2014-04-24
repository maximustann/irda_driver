/*
 * 
 * Copyright (C) 2012 Juhui Corperation, Inc. All rights reserved.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * - pwm_irda send/study driver is written by Maximus Tann <tanbx@koti.cn>.
 * 
 *
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/pwm.h>


#include "imx28_pwm.h"

static struct resource pwm_resource[] = {
	[0] = {
			.start 	= HW_PWM_BASE,
			.end	= HW_PWM_BASE + HW_PWM_PERIOD7_CLR,
			.flags	= IORESOURCE_MEM,
	},
	[1] = {
			.start 	= HW_TIMROT_BASE,
			.end	= HW_TIMROT_BASE + HW_TIMROT_FIXED_COUNT2,
			.flags	= IORESOURCE_MEM,
	},
	[2] = {
			.start	= HW_PINCTRL_BASE,
			.end	= HW_PINCTRL_BASE + HW_PINCTRL_DOE3_CLR,
			.flags	= IORESOURCE_MEM,
	},
};
struct platform_device *pwm_dev;

static int __init platform_dev_init(void)
{

	int ret;
	pwm_dev = platform_device_alloc("pwm", -1);
	platform_device_add_resources(pwm_dev, pwm_resource, 3);
	ret = platform_device_add(pwm_dev);
	if(ret)
		platform_device_put(pwm_dev);
	return ret;	

}

static void __exit platform_dev_exit(void)
{
	platform_device_unregister(pwm_dev);
}

module_init(platform_dev_init);
module_exit(platform_dev_exit);

MODULE_AUTHOR("Maximus_Tann");
MODULE_LICENSE("GPL");
