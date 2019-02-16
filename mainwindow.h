#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <string>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include "utils.h"
#include "car.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_startButton_toggled(bool checked);

    void on_videoFileToolButton_clicked();

    void SelectVideo();

    void ProcessVideo(bool checked);

   // void DrawLines(cv::Mat& frame);

    void BGS(cv::Mat inFrame, cv::Mat& frame);

    void AbsDiffBGS(cv::Mat firstFrame, cv::Mat inFrame, cv::Mat& outFrame);

    void on_roiSetButton_toggled(bool checked);

    void SetROIVideo(bool checked);

    void SetLineVideo(bool checked);

    void on_countLineSetButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    cv::VideoCapture cap;
    double frameHeight, frameWidht;
    cv::Ptr<cv::BackgroundSubtractor> pMOG2;
    std::vector<cv::Point> pointsROI;
    std::vector<cv::Point> pointsLine;
    int blobId;
    std::vector<Car> prevCars, cars;

};

#endif // MAINWINDOW_H
