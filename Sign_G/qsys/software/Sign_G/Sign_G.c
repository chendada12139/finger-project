#include <stdio.h>
#include <io.h>
#include <unistd.h>
#include "system.h"
#include "alt_types.h"
#include "altera_avalon_uart_regs.h"
#include "sys\alt_irq.h"
#include "stddef.h"
#include "stdarg.h"
#include <string.h>

alt_u8 RxCounter=0;
alt_u8 RxBuffer[11];
alt_u8 Res=0;

char send_data[600]={0};

/////////语音模块的数据
alt_u8 SYN_StopCom[]={0xFD,0X00,0X02,0X02,0XFD};     //停止合成
alt_u8 SYN_SuspendCom[]={0XFD,0X00,0X02,0X03,0XFC};   //暂停合成
alt_u8 SYN_RecoverCom[]={0XFD,0X00,0X02,0X04,0XFB};   //恢复合成
alt_u8 SYN_ChackCom[]={0XFD,0X00,0X02,0X21,0XDE};   //状态查询
alt_u8 SYN_PowerDownCom[]={0XFD,0X00,0X02,0X88,0X77}; //进入POWER DOWN 状态命令

float a[3]={0};
float w[3]={0};
float angle[3]={0};

float sample_v[5]={0};         //实时电压数据，AD采到的手指值
float sample_a[5]={0};         //实时角度数据，陀螺仪的值

float distance_v[43][2]={0};    //序号及电压记录表
float distance_a[7][2]={0};     //序号及电压记录表


float sheet_v[50][5]=       //////训练好的手指值   初始值：拇指（小） 食指（大） 中指 （大）无名指（大） 小指（大）   弯曲变反向
{

	{3.738, 3.719, 3.723, 3.639, 3.695},//  初始值

	{3.739, 3.720, 3.733, 3.598, 2.809},//  聋
	{0.021, 1.710, 0.016, 0.016, 0.359},

	{2.915, 2.964, 3.724, 2.905, 2.792},//  人
	{0.021, 1.829, 0.017, 0.016, 0.813},

	{3.670, 3.622, 3.647, 3.565, 3.631},//  教
	{0.020, 2.441, 0.016, 0.015, 0.331},

	{2.795, 2.784, 2.753, 2.602, 2.631},//  授
	{0.020, 2.289, 0.016, 0.015, 0.824},

	{2.972, 2.930, 2.638, 2.784, 2.792},//  朋友
	{0.021, 1.751, 0.016, 0.016, 0.420},

	{3.663, 3.648, 3.640, 2.860, 2.783},//  你们
	{0.020, 2.460, 0.016, 0.015, 0.408},

	{3.673, 3.652, 2.865, 2.841, 2.750},//  好
	{3.753, 3.733, 2.841, 2.868, 2.753},

	{3.561, 3.633, 3.627, 3.445, 3.466},//  欢迎
	{3.274, 3.289, 3.220, 3.121, 3.136},

	{3.682, 3.664, 3.632, 3.505, 3.600}, // 请
	{0.020, 2.350, 0.016, 0.016, 0.391},

	{3.632, 3.585, 3.580, 3.388, 3.534}, // 和
	{0.020, 1.774, 0.016, 0.015, 0.853},

	{3.243, 3.293, 3.066, 3.469, 3.546},  //联系
	{0.022, 2.633, 0.017, 0.017, 0.379},

	{2.921, 2.853, 3.125, 2.855, 2.818},  //沟
	{0.847, 2.658, 1.808, 1.589, 0.823},

	{2.942, 2.979, 3.670, 2.936, 2.819},  //通
	{0.836, 2.887, 2.336, 2.211, 0.596},

	{3.444, 3.461, 3.472, 3.288, 3.398},  //帮助
	{1.218, 2.889, 2.166, 3.088, 1.407},

	{3.735, 3.720, 3.107, 2.885, 2.799},  //克服
	{0.846, 2.958, 3.121, 2.276, 0.691},

	{3.735, 3.714, 3.213, 3.180, 3.182},  //困难
	{2.942, 2.871, 3.636, 3.447, 3.670},

	{3.735, 3.719, 2.888, 3.544, 3.606},//  完毕
	{3.741, 3.723, 2.956, 3.586, 3.630},

	{1.485, 2.099, 2.316, 2.144, 1.522},//
	{1.533, 2.149, 2.357, 2.136, 1.532},

	{0.819, 2.152, 2.345, 2.143, 1.460},//
	{0.714, 2.153, 2.296, 2.148, 1.438},

	{1.703, 3.054, 3.140, 3.264, 1.436},//
	{1.725, 3.092, 3.155, 3.066, 1.420},

	{1.435, 2.585, 2.250, 2.223, 0.686},//
	{1.506, 2.499, 2.208, 2.211, 0.581}
};

