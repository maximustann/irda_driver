/*
 * 
 * Copyright (C) 2012 Jhsys Corperation, Inc. All rights reserved.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * - pwm_irda send/study driver is written by Maximus Tann <tanbx@koti.cn>.
 * 
 *	pwm_irda driver version 2.6.0
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/fsl_devices.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/time.h>

#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>



#include <mach/mx28.h>
#include "imx28_pwm.h"



#define SIZE 1024
#define SHAKE 30



static int pwm_start(int);
static int pwm_shut(int);


static void timer_on(int);
static void timer_off(void);

static void timer_study_on(int);
static void timer_study_off(void);

static void change_mode_study(void);
static void change_mode_send(void);
static int translate_channel(int);
static void repair_data(char *data);

static int read_io_status(void);

enum condition {
	DOWN,
	UP,
};

enum pwm_mode {
	STUDY,
	SEND,
};

struct ir_read {
	unsigned char buffer[SIZE];
};

struct ir_send {
	unsigned int channel;
	unsigned char buffer[SIZE];
};


static DEFINE_MUTEX(pwm_lock);
static DEFINE_SPINLOCK(pwm_reg_lock);
static LIST_HEAD(pwm_list);

static int study_process(void);

/*	
 *		pwm_device结构体说明:
 *		buffer_send						作为发送时的buffer
 *		buffer_send_real				作为转换后的真实发送缓冲区
 *		buffer_read						作为学习时的buffer
 *		list_head node					添加到platform设备上的节点
 *		pdev							platform_device类设备节点
 *		__iomem	*pwm_base				pwm寄存器首地址
 *		__iomem *timer_base				timer1寄存器首地址
 *		__iomem *gpio					gpio首地址
 *		count							引用记数
 *		status[]						每个pwm的开关状态，开或关
 *		my_pwm_mode[]					每个pwm的模式, 发送或学习
 *		my_condition					中断状态位,中断时用到
 *
 */
struct pwm_device {
	struct ir_send buffer_send;
	int buffer_send_real[SIZE];
	struct ir_read *buffer_read;
	struct list_head node;
	struct platform_device *pdev;
	
	void __iomem	*pwm_base;
	void __iomem	*timer_base;
	void __iomem	*gpio_base;

	char count;
	char status[PWM_CHANNEL];
	enum pwm_mode my_pwm_mode[PWM_CHANNEL];
	enum condition my_condition;
};

/*
 *	全局变量说明:
 *		send_channel:	发送时将通道号传入此变量来判断从哪个通道发出
 *		begin_flag:		用来判断是否为第一次发送
 *		*send_pwm_data:	用来指向发送缓冲区的指针
 *		send_pwm_data_index:	发送缓冲区的记数值
 *		_pwm_study_data:	学习时读上来的数据
 *		tmp_data[2]:	前一次读到的值和后现在读到的值，用来判断电平是否变化
 *		*pwm:	结构体指针,用来申请空间
 *
 */
static int send_channel = 0;
static int begin_flag;
static int *send_pwm_data;
static int send_pwm_data_index;
static int _pwm_study_data;
static int tmp_data[2] = {0};
static struct pwm_device *pwm;


/*
 *	send_wait		发送中断队列
 *	study_wait		学习中断队列
 *	study_condition 学习状态切换
 */
wait_queue_head_t send_wait;
wait_queue_head_t study_wait;
enum condition study_condition;



/*
 *		pwm_open说明:	
 *				打开设备，引用记数 + 1,
 *				所有pwm状态设为关闭,
 *				所有pwm设为发送模式
 */
int pwm_open(struct inode *inode, struct file *filp)
{

	int i;
	if(pwm->count == 1)
		return -EBUSY;
	pwm->count += 1;
	for(i = 0; i < 7; i++)
	{
		pwm->status[i] = SHUT;
		pwm->my_pwm_mode[i] = SEND;
	}	

	return 0;
}


/*				
 *		pwm_send()说明:
 *			int channel : 标记哪个pwm发送
 *			char *data:	数据
 */
