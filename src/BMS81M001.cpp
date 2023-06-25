
/**********************************************************************
File:       		BMS81M001.cpp
Author:        	BESTMODULES
Description:    MPU6050 Wake on Vibration Module
History：			
	V1.0.1	 -- initial version；2023-05-10；Arduino IDE : v1.8.13

***********************************************************************/
#include "BMS81M001.h"


/**********************************************************
Description: Constructor
Parameters:       @ TwoWire : Wire object you use 
                  statusPin: int pin     
Return:          
Others:     
**********************************************************/
BMS81M001::BMS81M001(uint8_t statusPin,TwoWire *theWire)
{
    _wire = theWire;
    _intPin = statusPin;
}

/**********************************************************
Description: Module Initial
Parameters:      I2C_Clock:baud rate(The default baud rate:100000)         
Return:          
Others:          
**********************************************************/
void BMS81M001::begin(uint32_t I2C_Clock)
{
     _wire->begin(); 
     _wire->setWireTimeout(25000,true);
     _wire->setClock(I2C_Clock);  
     wakeup();
}

/*********************************************************************************
Description: Module reset(set motion thr,dur,delay and sensor setting to initial)
Parameters:               
Return:      Communication status  0:Success 1:Fail      
Others:              
**********************************************************************************/
uint8_t BMS81M001::reset(void)
{
    uint8_t length;
    int status;
    uint8_t txbuf[5];
    int rxbuf[6];
    length = 0x02;
    txbuf[0]=length;
    txbuf[1]=CMD_RESETMODULE;
    txbuf[2] = (uint8_t)(~( txbuf[0]+ txbuf[1]));
    writeBytes(txbuf,length+1);  
    delay(1);
    if(!(readBytes(rxbuf,4)))
    {
        status = rxbuf[2];
         if(status == 0x01)
        {
            return SUCCESS;
        }
        else
        {
            return FAIL;
        }
    }
    else
    {
        return FAIL;
    }                             
}

/***************************************************************************************
Description:  Enter low-power accel-only mode.
              In low-power accel mode, the chip goes to sleep and only wakes up to sample
              the accelerometer at one of the following frequencies:
              5Hz, 20Hz, 40Hz
              Requesting a rate not listed above, the device will disable low-power mode        
Parameters:   rate: WAKE_40Hz/WAKE_20Hz/WAKE_5Hz
              isEnable:0：OFF  1：ON  
Return:      Communication status  0:Success 1:Fail      
Others:              
*****************************************************************************************/
uint8_t BMS81M001::setWorkMode(uint8_t rate,uint8_t isEnable)
{

    uint8_t length;
    uint8_t status;
    uint8_t txbuf[4];
    int     rxbuf[6];   
    if(rate>4)
    {
        rate = OFF;
    }
    length = 0x03;
    txbuf[0]=length;
    txbuf[1]=CMD_LPMODE;
    if(isEnable == 1)
    {
      txbuf[2]=rate;
    }
    else
    {
      txbuf[2]=OFF;
    }
    txbuf[3] = (uint8_t)(~( txbuf[0]+ txbuf[1]+ txbuf[2]));
     writeBytes(txbuf,length+1);
     delay(1);
    if(!(readBytes(rxbuf,4)))
    {
        status = rxbuf[2];
         if(status == 0x01)
        {
            return SUCCESS;
        }
        else
        {
            return FAIL;
        }
    }
    else
    {
        return FAIL;
    }   
}

/**************************************************************************************
Description: read motion status when status pin external interrupt is availaible
Parameters :     
Return:      Communication status  0:motion detected  1:no motion      
Others:              
***************************************************************************************/
uint8_t BMS81M001::getShakeStatus()
{
    uint8_t length;
    uint8_t txbuf[4];
    int  rxbuf[6];
    int status;
    length = 0x02;
    txbuf[0]=length;
    txbuf[1]=CMD_READMOTIONSTATUS;
    txbuf[2] = (uint8_t)(~( txbuf[0]+ txbuf[1]));
    writeBytes(txbuf,length+1);
    delay(1);
    if(!(readBytes(rxbuf,4)))
    {
        status = rxbuf[2];
         if(status == 0x01)
        {
            return MOTION;
        }
        else
        {
            return NOMOTION;
        }
    }
    else
    {
        return NOMOTION;
    } 
}

