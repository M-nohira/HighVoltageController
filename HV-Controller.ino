#include <arduino.h>
#include <avr/io.h>

#define HVret 0
#define PwmPin0 6
#define PwmPin2 3
#define divisor 0x01 // 1/1

int _targetVoltage;
int _permitDev;
int _frequency;
int _duty;

void SetPwmFrequency();
void SetPwmDuty(float duty);
void IncreaseVoltage();
void DecreaseVoltage();
int GetVoltage();

void setup()
{
    pinMode(PwmPin2,OUTPUT);  //Set D3 OUTPUT MODE
    
    _targetVoltage = 690;   //Initialize Setting (178V +- 1V)
    _permitDev = 10; 

    SetPwmFrequency();//Setting to 62KHz

    Serial.begin(9600);
}

void loop()
{
    if(Serial.available()) _targetVoltage = Serial.readString().toInt();

    int retHV = GetVoltage();

    int tVoltageMin = _targetVoltage - _permitDev;
    int tVoltageMax = _targetVoltage + _permitDev;
    Serial.println(GetVoltage());
    if(retHV < 0)
    {
        DecreaseVoltage();
        Serial.println("Decrease");
        return;
    }

    if(tVoltageMax <= retHV)
    {
        DecreaseVoltage();
        Serial.println("Decrease");
        return;
    }
    if(tVoltageMin >= retHV)
    {
        
        IncreaseVoltage();
        Serial.println("Increase");
        
        return;
    }

}
void IncreaseVoltage()
{
    int increaseVector = 0;
    if(_duty <= 255)
    {
        int testBefore = GetVoltage();
        SetPwmDuty(_duty + 1);
        increaseVector = GetVoltage() - testBefore;
        if(increaseVector > 0)
        {
            SetPwmDuty(_duty + 1);
            return;
        }
        Serial.println("IncreaseFailed");
        //SetPwmDuty(_duty - 1);
        return;
    }
    
}

void DecreaseVoltage()
{
    int decreaseVector = 0;
    if(_duty > 0)
    {
        int testBefore = GetVoltage();
        SetPwmDuty(_duty - 1);
        decreaseVector = GetVoltage() - testBefore;
        if(decreaseVector < 0)
        {
            SetPwmDuty(_duty - 1);
            return;
        }
        Serial.println("DecreaseFaild");
        //SetPwmDuty(_duty + 2);
        return;
    }

}

int GetVoltage()
{
    
    int voltage = 0;
    int times = 5;
    for(int cnt = 0; cnt < times;cnt++)
    {
        voltage = voltage + analogRead(A0);
        if(a < 0) Serial.println("000000000000000000000000");
        
    }
    return voltage/times;
}
void SetPwmFrequency() //only Pin 3
{
    TCCR2A = 0b11101111;
    TCCR2B = (TCCR2B & 0b11111000) | divisor;
    return;
}

void SetPwmDuty(float duty)
{
    _duty = duty;
    OCR2B = (unsigned int)(duty);
    return;
}