static int pwm_send(int channel, int *data)
{
    send_channel = channel;
    send_pwm_data = data;	
    send_pwm_data_index = 0;    //even or odd and send data's index     
    timer_on(US20);


	if(wait_event_interruptible(send_wait, pwm->my_condition))
   	{
		printk("interrupt error\n");
   		return -ERESTARTSYS;
   	}
	pwm->my_condition = DOWN;
	return 0;
	
}

/*
 *	pwm_write()说明:
 *		以20us为周期，初始化一个计时器，以buffer内数据为循环数
 *		不断循环输出pwm
 *
 */
int pwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	int i;
	int channel;
	struct ir_send *buf_p;

	memset(pwm->buffer_send.buffer, 0, sizeof(struct ir_send) - 4);
	//copy user buffer to kernel
	copy_from_user(&pwm->buffer_send, buf, len);
	buf_p = &pwm->buffer_send;
	channel = buf_p->channel;
	channel -= 1;
	if(len > sizeof(struct ir_send))
	{
		printk("len is too long! %d\n", len);
		return -EINVAL;
	}
	if(channel < -1 || channel > 7)
	{
		printk("channel out of range!\n");
		return -EINVAL;
	}
	repair_data(buf_p->buffer);

	if(channel == -1)
	{
		for(i = 0; i < 7; i++)
		{
			pwm_send(i, pwm->buffer_send_real);
		}
	}
	else
	{
		channel = translate_channel(channel);
		pwm_send(channel, pwm->buffer_send_real);
	}
	
	return 0;
}

/*
 *	repair_data:
 *		将数据还原为原始数据
 *
 */

static void repair_data(char *data)
{
	int i, j = 0;
	memset(pwm->buffer_send_real, 0, SIZE);
	for(i = 0; i < SIZE; i++, j++)
	{
		pwm->buffer_send_real[j] = data[i];
		if(data[i] == 0x0)
		{
			pwm->buffer_send_real[j] = data[i + 1] * 0xff + data[i + 2];
			i += 2;
		}
	}
}


/*
 *		pwm_read函数说明:
 *				记录每个记数值。直到发现记数值为0就退出。将该块内存复制到用户区域。
 *				返回读到的数据个数。
 */
int pwm_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	int i;
	if(pwm->buffer_read->buffer[0] == 0x0 && pwm->buffer_read->buffer[1] == 0x0)
	{
		copy_to_user(buf, pwm->buffer_read, sizeof(struct ir_read));
		memset(pwm->buffer_read->buffer, 0, sizeof(struct ir_read));
		return -1;
	}
	copy_to_user(buf, pwm->buffer_read, sizeof(struct ir_read));
	for(i = 0; ;i++)
	{
		if(pwm->buffer_read->buffer[i] == 0x0 && pwm->buffer_read->buffer[i + 1] == 0x0)
			break;
	}
	memset(pwm->buffer_read->buffer, 0, sizeof(struct ir_read));
	printk("yea, you got the data = %d\n", i);
	return i;
}

/*
 *	pwm_study()函数:
 *		每次学习都调用study_process函数, 退出条件为读到数据为0，或超出SIZE
 *		将数组全部内容向前移动一格，将第一次读到的无意义数据覆盖掉
 *		如果读到的数据小于30个则判定为抖动数据，直接丢弃
 */
