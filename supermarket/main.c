/*头文件========================================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "bmp.h"
#include <linux/input.h>

/*全局变量定义========================================================================================================*/

//管理员账号链表
struct admin_list
{
	char id[10];
	char pwd[10];
	struct admin_list *next;
};

//管理员账号结点初始化
struct admin_list *new_node()
{
	struct admin_list *p = (struct admin_list *)malloc(sizeof(struct admin_list));
	if(p == NULL)
	{
		perror("malloc failed:");
		return 0;
	}
	
	p->next = NULL;
	
	return p;
}

//商品信息
struct stuff_info
{
	char income[3];  //进价
	char price[3];   //售价
	char number[3];  //数量
};

//存储各个商品数量
int stuff_a = 0;
int stuff_b = 0;
int stuff_c = 0;
int stuff_d = 0;
int stuff_e = 0;
int stuff_f = 0;

//存储各个商品售价
int sell_a = 0;
int sell_b = 0;
int sell_c = 0;
int sell_d = 0;
int sell_e = 0;
int sell_f = 0;

//存储各个商品进价
int income_a = 0;
int income_b = 0;
int income_c = 0;
int income_d = 0;
int income_e = 0;
int income_f= 0;

/*用户链表
//用户链表
struct stuff_list
{
	int stuff_a;
	int stuff_b;
	int stuff_c;
	int stuff_d;
	int stuff_e;
	int stuff_f;
	struct stuff_list *next;
};

//用户结点初始化
struct stuff_list *new_snode()
{
	struct stuff_list *p = (struct stuff_list *)malloc(sizeof(struct stuff_list));
	if(p == NULL)
	{
		perror("smalloc failed:");
		return 0;
	}
	
	p->stuff_a = 0;
	p->stuff_b = 0;
	p->stuff_c = 0;
	p->stuff_d = 0;
	p->stuff_e = 0;
	p->stuff_f = 0;
	p->next = NULL;
	
	return p;
}
*/

//插入结点
void insert_list(struct admin_list *p,struct admin_list *new)
{
	new->next = p->next;
	p->next = new;
}

/*界面========================================================================================================*/

//退出系统界面
void Goodbye()
{
	show_bmp("Goodbye.bmp");
}

//管理员登录界面
void admin_log()
{
	show_bmp("admin_login.bmp");
	admin_touch();
}

//管理员登录错误
void admin_login_wrong()
{
	show_bmp("admin_login_wrong.bmp");
	admin_login_wrong_touch();
}

//登录界面
void log_menu(char *src)
{
	show_bmp(src);
	login_touch();
}

//管理员超市界面
void admin_market()
{
	show_bmp("admin_market.bmp");
	admin_market_touch();
}

//消费者超市界面
void user_market()
{
	show_bmp("user_market.bmp");
	user_market_touch();
}

//消费者结算界面
void user_count()
{
	show_bmp("user_count.bmp");
	user_count_touch();
}

//消费者未购买
void user_buy_nothing()
{
	show_bmp("user_buy_nothing.bmp");
	user_buy_nothing_touch();
}

/*触摸处理========================================================================================================*/

//管理员登陆界面触摸
int admin_touch()
{
	struct admin_list *new;
	
	//打开显示屏的设备文件
	int fd = open("/dev/input/event0", O_RDWR);
	if(-1 == fd)
	{
		printf("open err!\n");
		return -1;
	}
	
	struct input_event data;
	int touch_x,touch_y;
	
	//账号、密码临时存放区
	char tmp_id[10] = {0};
	char tmp_pwd[10] = {0};
	
	while(1)
	{
		read(fd, &data, sizeof(data));
		
		/*=============================================================*/
		
		//按键事件
		if(data.type == EV_ABS)
		{
			if(data.code == ABS_X)
			{
				touch_x = data.value/1024.0*800;
				/*
				printf("touch_x：");
				printf("%d ",touch_x);
				printf("%d ",touch_x/1024*800);
				*/
			}
			
			if(data.code == ABS_Y)
			{
				touch_y = data.value/600.0*480;
				/*
				printf("touch_y：");
				printf("%d\n",touch_y);
				printf("%d ",touch_y/1024*800);
				*/
			}
			
			//登录
			if((touch_x > 220 && touch_x < 330) && (touch_y > 360 && touch_y < 390))
			{
				//打开管理员账号列表
				FILE *fp = fopen("admin_list.txt","r");
				if(fp == NULL)
				{
					perror("fopen failed:");
					return 0;
				}
				
				//创建新的结点
				new = new_node();
				
				char tibuf[10] = {0};
				char tpbuf[10] = {0};
				
				//读取管理员账号列表
				fread(tibuf,10,1,fp);
				fread(tpbuf,10,1,fp);				
				
				//printf("tmp_id = %s\ttmp_pwd = %s\n",tmp_id,tmp_pwd);
				//printf("tibuf = %s\ttpbuf = %s\n",tibuf,tpbuf);
				
				//读处信息与临时信息对比
				if((strcmp(tibuf,tmp_id) == 0) && (strcmp(tpbuf,tmp_pwd) == 0))
				{
					printf("Password Access！\nLoding...\n");
					fclose(fp);
					//进入管理员超市界面
					admin_market();
				}
				else
				{
					fclose(fp);
					//将new释放
					free(new);
					admin_login_wrong();
				}
			}
			
			//返回
			if((touch_x > 510 && touch_x < 620) && (touch_y > 360 && touch_y < 390))
			{
				//回到登录界面
				log_menu("login.bmp");
			}
			
			/*=============================================================*/
			
			//输入账号
			if((touch_x > 270 && touch_x < 620) && (touch_y > 210 && touch_y < 250))
			{
				//清空临时账号
				memset(tmp_id,0,10);
				//输入临时账号
				printf("输入账号：");
				scanf("%s",tmp_id);
				printf("临时账号已存在：%s\n",tmp_id);
			}
			
			//输入密码
			if((touch_x > 270 && touch_x < 620) && (touch_y > 280 && touch_y < 310))
			{
				//清空临时密码
				memset(tmp_pwd,0,10);
				//输入临时密码
				printf("输入密码：");
				scanf("%s",tmp_pwd);
				printf("临时密码已存在：%s\n",tmp_pwd);
			}
			
		}
	}
	
	if(close(fd))
	{
		printf("close err!\n");
		return -2;
	}
}

