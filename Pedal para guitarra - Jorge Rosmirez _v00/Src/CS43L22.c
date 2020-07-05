#include "main.h"
#include "CS43l22.h"


extern I2C_HandleTypeDef hi2c1;

void CS43L22_transmit(uint8_t registro, uint8_t valor)
{
	uint8_t data [2]; //necesito 2 bytes uno para el registro y otro para el dato que quiero poner en ese registro
	data[0]=registro; //registro
	data[1]=valor; //dato o valor
	HAL_I2C_Master_Transmit(&hi2c1,0x94, data, 2, 50);
}

void CS43L22_init(void)
{	HAL_GPIO_WritePin(Audio_RST_GPIO_Port,Audio_RST_Pin,GPIO_PIN_RESET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(Audio_RST_GPIO_Port,Audio_RST_Pin,GPIO_PIN_SET);
	/*Required Initialization Settings: inicializacion de DAC, datasheet pag 32*/
	CS43L22_transmit(0x00,0x99);
	CS43L22_transmit(0x47,0x80);
	CS43L22_transmit(0x32,0x80);
	CS43L22_transmit(0x32,0x00);
	CS43L22_transmit(0x00,0x00);
	CS43L22_transmit(0x02,0x9E);
	//configuracion de mi proyecto
	CS43L22_transmit(0x04,0xA0);	//Headphone channel is always ON. Speaker channel is always OFF.	
	CS43L22_transmit(0x06,0x07);  //Slave mode. SCLK Polarity Not Inverted. DSP Disabled. Format I2S. 16-bit data
	//Final required start
	CS43L22_transmit(0x02,0x9E);
}