static int pwm_study(void)
{
	int i = 0, j;
	int k = 0;
	int tmp_da = 0;	
	int yu;
	begin_flag = 0;
	memset(pwm->buffer_read->buffer, 0, sizeof(struct ir_read));
	study_condition = DOWN;

	for(; ;)
	{
		if(i == 0)	
		{
			timer_study_on(US20);
		}
		if(i == 1)
		{
			begin_flag = 1;
		}
		//学习一段wave的过程
		tmp_da = study_process();
		if(tmp_da >= ONE_S) //超长时间，直接退出
		{
			break;
		}
		else if(tmp_da > 5500 && begin_flag != 0)//如果超过5500，且不是第一次，则作退出处理
		{
			pwm->buffer_read->buffer[k] = 0x0;
			pwm->buffer_read->buffer[k + 1] = 0x0;
			break;
		}
		else if(tmp_da > 0xff && tmp_da < 5500) //记录下来
		{
			pwm->buffer_read->buffer[k] = 0x0;
			k++;
			yu = tmp_da / 0xff;
			pwm->buffer_read->buffer[k] = yu;
			k++;
			pwm->buffer_read->buffer[k] = tmp_da - yu * 0xff;
			k++;
		}
		else if(tmp_da == 0)
		{
			continue;
		}
		else
		{
			pwm->buffer_read->buffer[k] = tmp_da;
			k++;
		}
		i++;  //wave counter
	}

	for(j = 0; j < k; j++)
	{
		pwm->buffer_read->buffer[j] = pwm->buffer_read->buffer[j + 1];
	}

	if(k < SHAKE)
	{
		memset(pwm->buffer_read->buffer, 0, sizeof(struct ir_read));
		k = 0;
	}	

	return k;
}


/*
 *		study_process()函数说明:
 *				每次读取的值如果相同，则计数器+1，如果不同，则退出。
 *				返回计数器值。
 */

static int study_process(void)
{
	int tmp;
	if(wait_event_interruptible(study_wait, study_condition))
	{
		return -ERESTARTSYS;
	}
	
	tmp = _pwm_study_data;
	_pwm_study_data = 0;
	study_condition = DOWN;
	return tmp;
}

/*
 *		read_io_status()函数说明:
 *				读取寄存器状态并返回。
 *
 */
static int read_io_status(void)
{
	unsigned int i;
	
	i = ioread32(pwm->gpio_base + HW_PINCTRL_DIN1);
	i &= 1 << 11;
	i = i >> 11;
	return i;
}

/*
 *		ioctl说明:
 *			change_mode_study:改模式为study
 *			change_mode_send:改模式为send
 *
 */
int pwm_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case CHANGE_MODE_STUDY:		
		    change_mode_study(); 
		    pwm_study(); 
		    break;
		case CHANGE_MODE_SEND:		change_mode_send();	break;
		default:printk("command not found\n");		return -1;
	}
	return 0;
}


/*
 *	根据板上的实际端口位置，进行编号转换
 *
 */
static int translate_channel(int channel)
{
	switch(channel)
	{
	case 0: channel = 3; break;
	case 1: break;
	case 2: break;
	case 3:	channel = 0; break;
	case 4:	channel = 4; break;
	case 5:	channel = 1; break;
	default: break;
	}
	return channel;
}

static void change_mode_study(void)
{
	int i;
	for(i = 0; i < 7; i++)
	{
		pwm->my_pwm_mode[i] = STUDY;	
	}
}

static void change_mode_send(void)
{
	int i;
	for(i = 0; i < 7; i++)
	{
		pwm->my_pwm_mode[i] = SEND;
	}
}

/*
 *		pwm_shut说明:
 *			传入参数channel: 第几路pwm
 *
 */
static int pwm_shut(int channel)
{
	if(channel < 0 || channel > 7)
	{
		printk("channel out of range !\n");
		return -EINVAL;
	}
	spin_lock(&pwm_reg_lock);
	iowrite32(1 << channel, pwm->pwm_base + HW_PWM_CTRL_CLR);
	pwm->status[channel] = SHUT;
	spin_unlock(&pwm_reg_lock);
	return 0;
}

/*
 *		pwm_start说明:
 *			传入参数channel: 第几路pwm
 *
 */
static int pwm_start(int channel)
{
	if(channel < 0 || channel > 7)
	{
		printk("channel out of range!\n");
		return -EINVAL;
	}

	spin_lock(&pwm_reg_lock);
	iowrite32(START << channel, pwm->pwm_base + HW_PWM_CTRL_SET);
	pwm->status[channel] = START;
	spin_unlock(&pwm_reg_lock);

	return 0;
}

/*
 *	pwm_re_start说明:
 *		更改io的状态，从gpio状态改为pwm发送状态
 *
 */
