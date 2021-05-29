#include "hx_drv_tflm.h"
#include "synopsys_wei_delay.h"
#include "synopsys_wei_gpio.h"
#define accel_scale 10

hx_drv_gpio_config_t hal_gpio_0;
hx_drv_gpio_config_t hal_gpio_1;
hx_drv_gpio_config_t hal_gpio_2;
hx_drv_gpio_config_t hal_led_r;
hx_drv_gpio_config_t hal_led_g;

int UART_BF[8]={0,0,0,0,0,0,0,0};

int start=1,stop=0,i=0,number=0;


typedef struct
{
  uint8_t symbol;
	uint32_t int_part;
	uint32_t frac_part;
} accel_type;

void GPIO_INIT(void);
void UART_T(int a);
void UART_R(void);

int main(int argc, char* argv[])
{
  int k=2;
  int32_t int_buf;
	accel_type accel_x, accel_y, accel_z;

  hx_drv_accelerometer_initial();
	hx_drv_led_off(HX_DRV_LED_RED);
  hx_drv_led_off(HX_DRV_LED_GREEN);
  hx_drv_uart_initial(UART_BR_115200);

  
  GPIO_INIT();

  hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);


  
  hx_drv_uart_print("6\n");
  hx_drv_uart_print("%d\n",start);
  while (1) 
  {
    uint32_t available_count = 0;
		float x, y, z;
		available_count = hx_drv_accelerometer_available_count();		

    if(stop==1)
    {		
    // UART_T(k);
		for (int i = 0; i < available_count; i++) 
		{
			hx_drv_accelerometer_receive(&x, &y, &z);
		}

  
		int_buf = x * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_x.symbol = '-';
		}
		else 
		{
			accel_x.symbol = '+';
		}

		accel_x.int_part = int_buf / accel_scale;
		accel_x.frac_part = int_buf % accel_scale;


		int_buf = y * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_y.symbol = '-';
		}
		else 
		{
			accel_y.symbol = '+';
		}
		
		accel_y.int_part = int_buf / accel_scale;
		accel_y.frac_part = int_buf % accel_scale;


		int_buf = z * accel_scale; //scale value
		if(int_buf < 0)
		{
			int_buf = int_buf * -1;
			accel_z.symbol = '-';
		}
		else 
		{
			accel_z.symbol = '+';
		}

		accel_z.int_part = int_buf / accel_scale;
		accel_z.frac_part = int_buf % accel_scale;

  if(number==1)
  {
		if(accel_x.int_part==1|| accel_x.frac_part>6){
		     hx_drv_led_on(HX_DRV_LED_RED);
         hx_drv_led_off(HX_DRV_LED_GREEN);
         k=1;
		}
		else if(accel_y.int_part==1|| accel_y.frac_part>6){
		     hx_drv_led_on(HX_DRV_LED_RED);
         hx_drv_led_on(HX_DRV_LED_GREEN);
         k=2;
		}
		else if(accel_z.int_part==1||accel_z.frac_part>6){
		     hx_drv_led_off(HX_DRV_LED_RED);
         hx_drv_led_on(HX_DRV_LED_GREEN);
         k=3;
		}
  }
  else if(number==2)
  {
    k=accel_x.frac_part;
  }
  else if(number==3)
  {
    k=accel_x.int_part;
  }
  else if(number==4)
  {
    k=accel_y.frac_part;
  }
  else if(number==5)
  {
    k=accel_y.int_part;
  }
  else if(number==6)
  {
    k=accel_z.frac_part;
  }
  else if(number==7)
  {
    k=accel_z.int_part;
  }
  else if(number==8)
  {
    if(accel_x.symbol=='+'){  k=128; }
    else{ k=0; }
  }
  else if(number==9)
  {
    if(accel_y.symbol=='+'){  k=128; }
    else{ k=0; }
  }
  else if(number==10)
  {
    if(accel_z.symbol=='+'){  k=128; }
    else{ k=0; }
  }
    UART_T(k);
    stop=0;
    start=1;
   } ////stop
   else
    {
      do 
      {
        hx_drv_gpio_get(&hal_gpio_0);
        start=hal_gpio_0.gpio_data;
      }while(start==1);
       UART_R();
       
    
    }
    

  }/////loop
}

