#include "utils.h"

void cvROIMouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{

     std::vector<cv::Point> *points = (std::vector<cv::Point> *)userdata;
     //cout <<"Points size: "<< points->size()<<endl;
     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          if(points->size() <= 4)
            points->push_back(cv::Point(x,y));
     }


}

void cvLineMouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{

     std::vector<cv::Point> *points = (std::vector<cv::Point> *)userdata;
     //cout <<"Points size: "<< points->size()<<endl;
     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
          if(points->size() <= 2)
            points->push_back(cv::Point(x,y));
     }


}


/**
 * Draw the crossing Lines for counting
 * @brief DrawCrossingLine
 */

void DrawCrossingLine(cv::Mat& frame, std::vector<cv::Point> pointsLine){

    cv::Size frameSize = frame.size();


    std::string s = "A";
    //insertamos el texto en el video
    cv::putText(frame, //frame o imagen
        s, //texto en formato estandar
        cv::Point(40, 110), // ubicacion del texto
        cv::FONT_HERSHEY_TRIPLEX, // fuente del texto
        4, // tamano del texto
        cv::Scalar(255, 0, 0), // color del texto
        4); // ancho del texto

    s = "B";
    cv::putText(frame, //frame o imagen
        s, //texto en formato estandar
        cv::Point(40, frameSize.height-50), // ubicacion del texto
        cv::FONT_HERSHEY_TRIPLEX, // fuente del texto
        4, // tamano del texto
        cv::Scalar(255, 0, 0), // color del texto
        4); // ancho del texto

    if(pointsLine.size() == 2){

        cv::line( frame, pointsLine[0],
                pointsLine[1],
                cv::Scalar( 0, 0, 255 ), //red
                2, 8 );
    }
}

double distance(cv::Point p1, cv::Point p2){

    return std::sqrt((p1.x-p2.x) + (p1.y - p2.y));
}