static int pwm_re_start(int channel)
{
	spin_lock(&pwm_reg_lock);
	switch(channel)
	{
	case 0:
	case 1:
	case 2:
		iowrite32(0x3 << channel * 2, pwm->gpio_base + HW_PINCTRL_MUXSEL7_CLR);
		iowrite32(0x0 << channel * 2, pwm->gpio_base + HW_PINCTRL_MUXSEL7_SET);
		break;
	case 3:
	case 4:
	case 5:	
	case 6:
		iowrite32(0x3 << (channel * 2 + 2), pwm->gpio_base + HW_PINCTRL_MUXSEL7_CLR);
		iowrite32(0x1 << (channel * 2 + 2), pwm->gpio_base + HW_PINCTRL_MUXSEL7_SET);
		break;		
	case 7:
		iowrite32(0x3 << 20, pwm->gpio_base + HW_PINCTRL_MUXSEL7_CLR);
		iowrite32(0x1 << 20, pwm->gpio_base + HW_PINCTRL_MUXSEL7_SET);
		break;
	default:
		break;
	}
	spin_unlock(&pwm_reg_lock);
	return 0;
}


/*
 *	pwm_fake_shut:
 *		将pwm发送状态改为io口
 */
static int pwm_fake_shut(int channel)
{
	spin_lock(&pwm_reg_lock);
	switch(channel)
	{
	case 0:
	case 1:
	case 2:
		iowrite32(0x3 << channel * 2, pwm->gpio_base + HW_PINCTRL_MUXSEL7_CLR);
		iowrite32(0x3 << channel * 2, pwm->gpio_base + HW_PINCTRL_MUXSEL7_SET);
		break;
	case 3:
	case 4:
	case 5:	
	case 6:
		iowrite32(0x3 << (channel * 2 + 2), pwm->gpio_base + HW_PINCTRL_MUXSEL7_CLR);
		iowrite32(0x3 << (channel * 2 + 2), pwm->gpio_base + HW_PINCTRL_MUXSEL7_SET);
		break;		
	case 7:
		iowrite32(0x3 << 20, pwm->gpio_base + HW_PINCTRL_MUXSEL7_CLR);
		iowrite32(0x3 << 20, pwm->gpio_base + HW_PINCTRL_MUXSEL7_SET);
		break;
	default:
		break;
	}
	spin_unlock(&pwm_reg_lock);
	return 0;
}
/*
 *		pwm_release说明:
 *				引用记数减一
 */
int pwm_release(struct inode *inode, struct file *filp)
{
	pwm->count -= 1;
	return 0;
}

struct file_operations fops = {
	.open = pwm_open,
	.write = pwm_write,
	.read = pwm_read,
	.release = pwm_release,
	.ioctl = pwm_ioctl,
};


struct miscdevice pwm_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "pwm",
	.fops = &fops,
};


/*
 *		clear_active说明:
 *				对pwm的active寄存器清零
 *				传入参数channel: 第几路pwm
 */
static void clear_active(int channel)
{
	iowrite32(CLEAR_32, pwm->pwm_base + HW_PWM_ACTIVE0_CLR + channel * 0x20);
}

/*
 *		clear_period说明:
 *				对pwm的period寄存器清零
 *				传入参数channel: 第几路pwm
 */
static void clear_period(int channel)
{
	iowrite32(CLEAR_32, pwm->pwm_base + HW_PWM_PERIOD0_CLR + channel * 0x20);
}


/*
 *		init_active说明:
 *				对pwm的active寄存器初始化
 *				传入参数channel: 第几路pwm
 */
static void init_active(int channel)
{
	iowrite32((0x13a << 16) | 0x9d, pwm->pwm_base + HW_PWM_ACTIVE0_SET + channel * 0x20);
}


/*
 *		clear_gpio:
 *			将pwm的引脚清空
 *
 */
static void clear_gpio(void)
{
	iowrite32(0x30ff3f, pwm->gpio_base + HW_PINCTRL_MUXSEL7_CLR);
	iowrite32(3 << 22, pwm->gpio_base + HW_PINCTRL_MUXSEL2_CLR);
}

/*
 *		set_gpio:
 *			将8路pwm初始化为pwm状态
 *
 */