/*************************************************************************************************
Description: get F/W Ver with iic
Parameters : ver: F/W ver 
             version information high byte+ version information low byte
             default:version number = 1
Return:      version number :high byte + low byte     
Others:              
**************************************************************************************************/
uint16_t BMS81M001::getFWVer()
{
    uint8_t length;
    int rxbuf[6]={0};
    uint8_t txbuf[4];
    uint16_t ver=0;
    length = 0x02;

    txbuf[0]=length;
    txbuf[1]=CMD_GETFWVERSION;
    txbuf[2]=(uint8_t)(~(txbuf[0]+txbuf[1]));
    writeBytes(txbuf,length+1);
    delay(2);
    if(!(readBytes(rxbuf,5)))
    {

       ver = ((uint16_t)rxbuf[2])<<8 | (uint16_t)rxbuf[3]; 
      
    }
    return ver;

}

/**********************************************************
Description: get motion setting with iic
Parameters : thr: motion threshold
             dur: motion duration
             delay: module idle delay
Return:      Communication status  0:Success 1:Fail      
Others:              
**********************************************************/
uint8_t BMS81M001::getParameterSetting(uint8_t &thr,uint8_t &dur,uint8_t &idle_delay)
{
    uint8_t length;
    uint8_t txbuf[4];
    int rxbuf[6];
    length = 0x02;

    txbuf[0]=length;
    txbuf[1]=CMD_GETTHRDUR;
    txbuf[2]=(uint8_t)(~(txbuf[0]+txbuf[1]));
    writeBytes(txbuf,length+1);
    delay(1);
    if(!(readBytes(rxbuf,6)))
    {
        thr = rxbuf[2];
        dur =rxbuf[3];
        idle_delay = rxbuf[4];
        return SUCCESS;
    }
    else
    {
        return FAIL;
    } 
}

/**********************************************************
Description: Get INTpin status
Parameters : 
Return:      INT pin status     
Others:              
**********************************************************/
uint8_t BMS81M001::getStatus()
{
  return digitalRead(_intPin);
}

/**********************************************************
Description: set motion threshold
Parameters:  thr: (1~255), sensitive: 1LSB/2mg
             dur: (1~255), sensitive: 1LSB/1ms        
Return:      Communication status  0:Success 1:Fail      
Others:              
**********************************************************/
uint8_t BMS81M001::setShake(uint8_t thr,uint8_t dur)
{
    uint8_t length;
    uint8_t status;
    uint8_t txbuf[5];
    int rxbuf[6];
    length = 0x04;
    if(thr<1)
    {
        thr = 1; 
    }
    if(dur<1)
    {
        dur = 1; 
    }

    txbuf[0]=length;
    txbuf[1]=CMD_SETMOTIONTHRESOLD;
    txbuf[2]=thr;
    txbuf[3]=dur;
    txbuf[4]=(uint8_t)(~( txbuf[0]+ txbuf[1]+ txbuf[2]+ txbuf[3]));
    writeBytes(txbuf,length+1);
    delay(1);
    if(!(readBytes(rxbuf,4)))
    {
        status = rxbuf[2];
         if(status == 0x01)
        {
            return SUCCESS;
        }
        else
        {
            return FAIL;
        }
    }
    else
    {
        return FAIL;
    } 


}



/******************************************************************************************************************
Description: set idle mode delay(modue will enter idle mode when there's no motion deteceted lasting for this time)
Parameters :       @delay: (0~255),  1LSB/s, delay=0(idle mode disable)       
Return:      Communication status  0:Success 1:Fail      
Others:              
*******************************************************************************************************************/
uint8_t BMS81M001::setIdleModeDelay(uint8_t idle_delay)
{
    uint8_t length;
    uint8_t status;
    uint8_t txbuf[4];
    int rxbuf[6];
    length = 0x03; 
    txbuf[0]=length;
    txbuf[1]=CMD_SETSLEEPDELAY;
    txbuf[2]=idle_delay;
    txbuf[3] = (uint8_t)(~( txbuf[0]+ txbuf[1]+ txbuf[2]));
    writeBytes(txbuf,length+1);
     delay(1);
    if(!(readBytes(rxbuf,4)))
    {
        status = rxbuf[2];
         if(status == 0x01)
        {
            return SUCCESS;
        }
        else
        {
            return FAIL;
        }
    }
    else
    {
        return FAIL;
    } 

}

