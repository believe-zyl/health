/*https://blog.csdn.net/weixin_45633206/article/details/110276226*/
#include "max30102.h"
#include "myiic.h"



/**
* \brief        Initialize the MAX30102
* \par          Details
*               This function initializes the MAX30102
* \param        None
* \retval       true on success
*/
bool max30102_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;              //PB5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //PA5���óɸ������룬��MAX30102��INT����ת���Ƿ���ɵ��ź�����  
	GPIO_Init(GPIOB, &GPIO_InitStructure);                  //��ʼ��GPIOB5

    if(!max30102_write_reg(REG_INTR_ENABLE_1, 0xc0))// INTR setting
        return false;
    if(!max30102_write_reg(REG_INTR_ENABLE_2, 0x00))
        return false;

    if(!max30102_write_reg(REG_FIFO_WR_PTR, 0x00))  //FIFO_WR_PTR[4:0]
        return false;
    if(!max30102_write_reg(REG_OVF_COUNTER, 0x00))  //OVF_COUNTER[4:0]
        return false;
    if(!max30102_write_reg(REG_FIFO_RD_PTR, 0x00))  //FIFO_RD_PTR[4:0]
        return false;

    if(!max30102_write_reg(REG_FIFO_CONFIG, 0xEF))  //sample avg = 8, fifo rollover=false, fifo almost full = 17
        return false;
    if(!max30102_write_reg(REG_MODE_CONFIG, 0x03))  //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
        return false;
    if(!max30102_write_reg(REG_SPO2_CONFIG, 0x2F))  //SPO2_ADC range = 4096nA, SPO2 sample rate (400 Hz), LED pulseWidth (411uS)
        return false;

    if(!max30102_write_reg(REG_LED1_PA, 0x17))  //Choose value for ~ 4.5mA for LED1
        return false;
    if(!max30102_write_reg(REG_LED2_PA, 0x17))  //Choose value for ~ 4.5mA for LED2
        return false;
    // if(!max30102_write_reg(REG_PILOT_PA, 0x7f)) //Choose value for ~ 25mA for Pilot LED
        // return false;
    return true;
}

/**
* \brief        Write a value to a MAX30102 register
* \par          Details
*               This function writes a value to a MAX30102 register
* \param[in]    uch_addr    - register address
* \param[in]    uch_data    - register data
* \retval       true on success
*/
bool max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
{
    /* ��1��������I2C���������ź� */
    IIC_Start();

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(max30102_WRITE_ADDR);	/* �˴���дָ�� */

    /* ��3��������ACK */
    if(IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ */
    IIC_Send_Byte(uch_addr);

    if(IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    /* ��5������ʼд������ */
    IIC_Send_Byte(uch_data);

    /* ��6��������ACK */
    if(IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    /* ����I2C����ֹͣ�ź� */
    IIC_Stop();
    return true;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    IIC_Stop();
    return false;
}

/**
* \brief        Read a MAX30102 register
* \par          Details
*               This function reads a MAX30102 register
* \param[in]    uch_addr    - register address
* \param[out]   puch_data    - pointer that stores the register data
* \retval       true on success
*/
bool max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
{
    /* ��1��������I2C���������ź� */
    IIC_Start();

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(max30102_WRITE_ADDR);	/* �˴���дָ�� */

    /* ��3��������ACK */
    if(IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ�� */
    IIC_Send_Byte((uint8_t)uch_addr);

    /* ��5��������ACK */
    if(IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    /* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
    IIC_Start();

    /* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(max30102_READ_ADDR);	/* �˴��Ƕ�ָ�� */

    /* ��8��������ACK */
    if (IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    /* ��9������ȡ���� */
    *puch_data = IIC_Read_Byte();	/* ��1���ֽ� */

    /* ��10��������NACK */
    IIC_NAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */

    /* ����I2C����ֹͣ�ź� */
    IIC_Stop();
    return true;	/* ִ�гɹ� ����dataֵ */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    IIC_Stop();
    return false;
}

/**
* \brief        Read a set of samples from the MAX30102 FIFO register
* \par          Details
*               This function reads a set of samples from the MAX30102 FIFO register
* \param[out]   *pun_red_led   - pointer that stores the red LED reading data
* \param[out]   *pun_ir_led    - pointer that stores the IR LED reading data
* \retval       true on success
*/
bool max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
{
    uint32_t un_temp;
    uint8_t uch_temp;
    *pun_ir_led = 0;
    *pun_red_led = 0;
    max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);    //ͨ����ȡ�Ĵ���������ж�״̬/�൱���жϱ�־��ʼ��
    max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);    //ͨ����ȡ�Ĵ���������ж�״̬/�൱���жϱ�־��ʼ��

    /* ��1��������I2C���������ź� */
    IIC_Start();

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(max30102_WRITE_ADDR);	/* �˴���дָ�� */

    /* ��3��������ACK */
    if(IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ�� */
    IIC_Send_Byte((uint8_t)REG_FIFO_DATA);

    /* ��5��������ACK */
    if(IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    /* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
    IIC_Start();

    /* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(max30102_READ_ADDR);	/* �˴��Ƕ�ָ�� */

    /* ��8��������ACK */
    if(IIC_Wait_Ack() != 0) {
        goto cmd_fail;	/* ������Ӧ�� */
    }

    un_temp = IIC_Read_Byte();
    IIC_Ack();
    un_temp <<= 16;
    *pun_red_led += un_temp;
    un_temp = IIC_Read_Byte();
    IIC_Ack();
    un_temp <<= 8;
    *pun_red_led += un_temp;
    un_temp = IIC_Read_Byte();
    IIC_Ack();
    *pun_red_led += un_temp;

    un_temp = IIC_Read_Byte();
    IIC_Ack();
    un_temp <<= 16;
    *pun_ir_led += un_temp;
    un_temp = IIC_Read_Byte();
    IIC_Ack();
    un_temp <<= 8;
    *pun_ir_led += un_temp;
    un_temp = IIC_Read_Byte();
    IIC_NAck();     /* ��������������� */
    *pun_ir_led += un_temp;
    *pun_red_led &= 0x03FFFF;   //Mask MSB [23:18]
    *pun_ir_led &= 0x03FFFF;    //Mask MSB [23:18]

    /* ����I2C����ֹͣ�ź� */
    IIC_Stop();
    return true;
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    IIC_Stop();
    return false;
}

/**
* \brief        Reset the MAX30102
* \par          Details
*               This function resets the MAX30102
* \param        None
* \retval       true on success
*/
bool max30102_reset(void)
{
    if(!max30102_write_reg(REG_MODE_CONFIG, 0x40))
        return false;
    else
        return true;
}