float sheet_a[50][2]=     /////训练好的陀螺仪的值
{
	{-0.81, 3.24},//初值

	{8.84, -74.56},//聋
	{10, 10},

	{25.39, -36.88},//人
	{10, 10},

	{81.07, -2.98},//教
	{10, 10},

	{86.51, 2.83},//授
	{10, 10},

	{81.24, -17.41},//朋友
	{100, 100},

	{81.56, -5.09},//你们
	{100, 100},

	{83.03, -1.14},//好
	{82.13, -15.50},

	{32.32, -85.88},  //欢迎
	{76.68, -84.02},

	{152.16, -8.03},  //请
	{50, 50},

	{6.95, -63.42}, //和
	{100, 100},

	{10.72, -23.34},  //联系
	{20, 20},

	{18.78, 8.22},  //沟
	{10, 10},

	{91.49, 1.94}, //通
	{10, 10},

	{-3.70, -49.35},//帮助
	{10, 10},

	{18.22, -32.56},//克服
	{10, 10},

	{158.62, -8.90},//困难
	{136.57, -12.86},

	{-6.80, -40.86},//完毕
	{7.61, -16.00},

	{10, 10},//
	{10, 10},

	{10, 10},//
	{10, 10},

	{10, 10},//
	{10, 10},

	{10, 10},//
	{10, 10}
};

void read_power(alt_u8 ch , alt_u16 *pRegValue);   ///AD转换函数

void uart_1_init();            //////串口1初始化函数
void uart_0_init();              //////串口0初始化函数
static void uart_isr(void * context,int id);   //////串口0中断函数;

void SYN_FrameInfo(alt_u8 *HZdata);      /////语音模块驱动程序
void PrintCom(alt_u8 *DAT,alt_u8 len);
void YS_SYN_Set(alt_u8 *Info_data);

/////AD 转换函数
void read_power(alt_u8 ch , alt_u16 *pRegValue)
{
 	int  Value;
 	// set measure number for ADC convert
 	IOWR(ADC_LTC2308_BASE, 0x01, 1);
 	// start measure
 	IOWR(ADC_LTC2308_BASE, 0x00, (ch << 1) | 0x00);
 	IOWR(ADC_LTC2308_BASE, 0x00, (ch << 1) | 0x01);
 	IOWR(ADC_LTC2308_BASE, 0x00, (ch << 1) | 0x00);
 	usleep(1);
 	while ((IORD(ADC_LTC2308_BASE,0x00) & 0x01) == 0x00);
 	Value = IORD(ADC_LTC2308_BASE, 0x01);
 	//printf("Value=%d\n",Value);
 	*pRegValue= Value;
 }

/////单字节发送函数，串口1驱动语音模块
void PrintCom(alt_u8 *DAT,alt_u8 len)
{
	alt_u8 i;
	for(i=0;i<len;i++)
	{
	   while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_1_BASE)& ALTERA_AVALON_UART_STATUS_TRDY_MSK));
	   IOWR_ALTERA_AVALON_UART_TXDATA(UART_1_BASE, DAT[i]);
	}
}

//////多字节发送函数，合成语音帧
void SYN_FrameInfo(alt_u8 *HZdata)
{
/****************需要发送的文本**********************************/
	 unsigned  char  Frame_Info[50];    //定义文本长度
	 unsigned  char  HZ_Length;
	 HZ_Length =strlen(HZdata); //需要发送的文本长度
/*****************帧固定信息配置**************************************/
	 Frame_Info[0] = 0xFD ; 			//帧头FD
	 Frame_Info[1] = 0x00 ; 			//数据长度高字节
	 Frame_Info[2] = HZ_Length + 6; 		//数据长度低字节
	 Frame_Info[3] = 0x01 ; 			//构造命令：合成播放语音
	 Frame_Info[4] = 0x01;      //文本格式：GBK
	 Frame_Info[5] = 0x5B;
	 Frame_Info[6] = 0x68;
	 Frame_Info[7] = 0x30;
	 Frame_Info[8] = 0x5D;
	 memcpy(&Frame_Info[9], HZdata, HZ_Length);
	 PrintCom(Frame_Info,9+HZ_Length);//发送帧配置
}

/*********************语音模块初始化******************************/
void YS_SYN_Set(alt_u8 *Info_data)
{
	alt_u8 Com_Len;
	Com_Len =strlen(Info_data);
	PrintCom(Info_data,Com_Len);
}