/**********************************************************
Description: set motion sensitivity level
Parameters :  lev:（1~5）,lev = 1 is the highest sensitivity       
Return:      Communication status  0:Success 1:Fail      
Others:              
**********************************************************/
uint8_t BMS81M001::setThresholdLevel(uint8_t lev)
{
    u_int8_t status;
    status = SUCCESS;
    switch (lev)
    {
        case 1: 
                    setShake(1,1);      
        break;
        case 2: 
                    setShake(5,2);        
        break;
        case 3: 
                    setShake(10,3);            
        break;   
        case 4:
                    setShake(20,4);            
        break; 
        case 5:
                    setShake(100,1);            
        break;     
        default:
                    status = FAIL;
            break;
    }
    return status; 
}
/**********************************************************
Description: get sensor original data with iic
Parameters :       
            @data: sensor data read (12 byte)   
                    byte0: accel_xout[15:8]
                    byte1: accel_xout[7:0]
                    byte2: accel_yout[15:8]
                    byte3: accel_yout[7:0]
                    byte4: accel_zout[15:8]
                    byte5: accel_zout[7:0]
                    byte6: gyro_xout [15:8]
                    byte7: gyro_xout [7:0]
                    byte8: gyro_yout [15:8]
                    byte9: gyro_yout [7:0]
                    byte10: gyro_zout[15:8]
                    byte11: gyro_zout[7:0]
Return:            
Others:              
**********************************************************/
uint8_t BMS81M001::getSensorData(uint8_t data[])
{
    uint8_t length,i;
    uint8_t txbuf[3];
    int rxbuf[16];
    length = 0x02;
    txbuf[0]=length;
    txbuf[1]=CMD_READSENSORDATA;
    txbuf[2]=(uint8_t)(~(txbuf[0]+txbuf[1]));
    writeBytes(txbuf,length+1);
    delay(2);
    if(!(readBytes(rxbuf,15)))
    {  
        for(i=2;i<14;i++)
        {
           *(data+i-2) = rxbuf[i]; 
        }
        return SUCCESS;
    }
    else
    {
        return FAIL;
    } 
}

/**********************************************************************************************
Description: enter sleep mode 
             When  module(BMS81M001) is in sleep mode , accelerometer and gyroscope are disable, 
             the only way to wake it up is using the wakeup() function 
Parameters :                  
Return:      Communication status  0:Success 1:Fail      
Others:              
**********************************************************************************************/
  uint8_t BMS81M001::enterSleepMode(void)
 {
    uint8_t length,txbuf[3];
    int rxbuf[6];
     uint8_t status;
    length = 0x02;

    txbuf[0]=length;
    txbuf[1]=CMD_SLEEP;
    txbuf[2]=(uint8_t)(~(txbuf[0]+txbuf[1]));
    writeBytes(txbuf,length+1);

    delay(1);
    if(!(readBytes(rxbuf,4)))
    {
        status = rxbuf[2];
         if(status == 0x01)
        {
            return SUCCESS;
        }
        else
        {
            return FAIL;
        }
    }
    else
    {
        return FAIL;
    } 

 }

/**********************************************************************************************
Description: wake up the module(BMS81M001) when it is in sleep mode 
Parameters :                  
Return:      Communication status  0:Success 1:Fail      
Others:      after wakeup , it needs around 150ms to initial the sensor       
**********************************************************************************************/
uint8_t BMS81M001::wakeup(void)
{
    uint8_t length,txbuf[3];
    int rxbuf[14];
    length = 0x02;

    txbuf[0]=length;
    txbuf[1]=CMD_WAKEUP;
    txbuf[2]=(uint8_t)(~(txbuf[0]+txbuf[1]));
    writeBytes(txbuf,length+1);
    delay(1);
    readBytes(rxbuf,4);
    if(!rxbuf[2])
    {
        return FAIL;
    }
    else
    {
         return SUCCESS;
    }      

}
/*****************************************************************
Description: read byte from BMS81M001 with iic
Parameters :  
      rbuf: Data to be read.
      rlen:Length of data to be read .           
Return:      Communication status  0:Success 1:Fail      
Others:              
*****************************************************************/
uint8_t BMS81M001::readBytes(int rbuf[], int rlen)
{
    int checksum = 0;
    int i = 0;
    _wire->requestFrom(IIC_SLAVE_ADDR, rlen);  // request quantity  bytes from slave device address
    while(_wire->available())          // slave may send less than requested
    { 
        rbuf[i] = _wire->read();       // receive a byte 
        checksum += rbuf[i];
        i++;
    } 
    if ((uint8_t)rbuf[rlen-1] != (uint8_t)(~(checksum - rbuf[rlen-1])))
    {
        return FAIL;
    }
    else
    {
        return SUCCESS;
    } 
}

/*****************************************************************
Description: write byte to BMS81M001 with iic
Parameters :    
      wbuf:Data to be written.
      wlen:Length of data to be written.        
Return:      Communication status  0:Success 1:Fail      
Others:              
*****************************************************************/
void BMS81M001::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
    if (_wire != NULL)
    {
        while (_wire->available() > 0)
        {
        _wire->read();
        }
        _wire->beginTransmission(IIC_SLAVE_ADDR);
        _wire->write(wbuf, wlen);
        _wire->endTransmission();
     }
}
