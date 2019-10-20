#include "stm32f4xx_hal.h"
#include "init.h"





extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;





void init_CS43L22()
{	
	uint8_t initV[2];
	
	
	
	//** CS43L22 datasheet: 4.9 Recommended Power-Up Sequence **//
	
		
		
		// Hold RESET low until the power supplies are stable.
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
			
			HAL_Delay(500);
		
		// Bring RESET high.
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
		
		// The default state of the “Power Ctl. 1” register (0x02) is 0x01. Load the desired register settings while
		// keeping the “Power Ctl 1” register set to 0x01.
		
		// Load the required initialization settings listed in Section 4.11.
			// Write 0x99 to register 0x00.
				initV[0] = 0x00; // Registro
				initV[1] = 0x99; // Valor
				if(HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50) != HAL_OK)     //check if transfer failed. If so: turn on Red LED
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				}
				
				
				
			// Write 0x80 to register 0x47.
				initV[0] = 0x47; // Registro
				initV[1] = 0x80; // Valor
				if(HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50) != HAL_OK)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				}

				
				
			// Write ‘1’b to bit 7 in register 0x32.
				initV[0] = 0x32; // Registro
				initV[1] = 0x80; // Valor
				if(HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50) != HAL_OK)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				}
				
				

			// Write ‘0’b to bit 7 in register 0x32.
				initV[0] = 0x32; // Registro
				initV[1] = 0x00; // Valor
				if(HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50) != HAL_OK)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				}

				
				
			// Write 0x00 to register 0x00.
				initV[0] = 0x00; // Registro
				initV[1] = 0x00; // Valor
				if(HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50) != HAL_OK)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
				}
				
			
			
	//** Finish required initialization **//
				
			
			
	//** Custom Configuration **//
	
	
					
		// Power Control 1 (Address 02h)
		// bit:		  7 	 6 		 5 	   4 		 3 		 2 		 1 		 0
		// name:	PDN7 	PDN6 	PDN5 	PDN4 	PDN3 	PDN2 	PDN1 	PDN0
		// value:	  1 	 0		 0		 1		 1		 1		 1		 0
		// State:	Power on
			initV[0] = 0x02; // Registro
			initV[1] = 0x9E; // Valor
			if( HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50)!= HAL_OK)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
			}
				
			
			
		// Power Control 2 (0x04)
		// bit:		   7 	   	   6 			  5 		    4 		     3 		    	2 		    	1 		 			0
		// name:	PDN_HPB1 	PDN_HPB0 	PDN_HPA1 	PDN_HPA0 	PDN_SPKB1 	PDN_SPKB0 	PDN_SPKA1 	PDN_SPKA0
		// value:	   1	   	   0			  1		      0		   		 0		    	0		 		  	0		 	 			0
		// State:	Headphone channel is always ON. Speaker channel is always OFF.
			initV[0] = 0x04; // Registro
			initV[1] = 0xA0; // Valor
			if( HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50)!= HAL_OK)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
			}
				
			
			
		// Interface Control 1 (0x06)
		// bit:		7 	 	   6 				5 		 	4 		   3 		   		2 		 		1 	 0
		// name:	M/~S 	INV_SCLK 	Reserved 	DSP 		DACDIF1 	DACDIF0 	AWL1 	AWL0
		// value:	0		   0					0		 		0		  	 0		   		1		 			1		 1
		// State:	Slave mode. SCLK Polarity Not Inverted. DSP Disabled. Format I²S. 16-bit data
			initV[0] = 0x06; // Registro
			initV[1] = 0x07; // Valor
			if( HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50)!= HAL_OK)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
			}

			
			
	//** Final required start **//
			
			
			
		// Set the “Power Ctl 1” register (0x02) to 0x9E.
			initV[0] = 0x02; // Registro
			initV[1] = 0x9E; // Valor
			if( HAL_I2C_Master_Transmit(&hi2c1, 0x94, initV, 2, 50)!= HAL_OK)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
			}
}