//管理员登录错误触摸
int admin_login_wrong_touch()
{
	//打开显示屏的设备文件
	int fd = open("/dev/input/event0", O_RDWR);
	if(-1 == fd)
	{
		printf("open err!\n");
		return -1;
	}
	
	struct input_event data;
	
	while(1)
	{
		read(fd,&data,sizeof(data));
		
		//按键事件
		if(data.type == EV_ABS)
		{
			//返回登录界面
			log_menu("login.bmp");
		}
	}
}

//管理员超市界面触摸
int admin_market_touch()
{
	//打开显示屏的设备文件
	int fd = open("/dev/input/event0", O_RDWR);
	if(-1 == fd)
	{
		printf("open err!\n");
		return -1;
	}
	
	struct stuff_info goods[6];  //商品
	struct input_event data;
	int touch_x,touch_y;
	
	//打开商品信息文件
	FILE *gfd = fopen("stuff_info.txt","r+");
	if(gfd == NULL)
	{
		printf("open stuff_info.txt failed!\n");
		return -1;
	}
	
	//============================显示商品信息==============================
	int i = 0;
	char buf[6];
	
	//读取所有商品信息
	fread(&goods[0],sizeof(struct stuff_info),1,gfd);
	fread(&goods[1],sizeof(struct stuff_info),1,gfd);
	fread(&goods[2],sizeof(struct stuff_info),1,gfd);
	fread(&goods[3],sizeof(struct stuff_info),1,gfd);
	fread(&goods[4],sizeof(struct stuff_info),1,gfd);
	fread(&goods[5],sizeof(struct stuff_info),1,gfd);
	
	//读取各个商品的信息并显示
	while(i < 6)
	{
		bzero(buf,6);
		//=========================================================================
		if(i == 0)
		{
			//================================显示进价=====================================
			{
				int c,ch;
				
				//将商品的进价信息转换为int型
				ch = (goods[i].income[0]-'0')*100 + (goods[i].income[1]-'0')*10 + (goods[i].income[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,80,220,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,92,220,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,104,220,12,10);
			}
			//================================显示售价=====================================
			{
				int c,ch;
			
				ch = (goods[i].price[0]-'0')*100 + (goods[i].price[1]-'0')*10 + (goods[i].price[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,80,238,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,92,238,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,104,238,12,10);
				
			}
			//================================显示数量=====================================
			{
				int c,ch,j = 1;

				//将商品的进价信息转换为int型
				ch = (goods[i].number[0]-'0')*100 + (goods[i].number[1]-'0')*10 + (goods[i].number[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,80,255,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,92,255,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,104,255,12,10);
				
			}
		}
		//==========================================================================
		if(i == 1)
		{
			//================================显示进价=====================================
			{
				int c,ch,j = 1;
				
				ch = (goods[i].income[0]-'0')*100 + (goods[i].income[1]-'0')*10 + (goods[i].income[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,270,220,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,282,220,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,294,220,12,10);

			}
			//================================显示售价=====================================
			{		
				int c,ch,j = 1;
				
				ch = (goods[i].price[0]-'0')*100 + (goods[i].price[1]-'0')*10 + (goods[i].price[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,270,238,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,282,238,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,294,238,12,10);
				
			}
			//================================显示数量=====================================
			{
				int c,ch,j = 1;
				
				ch = (goods[i].number[0]-'0')*100 + (goods[i].number[1]-'0')*10 + (goods[i].number[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,270,255,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,282,255,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,294,255,12,10);
				
			}
		}
		//==========================================================================
		if(i == 2)
		{
			//================================显示进价=====================================
			//读取第i+1个商品的进价并显示
			{
				int c,ch,j = 1;
				
				ch = (goods[i].income[0]-'0')*100 + (goods[i].income[1]-'0')*10 + (goods[i].income[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,460,220,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,472,220,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,484,220,12,10);
				
			}
			//================================显示售价=====================================
			{
				int c,ch,j = 1;
				
				ch = (goods[i].price[0]-'0')*100 + (goods[i].price[1]-'0')*10 + (goods[i].price[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,460,238,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,472,238,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,484,238,12,10);

			}
			//================================显示数量=====================================
			{
				int c,ch,j = 1;
			
				ch = (goods[i].number[0]-'0')*100 + (goods[i].number[1]-'0')*10 + (goods[i].number[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,460,255,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,472,255,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,484,255,12,10);
				
			}
		}
		//=========================================================================
		if(i == 3)
		{
			//================================显示进价=====================================
			{
				int c,ch,j = 1;
				
				ch = (goods[i].income[0]-'0')*100 + (goods[i].income[1]-'0')*10 + (goods[i].income[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,80,395,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,92,395,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,104,395,12,10);
				
			}
			//================================显示售价=====================================
			{
				int c,ch,j = 1;
				
				ch = (goods[i].price[0]-'0')*100 + (goods[i].price[1]-'0')*10 + (goods[i].price[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,80,411,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,92,411,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,104,411,12,10);
			}
			//================================显示数量=====================================
			{		
				int c,ch,j = 1;
				
				ch = (goods[i].number[0]-'0')*100 + (goods[i].number[1]-'0')*10 + (goods[i].number[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,80,430,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,92,430,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,104,430,12,10);
				
			}
		}
		//=========================================================================
		if(i == 4)
		{
			//================================显示进价=====================================
			{
				int c,ch,j = 1;
				
				ch = (goods[i].income[0]-'0')*100 + (goods[i].income[1]-'0')*10 + (goods[i].income[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,270,395,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,282,395,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,294,395,12,10);
				
			}
			//================================显示售价=====================================
			{
				int c,ch,j = 1;
				
				ch = (goods[i].price[0]-'0')*100 + (goods[i].price[1]-'0')*10 + (goods[i].price[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,270,411,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,282,411,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,294,411,12,10);
				
			}
			//================================显示数量=====================================
			//读取第i+1个商品的数量并显示
			{
				int c,ch,j = 1;
				
				ch = (goods[i].number[0]-'0')*100 + (goods[i].number[1]-'0')*10 + (goods[i].number[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,270,430,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,282,430,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,294,430,12,10);
				
			}
		}
		//=========================================================================
		if(i == 5)
		{
			//================================显示进价=====================================
			{
				int c,ch,j = 1;
				
				ch = (goods[i].income[0]-'0')*100 + (goods[i].income[1]-'0')*10 + (goods[i].income[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,460,395,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,472,395,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,484,395,12,10);
				
			}
			//================================显示售价=====================================
			//读取第i+1个商品的售价并显示
			{
				int c,ch,j = 1;
				
				ch = (goods[i].price[0]-'0')*100 + (goods[i].price[1]-'0')*10 + (goods[i].price[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,460,411,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,472,411,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,484,411,12,10);
			}
			//================================显示数量=====================================
			//读取第i+1个商品的数量并显示
			{
				int c,ch,j = 1;
			
				ch = (goods[i].number[0]-'0')*100 + (goods[i].number[1]-'0')*10 + (goods[i].number[2]-'0');
				//得到第1位数字
				c = ch/100;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,460,430,12,10);
				bzero(buf,6);
				//得到第2位数字
				c = ch/10%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,472,430,12,10);
				bzero(buf,6);
				//得到第3位数字
				c = ch%10;
				sprintf(buf,"g%d.bmp",c);
				show_bmp_any(buf,484,430,12,10);
				
			}
		}
		i++;
	}
	//=======================================================================
	
	while(1)
	{
		read(fd, &data, sizeof(data));
		
		//按键事件
		if(data.type == EV_ABS)
		{
			
			if(data.code == ABS_X)
			{
				touch_x = data.value/1024.0*800;
			}
			
			if(data.code == ABS_Y)
			{
				touch_y = data.value/600.0*480;
			}
			
			//返回到登录界面
			if((touch_x > 715 && touch_x < 790) && (touch_y > 10 && touch_y < 30))
			{
				//返回到登录界面
				log_menu("login.bmp");
			}
			
			//退出系统
			if((touch_x > 10 && touch_x < 110) && (touch_y > 10 && touch_y < 40))
			{
				//清空商品信息
				FILE *fp = fopen("stuff_info.txt","w");
				fclose(fp);
				//退出系统
				Goodbye();
				exit(0);
			}
			
			//================================修改======================================
			
			char m_buf[3];
			
			//修改西瓜进价
			if((touch_x > 80 && touch_x < 180) && (touch_y > 220 && touch_y < 230))
			{
				//清空西瓜进价显示
				show_bmp_any("Null.bmp",76,220,116,13);
				
				printf("请输入要修改的西瓜进价：");
				scanf("%s\n",m_buf);
				goods[0].income[0] = m_buf[0];
				goods[0].income[1] = m_buf[1];
				goods[0].income[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,0,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[0].income,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改西瓜售价
			if((touch_x > 80 && touch_x < 180) && (touch_y > 238 && touch_y < 250))
			{
				//清空西瓜售价显示
				show_bmp_any("Null.bmp",76,238,116,13);
				
				printf("请输入要修改的西瓜进价：");
				scanf("%s\n",m_buf);
				goods[0].price[0] = m_buf[0];
				goods[0].price[1] = m_buf[1];
				goods[0].price[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,3,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[0].price,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改西瓜数量
			if((touch_x > 80 && touch_x < 180) && (touch_y > 255 && touch_y < 267))
			{
				//清空西瓜数量显示
				show_bmp_any("Null.bmp",76,255,116,13);
				
				printf("请输入要修改的西瓜进价：");
				scanf("%s\n",m_buf);
				goods[0].number[0] = m_buf[0];
				goods[0].number[1] = m_buf[1];
				goods[0].number[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,6,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[0].number,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改葡萄进价
			if((touch_x > 270 && touch_x < 370) && (touch_y > 220 && touch_y < 230))
			{
				//清空葡萄进价显示
				show_bmp_any("Null.bmp",261,220,116,13);
				
				printf("请输入要修改的葡萄进价：");
				scanf("%s\n",m_buf);
				goods[1].income[0] = m_buf[0];
				goods[1].income[1] = m_buf[1];
				goods[1].income[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,9,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[1].income,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改葡萄售价
			if((touch_x > 270 && touch_x < 370) && (touch_y > 238 && touch_y < 250))
			{
				//清空葡萄售价显示
				show_bmp_any("Null.bmp",261,238,116,13);
				
				printf("请输入要修改的葡萄售价：");
				scanf("%s\n",m_buf);
				goods[1].price[0] = m_buf[0];
				goods[1].price[1] = m_buf[1];
				goods[1].price[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,12,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[1].price,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改葡萄数量
			if((touch_x > 270 && touch_x < 370) && (touch_y > 255 && touch_y < 267))
			{
				//清空葡萄数量显示
				show_bmp_any("Null.bmp",261,255,116,13);
				
				printf("请输入要修改的葡萄数量：");
				scanf("%s\n",m_buf);
				goods[1].number[0] = m_buf[0];
				goods[1].number[1] = m_buf[1];
				goods[1].number[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,15,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[1].number,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改芒果进价
			if((touch_x > 460 && touch_x < 560) && (touch_y > 220 && touch_y < 230))
			{
				//清空芒果进价显示
				show_bmp_any("Null.bmp",450,220,116,13);
				
				printf("请输入要修改的芒果进价：");
				scanf("%s\n",m_buf);
				goods[2].income[0] = m_buf[0];
				goods[2].income[1] = m_buf[1];
				goods[2].income[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,18,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[2].income,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改芒果售价
			if((touch_x > 460 && touch_x < 560) && (touch_y > 238 && touch_y < 250))
			{
				//清空芒果售价显示
				show_bmp_any("Null.bmp",450,238,116,13);
				
				printf("请输入要修改的芒果售价：");
				scanf("%s\n",m_buf);
				goods[2].price[0] = m_buf[0];
				goods[2].price[1] = m_buf[1];
				goods[2].price[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,21,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[2].price,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改芒果数量
			if((touch_x > 460 && touch_x < 560) && (touch_y > 255 && touch_y < 267))
			{
				//清空芒果数量显示
				show_bmp_any("Null.bmp",450,255,116,13);
				
				printf("请输入要修改的芒果数量：");
				scanf("%s\n",m_buf);
				goods[2].number[0] = m_buf[0];
				goods[2].number[1] = m_buf[1];
				goods[2].number[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,24,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[2].number,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改香蕉进价
			if((touch_x > 80 && touch_x < 180) && (touch_y > 395 && touch_y < 405))
			{
				//清空香蕉进价显示
				show_bmp_any("Null.bmp",76,395,116,13);
				
				printf("请输入要修改的香蕉进价：");
				scanf("%s\n",m_buf);
				goods[3].income[0] = m_buf[0];
				goods[3].income[1] = m_buf[1];
				goods[3].income[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,27,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[3].income,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改香蕉售价
			if((touch_x > 80 && touch_x < 180) && (touch_y > 411 && touch_y < 424))
			{
				//清空香蕉售价显示
				show_bmp_any("Null.bmp",76,411,116,13);
				
				printf("请输入要修改的香蕉售价：");
				scanf("%s\n",m_buf);
				goods[3].price[0] = m_buf[0];
				goods[3].price[1] = m_buf[1];
				goods[3].price[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,30,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[3].price,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改香蕉数量
			if((touch_x > 80 && touch_x < 180) && (touch_y > 430 && touch_y < 440))
			{
				//清空香蕉数量显示
				show_bmp_any("Null.bmp",76,430,116,13);
				
				printf("请输入要修改的香蕉数量：");
				scanf("%s\n",m_buf);
				goods[3].number[0] = m_buf[0];
				goods[3].number[1] = m_buf[1];
				goods[3].number[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,33,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[3].number,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改枇杷进价
			if((touch_x > 270 && touch_x < 370) && (touch_y > 395 && touch_y < 405))
			{
				//清空枇杷进价显示
				show_bmp_any("Null.bmp",261,395,116,13);
				
				printf("请输入要修改的枇杷进价：");
				scanf("%s\n",m_buf);
				goods[4].income[0] = m_buf[0];
				goods[4].income[1] = m_buf[1];
				goods[4].income[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,36,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[4].income,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改枇杷售价
			if((touch_x > 270 && touch_x < 370) && (touch_y > 411 && touch_y < 424))
			{
				//清空枇杷售价显示
				show_bmp_any("Null.bmp",261,411,116,13);
				
				printf("请输入要修改的枇杷售价：");
				scanf("%s\n",m_buf);
				goods[4].price[0] = m_buf[0];
				goods[4].price[1] = m_buf[1];
				goods[4].price[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,39,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[4].price,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改枇杷数量
			if((touch_x > 270 && touch_x < 370) && (touch_y > 430 && touch_y < 440))
			{
				//清空西瓜售价显示
				show_bmp_any("Null.bmp",261,430,116,13);
				
				printf("请输入要修改的枇杷数量：");
				scanf("%s\n",m_buf);
				goods[4].number[0] = m_buf[0];
				goods[4].number[1] = m_buf[1];
				goods[4].number[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,42,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[4].number,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改车厘子进价
			if((touch_x > 460 && touch_x < 560) && (touch_y > 395 && touch_y < 405))
			{
				//清空车厘子进价显示
				show_bmp_any("Null.bmp",450,395,116,13);
				
				printf("请输入要修改的车厘子进价：");
				scanf("%s\n",m_buf);
				goods[5].income[0] = m_buf[0];
				goods[5].income[1] = m_buf[1];
				goods[5].income[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,45,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[5].income,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改车厘子售价
			if((touch_x > 460 && touch_x < 560) && (touch_y > 411 && touch_y < 424))
			{
				//清空车厘子售价显示
				show_bmp_any("Null.bmp",450,411,116,13);
				
				printf("请输入要修改的车厘子进价：");
				scanf("%s\n",m_buf);
				goods[5].price[0] = m_buf[0];
				goods[5].price[1] = m_buf[1];
				goods[5].price[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,48,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[5].price,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//修改车厘子数量
			if((touch_x > 460 && touch_x < 560) && (touch_y > 430 && touch_y < 440))
			{
				//清空车厘子数量显示
				show_bmp_any("Null.bmp",450,430,116,13);
				
				printf("请输入要修改的车厘子数量：");
				scanf("%s\n",m_buf);
				goods[5].number[0] = m_buf[0];
				goods[5].number[1] = m_buf[1];
				goods[5].number[2] = m_buf[2];
				
				//获取文件光标
				fseek(gfd,51,SEEK_SET);
				
				//将修改的数据写入文件
				fwrite(goods[5].number,3,1,gfd);
				fseek(gfd,0,SEEK_SET);
			}
			
			//==========================================================================
			
		}
		
	}
	
}

//消费者超市界面触摸
int user_market_touch()
{
	show_bmp_any("0.bmp",135,440,28,28);
	show_bmp_any("0.bmp",235,440,28,28);
	show_bmp_any("0.bmp",335,440,28,28);
	show_bmp_any("0.bmp",435,440,28,28);
	show_bmp_any("0.bmp",535,440,28,28);
	show_bmp_any("0.bmp",635,440,28,28);
	
	//打开显示屏的设备文件
	int fd = open("/dev/input/event0", O_RDWR);
	if(-1 == fd)
	{
		printf("open err!\n");
		return -1;
	}
	
	struct stuff_info sf[6];
	struct input_event data;
	int touch_x,touch_y;
	int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
	
	FILE *fp = fopen("stuff_info.txt","r+");
	if(fp == NULL)
	{
		printf("open stuff_info.txt failed!\n");
		return 0;
	}
	
	//memset(&,0,sizoef(struct stuff_info));
	bzero(sf,0);
	
	int i = 0, j = 0;
	//读取所有商品信息
	fread(&sf[0],sizeof(struct stuff_info),1,fp);
	fread(&sf[1],sizeof(struct stuff_info),1,fp);
	fread(&sf[2],sizeof(struct stuff_info),1,fp);
	fread(&sf[3],sizeof(struct stuff_info),1,fp);
	fread(&sf[4],sizeof(struct stuff_info),1,fp);
	fread(&sf[5],sizeof(struct stuff_info),1,fp);
	
	while(1)
	{
		read(fd, &data, sizeof(data));
		
		//按键事件
		if(data.type == EV_ABS)
		{
			if(data.code == ABS_X)
			{
				touch_x = data.value/1024.0*800;
			}
			
			if(data.code == ABS_Y)
			{
				touch_y = data.value/600.0*480;
			}
			
			{
				//===========================获取商品数量给stuff_n===============================
				//从商品信息表读出商品信息
				while(i < 6)
				{
					if(i == 0)
					{
						//printf("sf[%d].number = %s\n",i,sf[0].number);
						int ch;

						ch = (sf[i].income[0]-'0')*100 + (sf[i].income[1]-'0')*10 + (sf[i].income[2]-'0');
						stuff_a = ch;
					}
					if(i == 1)
					{
						//printf("sf[%d].number = %s\n",i,sf[1].number);
						int ch;

						ch = (sf[i].income[0]-'0')*100 + (sf[i].income[1]-'0')*10 + (sf[i].income[2]-'0');
						stuff_b = ch;
					}
					
					if(i == 2)
					{
						//printf("sf[%d].number = %s\n",i,sf[2].number);
						int ch;

						ch = (sf[i].income[0]-'0')*100 + (sf[i].income[1]-'0')*10 + (sf[i].income[2]-'0');
						stuff_c = ch;
					}
					
					if(i == 3)
					{
						//printf("sf[%d].number = %s\n",i,sf[3].number);
						int ch;

						ch = (sf[i].income[0]-'0')*100 + (sf[i].income[1]-'0')*10 + (sf[i].income[2]-'0');
						stuff_d = ch;
					}
					
					if(i == 4)
					{
						//printf("sf[%d].number = %s\n",i,sf[4].number);
						int ch;

						ch = (sf[i].income[0]-'0')*100 + (sf[i].income[1]-'0')*10 + (sf[i].income[2]-'0');
						stuff_e = ch;
					}
					
					if(i == 5)
					{
						//printf("sf[%d].number = %s\n",i,sf[5].number);
						int ch;

						ch = (sf[i].income[0]-'0')*100 + (sf[i].income[1]-'0')*10 + (sf[i].income[2]-'0');
						stuff_f = ch;
					}
					i++;
				}
			}
			
			//===============================================================================
			
			//各类按键
			//============================================================
			//返回
			if((touch_x > 740 && touch_x < 790) && (touch_y > 30 && touch_y < 75))
			{
				//返回登录界面
				log_menu("login.bmp");
				
			}
			//============================================================
			char buf[30];
			//西瓜+1
			if((touch_x > 240 && touch_x < 260) && (touch_y > 160 && touch_y < 180))
			{
				//西瓜+1
				bzero(buf,30);
				a++;
				if(a > 9)
					a = 9;
				sprintf(buf,"%d.bmp",a);
				//显示购物数量
				show_bmp_any(buf,237,185,28,28);
				//显示购物栏图片
				if(a != 0)
				{
					show_bmp_any("f1.bmp",135,390,48,48);
					show_bmp_any(buf,135,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",135,390,48,48);
					show_bmp_any("0.bmp",135,440,28,28);
				}
			}
			
			//西瓜-1
			if((touch_x > 240 && touch_x < 260) && (touch_y > 220 && touch_y < 240))
			{
				//西瓜-1
				bzero(buf,30);
				a--;
				if(a < 0)
					a = 0;
				sprintf(buf,"%d.bmp",a);
				show_bmp_any(buf,237,185,28,28);
				//显示购物栏图片
				if(a != 0)
				{
					show_bmp_any("f1.bmp",135,390,48,48);
					show_bmp_any(buf,135,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",135,390,48,48);
					show_bmp_any("0.bmp",135,440,28,28);
				}
			}
			//============================================================
			//葡萄+1
			if((touch_x > 460 && touch_x < 480) && (touch_y > 160 && touch_y < 180))
			{
				//葡萄+1
				bzero(buf,30);
				b++;
				if(b > 9)
					b = 9;
				sprintf(buf,"%d.bmp",b);
				show_bmp_any(buf,460,185,28,28);
				//显示购物栏图片
				if(b != 0)
				{
					show_bmp_any("f2.bmp",235,390,48,48);
					show_bmp_any(buf,235,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",235,390,48,48);
					show_bmp_any("0.bmp",235,440,28,28);
				}
			}
			
			//葡萄-1
			if((touch_x > 460 && touch_x < 480) && (touch_y > 220 && touch_y < 240))
			{
				//葡萄-1
				bzero(buf,30);
				b--;
				if(b < 0)
					b = 0;
				sprintf(buf,"%d.bmp",b);
				show_bmp_any(buf,460,185,28,28);
				//显示购物栏图片
				if(b != 0)
				{
					show_bmp_any("f2.bmp",235,390,48,48);
					show_bmp_any(buf,235,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",235,390,48,48);
					show_bmp_any("0.bmp",235,440,28,28);
				}
			}
			//============================================================
			//芒果+1
			if((touch_x > 680 && touch_x < 700) && (touch_y > 160 && touch_y < 180))
			{
				//芒果+1
				bzero(buf,30);
				c++;
				if(c > 9)
					c = 9;
				sprintf(buf,"%d.bmp",c);
				show_bmp_any(buf,671,185,28,28);
				//显示购物栏图片
				if(c != 0)
				{
					show_bmp_any("f3.bmp",335,390,48,48);
					show_bmp_any(buf,335,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",335,390,48,48);
					show_bmp_any("0.bmp",335,440,28,28);
				}
			}
			
			//芒果-1
			if((touch_x > 680 && touch_x < 700) && (touch_y > 220 && touch_y < 240))
			{
				//芒果-1
				bzero(buf,30);
				c--;
				if(c < 0)
					c = 0;
				sprintf(buf,"%d.bmp",c);
				show_bmp_any(buf,671,185,28,28);
				//显示购物栏图片
				if(c != 0)
				{
					show_bmp_any("f3.bmp",335,390,48,48);
					show_bmp_any(buf,335,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",335,390,48,48);
					show_bmp_any("0.bmp",335,440,28,28);
				}
			}
			//============================================================
			//香蕉+1
			if((touch_x > 240 && touch_x < 260) && (touch_y > 270 && touch_y < 290))
			{
				//香蕉+1
				bzero(buf,30);
				d++;
				if(d > 9)
					d = 9;
				sprintf(buf,"%d.bmp",d);
				show_bmp_any(buf,237,299,28,28);
				//显示购物栏图片
				if(d != 0)
				{
					show_bmp_any("f4.bmp",435,390,48,48);
					show_bmp_any(buf,435,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",435,390,48,48);
					show_bmp_any("0.bmp",435,440,28,28);
				}
			}
			
			//香蕉-1
			if((touch_x > 240 && touch_x < 260) && (touch_y > 330 && touch_y < 350))
			{
				//香蕉-1
				bzero(buf,30);
				d--;
				if(d < 0)
					d = 0;
				sprintf(buf,"%d.bmp",d);
				show_bmp_any(buf,237,299,28,28);
				//显示购物栏图片
				if(d != 0)
				{
					show_bmp_any("f4.bmp",435,390,48,48);
					show_bmp_any(buf,435,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",435,390,48,48);
					show_bmp_any("0.bmp",435,440,28,28);
				}
			}
			//============================================================
			//枇杷+1
			if((touch_x > 460 && touch_x < 480) && (touch_y > 270 && touch_y < 290))
			{
				//枇杷+1
				bzero(buf,30);
				e++;
				if(e > 9)
					e = 9;
				sprintf(buf,"%d.bmp",e);
				show_bmp_any(buf,460,299,28,28);
				//显示购物栏图片
				if(e != 0)
				{
					show_bmp_any("f5.bmp",535,390,48,48);
					show_bmp_any(buf,535,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",535,390,48,48);
					show_bmp_any("0.bmp",535,440,28,28);
				}
			}
			
			//枇杷-1
			if((touch_x > 460 && touch_x < 480) && (touch_y > 330 && touch_y < 350))
			{
				//枇杷-1
				bzero(buf,30);
				e--;
				if(e < 0)
					e = 0;
				sprintf(buf,"%d.bmp",e);
				show_bmp_any(buf,460,299,28,28);
				//显示购物栏图片
				if(e != 0)
				{
					show_bmp_any("f5.bmp",535,390,48,48);
					show_bmp_any(buf,535,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",535,390,48,48);
					show_bmp_any("0.bmp",535,440,28,28);
				}
			}
			//============================================================
			//车厘子+1
			if((touch_x > 680 && touch_x < 700) && (touch_y > 275 && touch_y < 290))
			{
				//车厘子+1
				bzero(buf,30);
				f++;
				if(f > 9)
					f = 9;
				sprintf(buf,"%d.bmp",f);
				show_bmp_any(buf,671,299,28,28);
				//显示购物栏图片
				if(f != 0)
				{
					show_bmp_any("f6.bmp",635,390,48,48);
					show_bmp_any(buf,635,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",635,390,48,48);
					show_bmp_any("0.bmp",635,440,28,28);
				}
			}
			
			//车厘子-1
			if((touch_x > 680 && touch_x < 700) && (touch_y > 330 && touch_y < 350))
			{
				//车厘子-1
				bzero(buf,30);
				f--;
				if(f < 0)
					f = 0;
				sprintf(buf,"%d.bmp",f);
				show_bmp_any(buf,671,299,28,28);
				//显示购物栏图片
				if(f != 0)
				{
					show_bmp_any("f6.bmp",635,390,48,48);
					show_bmp_any(buf,635,440,28,28);
				}
				else
				{
					show_bmp_any("null.bmp",635,390,48,48);
					show_bmp_any("0.bmp",635,440,28,28);
				}
			}
			//============================================================
			//结算
			if((touch_x > 740 && touch_x < 790) && (touch_y > 390 && touch_y < 460))
			{
				
				//========================各个商品盈利情况============================
				
				while(j < 6)
				{
					if(j == 0)
					{
						int ch;

						ch = (sf[j].income[0]-'0')*100 + (sf[j].income[1]-'0')*10 + (sf[j].income[2]-'0');
						sell_a = ch;
					}
					if(j == 1)
					{
						int ch;

						ch = (sf[j].income[0]-'0')*100 + (sf[j].income[1]-'0')*10 + (sf[j].income[2]-'0');
						sell_b = ch;
					}
					
					if(j == 2)
					{
						int ch;

						ch = (sf[j].income[0]-'0')*100 + (sf[j].income[1]-'0')*10 + (sf[j].income[2]-'0');
						sell_c = ch;
					}
					
					if(j == 3)
					{
						int ch;

						ch = (sf[j].income[0]-'0')*100 + (sf[j].income[1]-'0')*10 + (sf[j].income[2]-'0');
						sell_d = ch;
					}
					
					if(j == 4)
					{
						int ch;

						ch = (sf[j].income[0]-'0')*100 + (sf[j].income[1]-'0')*10 + (sf[j].income[2]-'0');
						sell_e = ch;
					}
					
					if(j == 5)
					{
						int ch;

						ch = (sf[j].income[0]-'0')*100 + (sf[j].income[1]-'0')*10 + (sf[j].income[2]-'0');
						sell_f = ch;
					}
					j++;
				}
				
				//====================================================================
				
				//剩余数量
				stuff_a = stuff_a - a;
				stuff_b = stuff_b - b;
				stuff_c = stuff_c - c;
				stuff_d = stuff_d - d;
				stuff_e = stuff_e - e;
				stuff_f = stuff_f - f;
				
				if(stuff_a < 0)
				{
					printf("对不起，西瓜不够了，请重新选择！\n");
					user_market();
				}
				
				if(stuff_b < 0)
				{
					printf("对不起，葡萄不够了，请重新选择！\n");
					user_market();
				}
				
				if(stuff_c < 0)
				{
					printf("对不起，芒果不够了，请重新选择！\n");
					user_market();
				}
				
				if(stuff_d < 0)
				{
					printf("对不起，香蕉不够了，请重新选择！\n");
					user_market();
				}
				
				if(stuff_e < 0)
				{
					printf("对不起，枇杷不够了，请重新选择！\n");
					user_market();
				}
				
				if(stuff_f < 0)
				{
					printf("对不起，车厘子不够了，请重新选择！\n");
					user_market();
				}
				
				if(a || b || c || d || e || f)
				{
					printf("感谢您的购物，本次消费如下：\n");
					printf("西瓜：%d个\t花费：%d元\n",a,a*sell_a);
					printf("葡萄：%d个\t花费：%d元\n",b,b*sell_b);
					printf("芒果：%d个\t花费：%d元\n",c,c*sell_c);
					printf("香蕉：%d个\t花费：%d元\n",d,d*sell_d);
					printf("枇杷：%d个\t花费：%d元\n",e,e*sell_e);
					printf("车厘子：%d个\t花费：%d元\n",f,f*sell_f);
					int sum = a*sell_a + b*sell_b + c*sell_c + d*sell_d + e*sell_e + f*sell_f;
					printf("本次购物一共消费：%d元\n",sum);
					
					//用户结算界面
					user_count();
				}
				else
				{
					//提示未购买商品
					user_buy_nothing();
				}
			}
		}
	}
	
	if(close(fd))
	{
		printf("close err!\n");
		return -2;
	}
}

//消费者结算触摸
int user_count_touch()
{
	//打开显示屏的设备文件
	int fd = open("/dev/input/event0", O_RDWR);
	if(-1 == fd)
	{
		printf("open err!\n");
		return -1;
	}
	
	struct input_event data;
	int touch_x,touch_y;
	
	while(1)
	{
		read(fd,&data,sizeof(data));
		
		//按键事件
		if(data.type == EV_ABS)
		{
			//返回登录界面
			log_menu("login.bmp");
		}
	}
}

//消费者未购买触摸
int user_buy_nothing_touch()
{
	//打开显示屏的设备文件
	int fd = open("/dev/input/event0", O_RDWR);
	if(-1 == fd)
	{
		printf("open err!\n");
		return -1;
	}
	
	struct input_event data;
	int touch_x,touch_y;
	
	while(1)
	{
		read(fd,&data,sizeof(data));
		
		//按键事件
		if(data.type == EV_ABS)
		{
			//返回登录界面
			log_menu("login.bmp");
		}
	}
}

//登陆界面触摸
int login_touch()
{
	//================================初始化商品信息===================================
	struct stuff_info sl[6];
	int ret = sizeof(struct stuff_info)*6;
	char buf[ret];
	
	FILE *sfp = fopen("stuff_info.txt","r+");
	if(sfp == NULL)
	{
		perror("open stuff_info.txt failed:");
		return ;
	}
	
	//若商品信息文件没有数据则初始化
	if(fread(buf,ret,1,sfp) == 0)
	{
		int i = 1;
		while(i < 7)
		{
			printf("请输入第%d个的商品的信息(进价、售价、数量)：\n注：数量0~999\n",i);
			printf("输入格式：ddd\n");
			scanf("%s",sl[i-1].income);
			scanf("%s",sl[i-1].price);
			scanf("%s",sl[i-1].number);
			//写入到文件中
			fwrite(sl[i-1].income,3,1,sfp);
			fwrite(sl[i-1].price,3,1,sfp);
			fwrite(sl[i-1].number,3,1,sfp);
			i++;
		}
		if(i == 7)
			printf("输入完毕！\n");
	}
	
	fclose(sfp);
	
	//=================================================================================
	
	//打开显示屏的设备文件
	int fd = open("/dev/input/event0", O_RDWR);
	if(-1 == fd)
	{
		printf("open err!\n");
		return -1;
	}
	
	struct input_event data;
	int touch_x,touch_y;
	
	while(1)
	{
		read(fd,&data,sizeof(data));
		
		//按键事件
		if(data.type == EV_ABS)
		{
			if(data.code == ABS_X)
			{
				touch_x = data.value/1024.0*800;
			}
			
			if(data.code == ABS_Y)
			{
				touch_y = data.value/600.0*480;
			}
			
			//触摸范围在管理员登录范围
			if((touch_x > 510 && touch_x < 740) && (touch_y > 200 && touch_y < 255))
			{
				admin_log();
				break;
			}
			
			//触摸范围在消费者购物
			if((touch_x > 510 && touch_x < 740) && (touch_y > 300 && touch_y < 400))
			{
				user_market();
				break;
			}
		}
	}
}

/*主函数========================================================================================================*/

int main(int argc,char *argv[])
{
	//=================================创建一个管理员账号===============================
	struct admin_list *head,*new;
	
	//创建头结点
	head = new_node();
	
	//新建一个管理员账号
	new = new_node();
	printf("创建一个管理员账号...\n");
	printf("请输入账号：");
	scanf("%s",new->id);
	printf("请输入密码：");
	scanf("%s",new->pwd);
	
	FILE *fp = fopen("admin_list.txt","w");
	if(fp == NULL)
	{
		perror("open admin_list.txt failed:");
		return 0;
	}
	
	//将管理员账号插入结点
	insert_list(head,new);
	
	//printf("new->id = %s\tnew->pwd = %s\n",new->id,new->pwd);
	
	fwrite(new->id,10,1,fp);
	fwrite(new->pwd,10,1,fp);
	
	fclose(fp);
	//=================================================================================
	
	//登录界面
	log_menu("login.bmp");
}