static void set_gpio(void)
{
	iowrite32(0x105500, pwm->gpio_base + HW_PINCTRL_MUXSEL7_SET);
	iowrite32(3 << 22, pwm->gpio_base + HW_PINCTRL_MUXSEL2_SET);
}

static void init_gpio(void)
{
	spin_lock(&pwm_reg_lock);
	clear_gpio();
	set_gpio();
	spin_unlock(&pwm_reg_lock);
}




/*
 *		init_period说明:
 *				对pwm的period寄存器初始化
 *				传入参数channel: 第几路pwm
 */
static void init_period(int channel)
{
	iowrite32(0x13a | 0x2 << 16 | 0x3 << 18 | 0x1 << 20, pwm->pwm_base + HW_PWM_PERIOD0_SET + channel * 0x20);
}



/*
 *		init_pwm_reg说明:
 *				初始化中断状态位
 *				初始化八路pwm
 */
static void init_pwm_reg(void)
{
	int i;
	pwm->my_condition = DOWN;

	spin_lock(&pwm_reg_lock);
	iowrite32(0xff, pwm->pwm_base + HW_PWM_CTRL_CLR);
	for(i = 0; i < 7; i++)
	{
		clear_active(i);
		clear_period(i);
		init_active(i);
		init_period(i);
	}
	spin_unlock(&pwm_reg_lock);
}


/*
 *		timer_on说明:
 *			传入参数i:	时间
 *			具体请参考头文件中的宏
 */
static void timer_on(int i)
{
	iowrite32(i, pwm->timer_base + HW_TIMROT_FIXED_COUNT1);
	iowrite32(0xf, pwm->timer_base + HW_TIMROT_TIMCTRL1_SET);
}

static void timer_off(void)
{
	iowrite32(1 << 15, pwm->timer_base + HW_TIMROT_TIMCTRL1_CLR);
	iowrite32(0xf, pwm->timer_base + HW_TIMROT_TIMCTRL1_CLR);
}

static void timer_study_on(int i)
{
	iowrite32(i, pwm->timer_base + HW_TIMROT_FIXED_COUNT2);
	iowrite32(0xf, pwm->timer_base + HW_TIMROT_TIMCTRL2_SET);
}

static void timer_study_off(void)
{
	iowrite32(1 << 15, pwm->timer_base + HW_TIMROT_TIMCTRL2_CLR);
	iowrite32(0xf, pwm->timer_base + HW_TIMROT_TIMCTRL2_CLR);
}


/*
 *		timer_clear_irq说明:
 *				清理中断标志位，防止重复进入同一个中断
 *				int num:	清理哪个timer的中断
 */
static void timer_clear_irq(int num)
{
	if(num == 1)
	{
		iowrite32(1 << 15, pwm->timer_base + HW_TIMROT_TIMCTRL1_CLR);
	}
	else
	{
		iowrite32(1 << 15, pwm->timer_base + HW_TIMROT_TIMCTRL2_CLR);
	}
}


/*
 *		学习状态中断:
 *			清中断。读取寄存器状态。
 *			判断是否与上一次读取的状态相同。
 *			如果相同，则记数+1, 
 *			如果大于结束码且不是第一次进入，立刻写0退出
 *			如果大于1秒，就写0退出
 *			如果与上一次读取状态不同，则退出
 */
