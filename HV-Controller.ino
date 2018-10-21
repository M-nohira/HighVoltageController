#include <arduino.h>
#include <avr/io.h>

#define HVret 0
#define PwmPin0 6
#define PwmPin2 3
#define divisor 0x01 // 1/8分周

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
    pinMode(PwmPin2,OUTPUT);  
    
    //SetPwmFrequency(PwmPin2,1); //SetPwn Frequency 31.250kHz

    _targetVoltage = 690; //set Voltage at start to 175V
    _permitDev = 10; // set Voltage Deviation to 1V (Deviation +- 1V)

    SetPwmFrequency();
    //SetPwmDuty(0.3);
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
    for(int cnt = 0; cnt <= 9;cnt ++)
    {
        int a = analogRead(A0);
        voltage = voltage + a;
        if(a < 0) Serial.println("000000000000000000000000");
        
    }
    //Serial.println(analogRead(A0));
    return voltage/10;
}
void SetPwmFrequency() //only Pin 3
{
    //_frequency = frequency;
    TCCR2A = 0b11101111;
    TCCR2B = (TCCR2B & 0b11111000) | divisor; //1/8分周設定 => 16MHz / 8 = 2MHz
    //OCR2A = (unsigned int)(2000000/frequency); //Frequency Setting
    //OCR2B = (unsigned int)(128/*2000000/frequency * _duty*/);
}

void SetPwmDuty(float duty)
{
    _duty = duty;
    OCR2B = (unsigned int)(duty);
}