void UART_R(void)
{
  for(i=0;i<8;i++)
  { 
    hal_delay_ms(90);
    hx_drv_gpio_get(&hal_gpio_0);
    start=hal_gpio_0.gpio_data;
    UART_BF[i]=start;
  }
   hal_delay_ms(90);
   hx_drv_gpio_get(&hal_gpio_0);
   stop=hal_gpio_0.gpio_data;
   if(UART_BF[0]==1 && UART_BF[1]==0 && UART_BF[2]==0 && UART_BF[3]==0){      number=1; }
   else if(UART_BF[0]==0 && UART_BF[1]==1 && UART_BF[2]==0 && UART_BF[3]==0){ number=2; }
   else if(UART_BF[0]==1 && UART_BF[1]==1 && UART_BF[2]==0 && UART_BF[3]==0){ number=3; }
   else if(UART_BF[0]==0 && UART_BF[1]==0 && UART_BF[2]==1 && UART_BF[3]==0){ number=4; }
   else if(UART_BF[0]==1 && UART_BF[1]==0 && UART_BF[2]==1 && UART_BF[3]==0){ number=5; }
   else if(UART_BF[0]==0 && UART_BF[1]==1 && UART_BF[2]==1 && UART_BF[3]==0){ number=6; }
   else if(UART_BF[0]==1 && UART_BF[1]==1 && UART_BF[2]==1 && UART_BF[3]==0){ number=7; }
   else if(UART_BF[0]==0 && UART_BF[1]==0 && UART_BF[2]==0 && UART_BF[3]==1){ number=8; }
   else if(UART_BF[0]==1 && UART_BF[1]==0 && UART_BF[2]==0 && UART_BF[3]==1){ number=9; }
   else if(UART_BF[0]==0 && UART_BF[1]==1 && UART_BF[2]==0 && UART_BF[3]==1){ number=10; }
}

void UART_T(int a)
{
  int UART_TR[8]={0,0,0,0,0,0,0,0};

  switch(a)
  {
  case 1:{    UART_TR[0]=1;  UART_TR[1]=0; UART_TR[2]=0; UART_TR[3]=0;             break; }
  case 2:{    UART_TR[0]=0;  UART_TR[1]=1; UART_TR[2]=0; UART_TR[3]=0;             break; }
  case 3:{    UART_TR[0]=1;  UART_TR[1]=1; UART_TR[2]=0; UART_TR[3]=0;             break; }
  case 4:{    UART_TR[0]=0;  UART_TR[1]=0; UART_TR[2]=1; UART_TR[3]=0;             break; }
  case 5:{    UART_TR[0]=1;  UART_TR[1]=0; UART_TR[2]=1; UART_TR[3]=0;             break; }
  case 6:{    UART_TR[0]=0;  UART_TR[1]=1; UART_TR[2]=1; UART_TR[3]=0;             break; }
  case 7:{    UART_TR[0]=1;  UART_TR[1]=1; UART_TR[2]=1; UART_TR[3]=0;             break; }
  case 8:{    UART_TR[0]=0;  UART_TR[1]=0; UART_TR[2]=0; UART_TR[3]=1;             break; }
  case 9:{    UART_TR[0]=1;  UART_TR[1]=0; UART_TR[2]=0; UART_TR[3]=1;             break; }
  case 128:{    UART_TR[7]=1;                                                      break; }
  default:{    UART_TR[0]=0;  UART_TR[1]=0; UART_TR[2]=0; UART_TR[3]=0;            break; }
  }

  for(i=0;i<10;i++)
  { 
    
    if(i==0)
    {
       hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
    }
    else if(i==9)
    {
       hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
    }
    else
    {
      if(UART_TR[i-1]==0){ hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);}
      else{ hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);}
    }
      hal_delay_ms(90);
    
    
  }

}

void GPIO_INIT(void)
{
  if(hal_gpio_init(&hal_gpio_0, HX_DRV_PGPIO_0, HX_DRV_GPIO_INPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO0 Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO0 Initialized: Error\n");

  if(hal_gpio_init(&hal_gpio_1, HX_DRV_PGPIO_1, HX_DRV_GPIO_OUTPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO1 Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO1 Initialized: Error\n");
}