static void uart_isr(void * context,int id)     //串口0中断函数,接收陀螺仪的数据
{
	alt_u16 status;
	status = IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE);

	if(status & ALTERA_AVALON_UART_STATUS_RRDY_MSK)
	{
		Res= IORD_ALTERA_AVALON_UART_RXDATA(UART_0_BASE);
		RxBuffer[RxCounter]=Res;
		RxCounter++;

	   if(RxBuffer[0]!=0x55)	RxCounter=0;
	   else
	  	{
		   switch(RxBuffer[1])
			{
			   case 0x51:
				   {
					  a[0] = ((short)(RxBuffer[3]<<8 | RxBuffer[2]))/32768.0*16;
					  a[1] = ((short)(RxBuffer[5]<<8 | RxBuffer[4]))/32768.0*16;
					  a[2] = ((short)(RxBuffer[7]<<8 | RxBuffer[6]))/32768.0*16;
					  break;
				   }

			   case 0x52:
			      {
					  w[0] = ((short)(RxBuffer[3]<<8| RxBuffer[2]))/32768.0*2000;
					  w[1] = ((short)(RxBuffer[5]<<8| RxBuffer[4]))/32768.0*2000;
					  w[2] = ((short)(RxBuffer[7]<<8| RxBuffer[6]))/32768.0*2000;
					  break;
				  }

			   case 0x53:
	             {
					  angle[0] = ((short)(RxBuffer[3]<<8| RxBuffer[2]))/32768.0*180;
					  angle[1] = ((short)(RxBuffer[5]<<8| RxBuffer[4]))/32768.0*180;
					  angle[2] = ((short)(RxBuffer[7]<<8| RxBuffer[6]))/32768.0*180;
					  break;
				 }
			  default:  break;
			}
	  	  }

		if(RxCounter>10)    RxCounter=0;    //接收错误，重新开始
	}
}

void  USART_printf(char *fmt,...)          //发送屏幕显示内容
{
	alt_u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)send_data,fmt,ap);
	va_end(ap);
	i=strlen((const char*)send_data);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
	  while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE)& ALTERA_AVALON_UART_STATUS_TRDY_MSK));
		IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, send_data[j]);
	}
}

//串口0初始化//
void uart_0_init()
{
	alt_irq_register(UART_0_IRQ,NULL,uart_isr);      //注册串口中断服务程序
	IOWR_ALTERA_AVALON_UART_STATUS(UART_0_BASE,0);
	IOWR_ALTERA_AVALON_UART_CONTROL(UART_0_BASE,0X80);
}

//串口1初始化//
void uart_1_init()     //串口1发送语音给语音模块
{
	alt_irq_register(UART_1_IRQ,NULL,0);
	IOWR_ALTERA_AVALON_UART_STATUS(UART_1_BASE,0);
	IOWR_ALTERA_AVALON_UART_CONTROL(UART_1_BASE,0X80);
}

