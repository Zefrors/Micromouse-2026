/*
 * irs.c
 */

#include "irs.h"
#include "delay.h"
#define IR_THRESH 500
// This is the buffer that will get filled up with all the measurements
uint16_t adc_buf[NUM_SAMPLES];
// "boolean" variable to keep say when the ADC has finished filling up the buffer
uint8_t complete = 0;

/*
 This function should handle everything for reading a specific IR
 First turn on the correct IR emitter
 Wait for a small amount of time (at least 20 us) so the photodiode can react
 Then read the correct receiver
 Lastly turn off the emitter
 */
uint16_t readIR(IR ir)
{

}

/*
 Implement the following 4 functions to read each IR
 Hint: Each function may just be one line
 */
uint16_t readLeftIR(void)
{
	if (analogRead(IR_LEFT) < IR_THRESH)
		return 1;
	return 0;
}

uint16_t readFrontIR(void)
{
	if (analogRead(IR_FRONT) < IR_THRESH)
		return 1;
	return 0;

}


uint16_t readRightIR(void)
{
	if (analogRead(IR_RIGHT) < IR_THRESH)
		return 1;
	return 0;

}

/*
 This function reads the specific channel of the ADC corresponding to the correct IR
 You should not have to edit this function
 */
uint16_t analogRead(IR ir)
{
    ADC_ChannelConfTypeDef sConfig = {0}; //this initializes the IR ADC [Analog to Digital Converter]
    ADC_HandleTypeDef *hadc1_ptr = Get_HADC1_Ptr(); //this is a pointer to your hal_adc
    //this pointer will also be used to read the analog value, val = HAL_ADC_GetValue(hadc1_ptr);

    //this picks the IR direction to choose the right ADC.
    switch(ir)
    {
        case IR_LEFT:
            sConfig.Channel = ADC_CHANNEL_14;
            break;
        case IR_LEFT_DIAG:
            sConfig.Channel = ADC_CHANNEL_5;
            break;
        case IR_RIGHT_DIAG:
            sConfig.Channel = ADC_CHANNEL_13;
            break;
        case IR_RIGHT:
            sConfig.Channel = ADC_CHANNEL_16;
            break;
        case IR_FRONT:
            sConfig.Channel = ADC_CHANNEL_15;
            break;
        default:
            return 0;
    }

    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLE_5;

    // make sure everything was set up correctly
    if (HAL_ADC_ConfigChannel(hadc1_ptr, &sConfig) != HAL_OK)
    {
        return 0;
    }
    
    complete = 0;

    // start filling up the ADC buffer
    HAL_ADC_Start_DMA(hadc1_ptr, (uint32_t*)adc_buf, NUM_SAMPLES);

    // wait for the buffer to become full
    while (complete == 0)
    {
        continue;
    }

    uint32_t sum = 0;
    // calculate the sum of the measurements in order to calculate the average
    uint16_t measurement = 0;
    while(measurement < NUM_SAMPLES) //this takes multiple measurements
    {
        sum += adc_buf[measurement];
        ++measurement;
    }

    return sum/NUM_SAMPLES;
}

/*
 This function is called when the ADC buffer is filled
 It stops the ADC and changes our "complete" variable to be "true"
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    // stop the ADC
    HAL_ADC_Stop_DMA(hadc);
    complete = 1;
}
