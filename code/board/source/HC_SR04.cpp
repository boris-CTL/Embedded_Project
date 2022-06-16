 #include "HC_SR04.h"
#include <cstdio>
    
HC_SR04::HC_SR04(PinName trigPin, PinName echoPin, float updateSpeed, float timeout):
    _trig(trigPin), _echo(echoPin){
    _updateSpeed = updateSpeed;
    _timeout = timeout;
}

HC_SR04::HC_SR04(PinName trigPin, PinName echoPin, float updateSpeed, float timeout, int &dist):
    _trig(trigPin), _echo(echoPin), _dist(&dist){
    // _onUpdateMethod=onUpdate;
    _updateSpeed = updateSpeed;
    _timeout = timeout;
    _t.start();
}
void HC_SR04::_startT()
{ 
    if(_t.read()>600)
    {
        _t.reset();
    }
    start = _t.read_us();
}
    
void HC_SR04::_updateDist()
{
    end = _t.read_us();
    done = 1;
    _distance = (end - start)/6;       
    _tout.detach();
    _tout.attach(callback(this, &HC_SR04::_startTrig), _updateSpeed);
}
void HC_SR04::_startTrig(void)
{
        _tout.detach();
        _trig=1;             
        wait_us(10);        
        done = 0;            
        _echo.rise(callback(this,&HC_SR04::_startT));   
        _echo.fall(callback(this,&HC_SR04::_updateDist));
        _echo.enable_irq ();
        _tout.attach(callback(this,&HC_SR04::_startTrig), _timeout);
        _trig=0;                     
}

int HC_SR04::getCurrentDistance(void)
{
    return _distance;
}
void HC_SR04::pauseUpdates(void)
{
    _tout.detach();
    _echo.rise(NULL);
    _echo.fall(NULL);
}
void HC_SR04::startUpdates(void)
{
    _startTrig();
}
// void HC_SR04::attachOnUpdate(void method(int))
// {
//     _onUpdateMethod = method;
// }
void HC_SR04::changeUpdateSpeed(float updateSpeed)
{
    _updateSpeed = updateSpeed;
}
float HC_SR04::getUpdateSpeed()
{
    return _updateSpeed;
}
int HC_SR04::isUpdated(void)
{
    //printf("%d", done);
    d=done;
    done = 0;
    return d;
}
bool HC_SR04::checkDistance(void)
{
    if(isUpdated())
    {
        // (*_onUpdateMethod)(_distance);
        *_dist = _distance;
        // printf("Radar is Updated!\n");
        return true;
    }
    // printf("Radar is not Updated!\n");
    return false;
}