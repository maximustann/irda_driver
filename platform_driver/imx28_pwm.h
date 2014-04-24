#ifndef _IMX28_PWM_H
#define _IMX28_PWM_H

#define HW_TIMROT_BASE						0x80068000


#define HW_TIMROT_TIMCTRL1					 0x60
#define HW_TIMROT_TIMCTRL1_SET				 0X64
#define HW_TIMROT_TIMCTRL1_CLR				 0X68


#define HW_TIMROT_RUNNING_COUNT1			 0X70
#define HW_TIMROT_FIXED_COUNT1				 0X80

#define HW_TIMROT_TIMCTRL2					 0xa0
#define HW_TIMROT_TIMCTRL2_SET				 0Xa4
#define HW_TIMROT_TIMCTRL2_CLR				 0Xa8

#define HW_TIMROT_RUNNING_COUNT2			 0XB0
#define HW_TIMROT_FIXED_COUNT2				 0XC0

//************************************************************************************


#define HW_PWM_BASE								0x80064000

#define HW_PWM_CTRL                              0x0
#define HW_PWM_CTRL_SET                          0x4
#define HW_PWM_CTRL_CLR                          0x8
#define HW_PWM_CTRL_TOG                          0Xc

#define HW_PWM_ACTIVE0                           0x10
#define HW_PWM_ACTIVE0_SET                       0x14
#define HW_PWM_ACTIVE0_CLR                       0x18
#define HW_PWM_PERIOD0                           0x20
#define HW_PWM_PERIOD0_SET                       0x24
#define HW_PWM_PERIOD0_CLR                       0x28


#define HW_PWM_ACTIVE1                            0x30
#define HW_PWM_ACTIVE1_SET                        0x34
#define HW_PWM_ACTIVE1_CLR                        0x38
#define HW_PWM_PERIOD1                            0x40
#define HW_PWM_PERIOD1_SET                        0x44
#define HW_PWM_PERIOD1_CLR                        0x48


#define HW_PWM_ACTIVE2                            0x50
#define HW_PWM_ACTIVE2_SET                        0x54
#define HW_PWM_ACTIVE2_CLR                        0x58
#define HW_PWM_PERIOD2                            0x60
#define HW_PWM_PERIOD2_SET                        0x64
#define HW_PWM_PERIOD2_CLR                        0x68


#define HW_PWM_ACTIVE3                            0x70
#define HW_PWM_ACTIVE3_SET                        0x74
#define HW_PWM_ACTIVE3_CLR                        0x78
#define HW_PWM_PERIOD3	                          0x80
#define HW_PWM_PERIOD3_SET                        0x84
#define HW_PWM_PERIOD3_CLR                        0x88

#define HW_PWM_ACTIVE4                            0x90
#define HW_PWM_ACTIVE4_SET                        0x94
#define HW_PWM_ACTIVE4_CLR                        0x98
#define HW_PWM_PERIOD4                            0xA0
#define HW_PWM_PERIOD4_SET                        0xA4
#define HW_PWM_PERIOD4_CLR                        0xA8

#define HW_PWM_ACTIVE5                            0xB0
#define HW_PWM_ACTIVE5_SET                        0xB4
#define HW_PWM_ACTIVE5_CLR                        0xB8
#define HW_PWM_PERIOD5                            0xC0
#define HW_PWM_PERIOD5_SET                        0xC4
#define HW_PWM_PERIOD5_CLR                        0xC8

#define HW_PWM_ACTIVE6                            0xD0
#define HW_PWM_ACTIVE6_SET                        0xD4
#define HW_PWM_ACTIVE6_CLR                        0xD8
#define HW_PWM_PERIOD6                            0xE0
#define HW_PWM_PERIOD6_SET                        0xE4
#define HW_PWM_PERIOD6_CLR                        0xE8



#define HW_PWM_ACTIVE7                            0xF0
#define HW_PWM_ACTIVE7_SET                        0xF4
#define HW_PWM_ACTIVE7_CLR                        0xF8
#define HW_PWM_PERIOD7                            0x100
#define HW_PWM_PERIOD7_SET                        0x104
#define HW_PWM_PERIOD7_CLR                        0x108


//************************************gpio**************************
#define HW_PINCTRL_BASE                         0x80018000

#define HW_PINCTRL_MUXSEL2                      0x120
#define HW_PINCTRL_MUXSEL2_SET                  0x124
#define HW_PINCTRL_MUXSEL2_CLR                  0x128


#define HW_PINCTRL_MUXSEL3                      0x130
#define HW_PINCTRL_MUXSEL3_SET                  0x134
#define HW_PINCTRL_MUXSEL3_CLR                  0x138

#define HW_PINCTRL_MUXSEL6                      0x160
#define HW_PINCTRL_MUXSEL6_SET                  0x164
#define HW_PINCTRL_MUXSEL6_CLR                  0x168

#define HW_PINCTRL_MUXSEL7                      0x170
#define HW_PINCTRL_MUXSEL7_SET                  0x174
#define HW_PINCTRL_MUXSEL7_CLR                  0x178


#define HW_PINCTRL_DRIVE12						0x3c0
#define HW_PINCTRL_DRIVE12_SET					0x3c4
#define HW_PINCTRL_DRIVE12_CLR					0x3c8

#define HW_PINCTRL_DRIVE14_SET                  0xE4
#define HW_PINCTRL_DRIVE14_CLR                  0x3E8


#define HW_PINCTRL_PULL3						0x630
#define HW_PINCTRL_PULL3_SET					0x634
#define HW_PINCTRL_PULL3_CLR					0x648

#define HW_PINCTRL_DOUT3						0x730
#define HW_PINCTRL_DOUT3_SET					0x734
#define HW_PINCTRL_DOUT3_CLR					0x738

#define HW_PINCTRL_DIN1							0x910
#define HW_PINCTRL_DIN1_SET						0x914
#define HW_PINCTRL_DIN1_CLR						0x918

#define HW_PINCTRL_DIN3							0x930
#define HW_PINCTRL_DIN3_SET						0x934
#define HW_PINCTRL_DIN3_CLR						0x938

#define HW_PINCTRL_DOE1							0xb10
#define HW_PINCTRL_DOE1_SET						0xb14
#define HW_PINCTRL_DOE1_CLR						0xb18

#define HW_PINCTRL_DOE3							0xb30
#define HW_PINCTRL_DOE3_SET						0xb34
#define HW_PINCTRL_DOE3_CLR						0xb38

#define HW_GPMI_CTRL0                           0x8000C000
#define HW_GPMI_CTRL0_SET                       0x4
#define HW_GPMI_CTRL0_CLR                       0x8

//************************************************************************

#define START                                   1
#define SHUT                                    2
#define CHANGE_SHUT                             73
#define CHANGE_START                            74
#define CHANGE_MODE_STUDY						0x101
#define CHANGE_MODE_SEND						0x100
#define TEST									99

#define CLEAR_LOW_16                            0x0000ffff
#define CLEAR_32								0xffffffff
//************************************************************************

#define ONE_S									50000
#define MS										0xbb0

#define US20									0x1cc
//#define US20									0x100


//*************************************************************************

#define CHANNEL									8
#define PWM_CHANNEL								8
#define DATA_END_FLAG							0
#define FIRST_TIME_EMPTY						0
#define TIMER_2									2	
#define TIMER_1									1	

#endif // _IMX28_PWM_H