irqreturn_t irq_study_timer_handler(int irq, void *dev_no)
{
    static unsigned int  study_data = 0;
    timer_clear_irq(TIMER_2);
    tmp_data[1] = read_io_status();
	
    if(tmp_data[0] == tmp_data[1]) //wave no change
    {
/*
		if(wave_begin == 0x0 && tmp_data[0] == 1)
		{
			study_data = 0;
            return IRQ_HANDLED;
		}
*/

        if(study_data == 0)
        {
            study_data = study_data + 2;
        }
        else
        {
            study_data++;		       
        }

//		study_data++;
//           _pwm_study_data += 1;
//		tmp_data[0] = tmp_data[1];

        if(study_data > 5500 && begin_flag != 0)//rev wave  long '0', study stop
        {
            _pwm_study_data = study_data;
            study_data = 0;
            timer_study_off();
            study_condition = UP;
            wake_up_interruptible(&study_wait);
            return IRQ_HANDLED;
        }	
        
        if(study_data >= ONE_S) //rev key long '0' or '1', study stop
        {
            _pwm_study_data = study_data;
            study_data = 0;
            timer_study_off();
            study_condition = UP;
            wake_up_interruptible(&study_wait);
            return IRQ_HANDLED;
        }
    }
    else//new wave is coming and old wav is end
    {
//		wave_begin++;
        _pwm_study_data = study_data;	
        study_data = 0;  	
        study_condition = UP;
        wake_up_interruptible(&study_wait);
    }
    tmp_data[0] = tmp_data[1];	
    return IRQ_HANDLED;
}
/*
 *		发送状态中断说明:
 *			每次发送，数据-1，清中断。判断时候退出。
 */
irqreturn_t irq_send_timer_handler(int irq, void *dev_no)
{
    static int send_data = FIRST_TIME_EMPTY;

    timer_clear_irq(TIMER_1);

    if(send_data == FIRST_TIME_EMPTY)
    {
        if(send_pwm_data_index >= SIZE)
        {
            printk("send_pwm_data_index > 1024 \n");
            return 	IRQ_HANDLED;
        }

        send_data = *(send_pwm_data + send_pwm_data_index);
		
        if(send_data == DATA_END_FLAG)
        {
        	wake_up_interruptible(&send_wait);
			pwm->my_condition = UP;
			timer_off();
			pwm_shut(send_channel);
            return IRQ_HANDLED;
        }
        
        if(send_pwm_data_index % 2 == 0 && send_pwm_data_index == 0)
        {
            pwm_start(send_channel);
        }
		else if(send_pwm_data_index % 2 == 0)
		{
			pwm_re_start(send_channel);
		}
        else
        {
			pwm_fake_shut(send_channel);
        }

        send_pwm_data_index++;//read index for next coming
        return IRQ_HANDLED;
    }
    
    send_data -- ;
    return 	IRQ_HANDLED;
}

/*
 *	timer_init:
 *		计时器初始化
 *		注册计时器timer1，timer2
 *		配置说明:
 *			打开中断使能
 *			分频设置为1（不分频）
 *			打开重新载入
 */

static int timer_init(void)
{
	int tmp;
	tmp = request_irq(IRQ_TIMER1, irq_send_timer_handler, IRQF_TIMER, "timer1", NULL);
	if(tmp)
	{
		printk("request timer1 irq error %d\n", tmp);
		return tmp;
	}

	tmp = request_irq(IRQ_TIMER2, irq_study_timer_handler, IRQF_TIMER, "timer2", NULL);
	if(tmp)
	{
		printk("request timer2 irq error %d\n", tmp);
		return tmp;
	}

	spin_lock(&pwm_reg_lock);
	iowrite32(CLEAR_LOW_16, pwm->timer_base + HW_TIMROT_TIMCTRL1_CLR);
	iowrite32((1 << 14) | (0x0 << 4) | (0x3 << 6), pwm->timer_base + HW_TIMROT_TIMCTRL1_SET);
	
	iowrite32(CLEAR_LOW_16, pwm->timer_base + HW_TIMROT_TIMCTRL2_CLR);
	iowrite32((1 << 14) | (0x0 << 4) | (0x3 << 6), pwm->timer_base + HW_TIMROT_TIMCTRL2_SET);

	spin_unlock(&pwm_reg_lock);

	return 0;

}