int main()
{
	USART_printf("CLS(0);\n");
	USART_printf("DS32(90,2,'基于DE10-Nano的',1);\n");usleep(200*2000);
	USART_printf("DS32(100,40,'手语翻译系统',1);\n");usleep(200*2000);
	USART_printf("DS24(2,120,'手语翻译显示:',2);\n");usleep(200*2000);
	//SYN_FrameInfo("基于FPGA的手语翻译系统");usleep(2000*2000);
	//SYN_FrameInfo("现在开始为您进行演示");usleep(2000*1000);

	float sum,d,m,f;
	char n;
	alt_u8 i,j,k,q=64;
	alt_u16 Data0,Data1,Data2,Data3,Data4;
	char szADC_Data[70]={0};
	uart_0_init();
	//usleep(100*1000);
	uart_1_init();
	while(1)
	{

			if( (-5<w[0])&&(w[0]<5)&&(-5<w[1])&&(w[1]<5)&&(-5<w[2])&&(w[2]<5))
			{
					usleep(8*1000);
					///读取AD、陀螺仪角度值
					if( (-5<w[0])&&(w[0]<5)&&(-5<w[1])&&(w[1]<5)&&(-5<w[2])&&(w[2]<5))
					{
						 sample_a[0]=angle[0];
						 sample_a[1]=angle[1];

						 read_power(0x00,&Data0);
						 read_power(0x01,&Data1);
						 read_power(0x02,&Data2);
						 read_power(0x03,&Data3);
						 read_power(0x04,&Data4);

						 sample_v[0]=(float)Data0/1000.0;
						 sample_v[1]=(float)Data1/1000.0;
						 sample_v[2]=(float)Data2/1000.0;
						 sample_v[3]=(float)Data3/1000.0;
						 sample_v[4]=(float)Data4/1000.0;
					}
					//相关性运算（距离，未开方）
		    	 	for(i=0;i<43;i++)
					  {
							for(j=0;j<5;j++)
								{
								  sum=sum+(sheet_v[i][j]-sample_v[j])*(sheet_v[i][j]-sample_v[j]);  ///5个手指头的误差平方和
								}
							distance_v[i][0]=i;
							distance_v[i][1]=sum;
							sum=0;
					  }
					 //冒泡排序，计算得出序号（第一列）

					 for(i=0;i<5;i++)
					  {
						   for(j=43;j>i;j--)
						   {
								if(distance_v[j-1][1]>distance_v[j][1])
								 {
									d=distance_v[j][1];     //电压交换
									n=distance_v[j][0];     //序号交换
									distance_v[j][1]=distance_v[j-1][1];
									distance_v[j][0]=distance_v[j-1][0];
									distance_v[j-1][1]=d;
									distance_v[j-1][0]=n;
								 }
						   }
					  }
					 //选取靠前的n个数据+电压距离约束（经验）
					  sum=0;
					  for(i=0;i<5;i++)
					  {
							k=distance_v[i][0];     //提取前k个序列号
							for(j=0;j<2;j++)
							 {
								sum=sum+(sheet_a[k][j]-sample_a[j])*(sheet_a[k][j]-sample_a[j]);
							 }
							 distance_a[i][0]=k;
							 distance_a[i][1]=sum;
							 sum=0;
					  }
					  //初始化数据
					  m=distance_a[0][1];
					  k=distance_a[0][0];
					 ////查找最小
					  for(i=0;i<5;i++)
					  {
						  if(distance_v[i][1]>=0.2)
							 break;
						   if(m>=distance_a[i][1])
							{
							   m=distance_a[i][1];
							   k=distance_a[i][0];
							}
					  }
					  	if(q!=k)
					  	{
							 q=k;  /////输出对应的中文
								USART_printf("BOXF(150,120,270,200,0);\n");
							 switch(k)
							 {
					 	 	 	case 1 : {SYN_FrameInfo("聋");USART_printf("DS48(170,130,'聋',5);\n");k=0;break;}

					 			case 2 : {SYN_FrameInfo("聋");USART_printf("DS48(170,130,'聋',5);\n");k=0;break;}

					 			case 3 : {SYN_FrameInfo("人");USART_printf("DS48(170,130,'人',5);\n");k=0;break;}

					 			case 4 : {SYN_FrameInfo("人");USART_printf("DS48(170,130,'人',5);\n");k=0;break;}

					 			case 5 : {SYN_FrameInfo("教");USART_printf("DS48(170,130,'教',5);\n");k=0;break;}

					 			case 6 : {SYN_FrameInfo("教");USART_printf("DS48(170,130,'教',5);\n");k=0;break;}
					 			case 7 : {SYN_FrameInfo("授");USART_printf("DS48(170,130,'授',5);\n");k=0;break;}
					 			case 8 : {SYN_FrameInfo("授");USART_printf("DS48(170,130,'授',5);\n");k=0;break;}
					 			case 9 : {SYN_FrameInfo("朋友");USART_printf("DS48(170,130,'朋友',5);\n");k=0;break;}
					 			case 10 : {SYN_FrameInfo("朋友");USART_printf("DS48(170,130,'朋友',5);\n");k=0;break;}
					 			case 11 : {SYN_FrameInfo("你们");USART_printf("DS48(170,130,'你们',5);\n");k=0;break;}
					 			case 12 : {SYN_FrameInfo("你们");USART_printf("DS48(170,130,''你们,5);\n");k=0;break;}
					 			case 13 : {SYN_FrameInfo("好");USART_printf("DS48(170,130,'好',5);\n");k=0;break;}
					 			case 14 : {SYN_FrameInfo("好");USART_printf("DS48(170,130,'好',5);\n");k=0;break;}
					 			case 15 : {SYN_FrameInfo("欢迎");USART_printf("DS48(170,130,'欢迎',5);\n");k=0;break;}
					 			case 16 : {SYN_FrameInfo("欢迎");USART_printf("DS48(170,130,'欢迎',5);\n");k=0;break;}
					 			case 17 : {SYN_FrameInfo("请");USART_printf("DS48(170,130,'请',5);\n");k=0;break;}
					 			case 18 : {SYN_FrameInfo("请");USART_printf("DS48(170,130,'请',5);\n");k=0;break;}
					 			case 19 : {SYN_FrameInfo("和");USART_printf("DS48(170,130,'和',5);\n");k=0;break;}
					 			case 20 : {SYN_FrameInfo("和");USART_printf("DS48(170,130,'和',5);\n");k=0;break;}
					 			case 21 : {SYN_FrameInfo("联系");USART_printf("DS48(170,130,'联系',5);\n");k=0;break;}
					 			case 22 : {SYN_FrameInfo("联系");USART_printf("DS48(170,130,'联系',5);\n");k=0;break;}
					 			case 23 : {SYN_FrameInfo("沟");USART_printf("DS48(170,130,'沟',5);\n");k=0;break;}
					 			case 24 : {SYN_FrameInfo("沟");USART_printf("DS48(170,130,'沟',5);\n");k=0;break;}
					 			case 25 : {SYN_FrameInfo("通");USART_printf("DS48(170,130,'通',5);\n");k=0;break;}
					 			case 26 : {SYN_FrameInfo("通");USART_printf("DS48(170,130,'通',5);\n");k=0;break;}
					 			case 27 : {SYN_FrameInfo("帮助");USART_printf("DS48(170,130,'帮助',5);\n");k=0;break;}
								case 28 : {SYN_FrameInfo("帮助");USART_printf("DS48(170,130,'帮助',5);\n");k=0;break;}
								case 29 : {SYN_FrameInfo("克服");USART_printf("DS48(170,130,'克服',5);\n");k=0;break;}
								case 30 : {SYN_FrameInfo("克服");USART_printf("DS48(170,130,'克服',5);\n");k=0;break;}
								case 31 : {SYN_FrameInfo("困难");USART_printf("DS48(170,130,'困难',5);\n");k=0;break;}
								case 32 : {SYN_FrameInfo("困难");USART_printf("DS48(170,130,'困难',5);\n");k=0;break;}
								case 33 : {SYN_FrameInfo("完毕");USART_printf("DS48(170,130,'完毕',5);\n");k=0;break;}
								case 34 : {SYN_FrameInfo("完毕");USART_printf("DS48(170,130,'完毕',5);\n");k=0;break;}
								case 35 : {SYN_FrameInfo("");USART_printf("DS48(170,130,'',5);\n");k=0;break;}
								case 36 : {SYN_FrameInfo("");USART_printf("DS48(170,130,'',5);\n");k=0;break;}
								case 37 : {SYN_FrameInfo("");USART_printf("DS48(170,130,'',5);\n");k=0;break;}
								case 38 : {SYN_FrameInfo("");USART_printf("DS48(170,130,'',5);\n");k=0;break;}
								case 39 : {SYN_FrameInfo("");USART_printf("DS48(170,130,'',5);\n");k=0;break;}
								case 40 : {SYN_FrameInfo("");USART_printf("DS48(170,130,'',5);\n");k=0;break;}
								case 41 : {SYN_FrameInfo("");USART_printf("DS48(160,130,'',5);\n");k=0;break;}
								case 42 : {SYN_FrameInfo("");USART_printf("DS48(160,130,'',5);\n");k=0;break;}

					 		}
					  	}
					}

		    printf("%.3f, ",(float)Data0/1000.0);
			printf("%.3f, ",(float)Data1/1000.0);
			printf("%.3f, ",(float)Data2/1000.0);
			printf("%.3f, ",(float)Data3/1000.0);
			printf("%.3f\n",(float)Data4/1000.0);


			printf("%.2f, ",angle[0]);
			printf("%.2f\n",angle[1]);
			/*printf("%.2f, ",a[0]);
			printf("%.2f, ",a[1]);
			printf("%.2f, ",a[2]);
			printf("%.2f, ",w[0]);
			printf("%.2f, ",w[1]);
			printf("%.2f\n",w[2]);
			*/
		    //printf("over\n");
               //////将AD采集到的数据、陀螺仪的数据通过串口1发送出来，用于手势训练，串口1连接串口助手
			/*sprintf(szADC_Data,"%.3f, %.3f, %.3f, %.3f, %.3f, %.2f, %.2f",(float)Data0/1000.0,(float)Data1/1000.0,(float)Data2/1000.0,(float)Data3/1000.0,(float)Data4/1000.0,angle[0],angle[1]);

			for(i=0;i<66;i++)
			{
				while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE)& ALTERA_AVALON_UART_STATUS_TRDY_MSK));
				IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, szADC_Data[i]);
			}*/
			//usleep(200*1000);
    }   // while出口

	return 0;
}    ///主函数出口

