#ifndef CAR_CPP
#define CAR_CPP

#include "car.hpp"


Car::Car(int state, cv::Point p, cv::Rect r){

    this->state = state;
    this->actual = p;
    this->origin = this->actual;
    this->boundRect = r;
    this->age = 0;
    this->tracked = false;

}


void Car::updatePosition(cv::Point p){

   track.push_back(actual);

   p = actual;
}

bool Car::getTracked(){
    return tracked;
}

void Car::setTracked(bool s){
    tracked = s;
}

void Car::setState(int s){
    state = s;
}

int Car::getState()
{
    return state;
}

cv::Point Car::getOrigin() const
{
    return origin;
}

void Car::setOrigin(const cv::Point &value)
{
    origin = value;
}

cv::Point Car::getActual() const
{
    return actual;
}

void Car::setActual(const cv::Point &value)
{
    actual = value;
}

#endif // CAR_CPP
