#ifndef CAR_HPP
#define CAR_HPP

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc.hpp>

using namespace std;

enum {NEW, IN, MOVE, OUT};

class Car{

public:
    Car(int state, cv::Point p, cv::Rect r);
    void updatePosition(cv::Point p);

    bool getTracked();
    void setTracked(bool s);
    void setState(int s);
    int getState();

    cv::Point getOrigin() const;
    void setOrigin(const cv::Point &value);

    cv::Point getActual() const;
    void setActual(const cv::Point &value);

private:
    vector<cv::Point> track;
    cv::Point origin, actual;
    cv::Rect boundRect;
    int id;
    int age;
    int done;
    int state;
    bool tracked;
};

#endif // CAR_HPP
