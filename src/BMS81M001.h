/**********************************************************************
File:           BMS81M001.h
Author:         BESTMODULES
Description:    MPU6050 Wake on Vibration Module
History：      
  V1.0.1   -- initial version；2023-05-10；Arduino IDE : v1.8.13

***********************************************************************/

#ifndef BMS81M001_h
#define BMS81M001_h

#include "Wire.h"
#include "Arduino.h"

#define IIC_SLAVE_ADDR   (0x43)
#define BMS81M001_CLOCK  (100000)

#define SUCCESS     (0)
#define FAIL        (1)
#define MOTION      (1)
#define NOMOTION    (0)

#define OFF                (0x00)
#define WAKE_5Hz           (0x02)
#define WAKE_20Hz          (0x03)
#define WAKE_40Hz          (0x04)

typedef enum 
{
    CMD_RESETMODULE,        //reset module
    CMD_SETMOTIONTHRESOLD,  //set motion threshold
    CMD_SETSLEEPDELAY,      //set delay time to sleep when no motion detected
    CMD_READMOTIONSTATUS,   //read motion status
    CMD_GETFWVERSION,       //get F/W version
    CMD_GETTHRDUR,          //get eeprom thr and dur
    CMD_LPMODE,             //low power mode (accel mode) setting
    CMD_WAKEUP,             //wake up
    CMD_SLEEP,              //sleep
    CMD_READSENSORDATA,     //get seneor original data
}COMMAND;


class BMS81M001
{   
public:
    BMS81M001( uint8_t statusPin,TwoWire *theWire = &Wire);
    void begin(uint32_t I2C_Clock = BMS81M001_CLOCK);
    uint8_t reset();
    uint8_t setWorkMode(uint8_t rate,uint8_t isEnable = 1);
    uint8_t getShakeStatus();
    uint16_t getFWVer();
    uint8_t getParameterSetting(uint8_t &thr,uint8_t &dur,uint8_t &idle_delay); 
    uint8_t getStatus();
    uint8_t setShake(uint8_t thr,uint8_t dur);
    uint8_t setIdleModeDelay(uint8_t idle_delay);
private:
    TwoWire* _wire; 
    uint8_t _intPin;
    uint8_t readBytes(int rbuf[], int rlen);
    void writeBytes(uint8_t wbuf[], uint8_t wlen);
    uint8_t setThresholdLevel(uint8_t lev);
    uint8_t getSensorData(uint8_t data[]);
    uint8_t enterSleepMode(void);
    uint8_t wakeup(void);    
};



#endif