static int __devinit imx28_pwm_probe(struct platform_device *pdev)
{
	struct resource *pwm_res;
	int ret = 0;

	pwm = kzalloc(sizeof(struct pwm_device), GFP_KERNEL);
	if(pwm == NULL)
	{
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	pwm->pdev = pdev;

	pwm->buffer_read = kzalloc(SIZE, GFP_KERNEL);
	if(pwm->buffer_read == NULL)
	{
		printk("failed mem\n");
		goto err_free;
	}

	//*****************get pwm_resource*********************
	pwm_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(pwm_res == NULL)
	{
		dev_err(&pdev->dev, "no memory resource defined\n");
		ret = -ENODEV;
		goto err_free;
	}
	
	pwm_res = request_mem_region(pwm_res->start, pwm_res->end - pwm_res->start + 1, "pwm");
	if(pwm_res == NULL)
	{
		dev_err(&pdev->dev, "failed to request memory resource\n");
		ret = -EBUSY;
		goto err_free;
	}

	
	pwm->pwm_base = ioremap(pwm_res->start, pwm_res->end - pwm_res->start + 1);
	if(pwm->pwm_base == NULL)
	{
		dev_err(&pdev->dev, "failed to ioremap registers \n");
		ret = -ENODEV;
		goto err_free_mem;
	}
	//**********************************************************

	//***********get timer_address******************************
	pwm_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if(pwm_res == NULL)
	{
		dev_err(&pdev->dev, "no memory resource defined\n");
		ret = -ENODEV;
		goto err_free;
	}
	pwm_res = request_mem_region(pwm_res->start, pwm_res->end - pwm_res->start + 1, "timer");
	if(pwm_res == NULL)
	{
		dev_err(&pdev->dev, "failed to request memory resource\n");
		ret = -EBUSY;
		goto err_free;
	}
	pwm->timer_base = ioremap(pwm_res->start, pwm_res->end - pwm_res->start + 1);
	if(pwm->timer_base == NULL)
	{
		dev_err(&pdev->dev, "failed to ioremap registers \n");
		ret = -ENODEV;
		goto err_free_mem;
	}
	//**********************************************************


	//***********get gpio_address*******************************

	pwm_res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	if(pwm_res == NULL)
	{
		dev_err(&pdev->dev, "no memory resource defined\n");
		ret = -ENODEV;
		goto err_free;
	}
	pwm_res = request_mem_region(pwm_res->start, pwm_res->end - pwm_res->start + 1, "gpio");
	if(pwm_res == NULL)
	{
		dev_err(&pdev->dev, "failed to request memory resource\n");
		ret = -EBUSY;
		goto err_free;
	}
	pwm->gpio_base = ioremap(pwm_res->start, pwm_res->end - pwm_res->start + 1);
	if(pwm->gpio_base == NULL)
	{
		dev_err(&pdev->dev, "failed to ioremap registers \n");
		ret = -ENODEV;
		goto err_free_mem;
	}
	//************************************************************

	init_gpio();
	init_pwm_reg();
	timer_init();
	init_waitqueue_head(&send_wait);
	init_waitqueue_head(&study_wait);

	mutex_lock(&pwm_lock);
	list_add_tail(&pwm->node, &pwm_list);
	mutex_unlock(&pwm_lock);
	platform_set_drvdata(pdev, pwm);
	ret = misc_register(&pwm_device);

	return 0;
err_free_mem:
	release_mem_region(pwm_res->start, pwm_res->end - pwm_res->start + 1);
err_free:
	kfree(pwm->buffer_read);
	kfree(pwm);
	return ret;

}

static int __devexit imx28_pwm_remove(struct platform_device *pdev)
{
	printk("has been removed\n");
	iounmap(pwm->pwm_base);
	iounmap(pwm->timer_base);
	iounmap(pwm->gpio_base);
	kfree(pwm->buffer_read);
	kfree(pwm);
	free_irq(IRQ_TIMER1, NULL);
	free_irq(IRQ_TIMER2, NULL);
	pwm = NULL;

	misc_deregister(&pwm_device);

	return 0;
}

static struct platform_driver imx28_pwm_driver = {
	.driver = {
		.name = "pwm",
	},
	.probe = imx28_pwm_probe,
	.remove = __devexit_p(imx28_pwm_remove),
};

static int __init imx28_pwm_init(void)
{
	return platform_driver_register(&imx28_pwm_driver);
}
arch_initcall(imx28_pwm_init);
static void __exit imx28_pwm_exit(void)
{
	platform_driver_unregister(&imx28_pwm_driver);
}

module_exit(imx28_pwm_exit)


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Maximus_Tann");
