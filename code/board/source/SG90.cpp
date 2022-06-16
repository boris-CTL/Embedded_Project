#include "SG90.h"
 
SG90::SG90(PinName pwmPin){
    iPreRotate=1450;
    Pin = new PwmOut(pwmPin);
    // period_ms(20);// 50Hz to trigger SG90
    Pin->period_ms(20);
    // pulsewidth_us(iPreRotate); //500:90 1450:0 2400:-90   
    Pin->pulsewidth_us(iPreRotate);
}
void SG90::SetAngle(float fAngle){
    if (fAngle>90.0f)
        fAngle=90.0f;
    else if(fAngle<-90.0f)
        fAngle=-90.0f;
    int iRotate=(-(fAngle-90.0f)*950.0f/90.0f)+500.0f;
    // pulsewidth_us(iRotate);
    Pin->pulsewidth_us(iRotate);
}