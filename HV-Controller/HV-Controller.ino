#include <arduino.h>
#include <avr/io.h>

#define HVret 0
#define PwmPin0 6
#define PwmPin2 3
#define divisor 0x02 // 1/8分周

int _targetVoltage;
int _permitDev;
int _frequency;
int _duty;

void SetPwmFrequency(int frequency);
void SetPwmDuty(float duty);
void IncreaseVoltage();
void DecreaseVoltage();


void setup()
{
    pinMode(PwmPin2,OUTPUT);  
    
    //SetPwmFrequency(PwmPin2,1); //SetPwn Frequency 31.250kHz

    _targetVoltage = 175; //set Voltage at start to 175V
    _permitDev = 1; // set Voltage Deviation to 1V (Deviation +- 1V)
}

void loop()
{
    int retHV = analogRead(HVret);
    int tVoltageMin = _targetVoltage - _permitDev;
    int tVoltageMax = _targetVoltage + _permitDev;
    if(tVoltageMax <= retHV)
    {
        IncreaseVoltage();
        return;
    }
    if(tVoltageMin >= retHV)
    {
        DecreaseVoltage();
        return;
    }

}
void IncreaseVoltage()
{
    int increaseVector = 0;
    if(_duty <= 0.9)
    {
        int testBefore = analogRead(HVret);
        SetPwmDuty(_duty + 0.05);
        increaseVector = analogRead(HVret) - testBefore;
        if(increaseVector > 0)
        {
            SetPwmDuty(_duty + 0.05);
            return;
        }
        SetPwmDuty(_duty - 0.15);
        return;
    }
    
    int testBefore = analogRead(HVret);
    SetPwmFrequency(_frequency + 500);
    increaseVector = analogRead(HVret) - testBefore;
    if(increaseVector > 0)
    {
        SetPwmFrequency(_frequency + 500);
        return;
    }
    SetPwmFrequency(_frequency - 1500);
    return;
    
}

void DecreaseVoltage()
{
    int decreaseVector = 0;
    if(_duty > 0.1)
    {
        int testBefore = analogRead(HVret);
        SetPwmDuty(_duty - 0.05);
        decreaseVector = analogRead(HVret) - testBefore;
        if(decreaseVector < 0)
        {
            SetPwmDuty(_duty - 0.05);
            return;
        }
        SetPwmDuty(_duty + 0.15);
        return;
    }

    int testBefore = analogRead(HVret);
    SetPwmFrequency(_frequency - 500);
    decreaseVector = analogRead(HVret) - testBefore;
    if(decreaseVector < 0)
    {
        SetPwmDuty(_frequency - 500);
        return;
    }
    SetPwmFrequency(_frequency +1500);
    return;

}

void SetPwmFrequency(int frequency) //only Pin 3
{
    _frequency = frequency;
    TCCR2A = 0b11111111;
    TCCR2B = 0b11111000 | divisor; //1/8分周設定 => 16MHz / 8 = 2MHz
    OCR2A = (uint)(2000000/frequency); //Frequency Setting
    OCR2B = (uint)(2000000/(frequency * _duty));
}

void SetPwmDuty(float duty)
{
    _duty = duty;
    OCR2B = (2000000/(frequency * duty);
}
