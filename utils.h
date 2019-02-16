#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

void cvROIMouseCallBackFunc(int event, int x, int y, int flags, void* userdata);
void cvLineMouseCallBackFunc(int event, int x, int y, int flags, void* userdata);
void DrawCrossingLine(cv::Mat& frame);
double distance(cv::Point p1, cv::Point p2);

#endif // UTILS_H
