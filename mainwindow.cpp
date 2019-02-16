#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * get video file path
 * @brief MainWindow::SelectVideo
 */
void MainWindow::SelectVideo()
{
    // Declara la variable con la ruta del archivo
    QString file = QFileDialog::getOpenFileName(this,
        tr("Abrir Video"),
        "",
        tr("Videos (*.avi *.mp4 *.mov)"));
    //Agrega la ruta del archivo
    ui->lineVideo->setText(file);
    ui->fileRadioButton->setChecked(true);
}



/**
 * Add BGS to frame
 * @brief MainWindow::BGS
 */

void MainWindow::BGS(cv::Mat inFrame, cv::Mat& outFrame){

    cv::Mat gray, mogFrame, threshFrame, blurFrame, opFrame, clFrame;
    int erosion_op = 3;
    int erosion_cl = 7;
    Mat elementOp = getStructuringElement(MORPH_RECT, Size(2 * erosion_op + 1, 2 * erosion_op + 1), Point(erosion_op,erosion_op) );
    Mat elementCl = getStructuringElement(MORPH_RECT, Size(2 * erosion_cl + 1, 2 * erosion_cl + 1), Point(erosion_cl,erosion_cl) );

    cv::cvtColor(inFrame, gray, cv::COLOR_BGR2GRAY);
    //cv::GaussianBlur(gray,gray,Size(7,7), 0);
    cv::blur(gray,gray,Size(4,4));

    pMOG2->apply(gray, mogFrame, -1);


    //cv::blur( threshFrame, blurFrame, Size( 5, 5 ) );

    //cv::morphologyEx(blurFrame, opFrame,cv::MORPH_OPEN, elementOp, cv::Point(1,1), 1);

    cv::morphologyEx(mogFrame, clFrame, cv::MORPH_CLOSE, elementCl);

    cv::threshold(clFrame, outFrame, 128.0, 255.0, cv::THRESH_BINARY);


}

/**
 * Add AbsDiffBGS to frame
 * @brief MainWindow::AbsDiffBGS
 */

void MainWindow::AbsDiffBGS(cv::Mat firstFrame, cv::Mat inFrame, cv::Mat& outFrame){

    cv::Mat gray, frameDelta, threshFrame;

    cv::cvtColor(inFrame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray,gray,Size(21,21), 0);

    cv::absdiff(firstFrame, gray, frameDelta);

    cv::threshold(frameDelta, threshFrame, 150.0, 255.0, cv::THRESH_BINARY);

    cv::dilate(threshFrame, outFrame, cv::Mat(), Point(-1, -1), 2);
}

/**
 * Process Video function with opencv
 * @brief MainWindow::ProcesarVideo
 * @param checked
 *
 */
void MainWindow::ProcessVideo(bool checked)
{

    cv::destroyAllWindows(); // Para cerrar todas las ventanas

    cv::Mat origFrame, frame, mask, dstFrame, maskFrame; // Frame como array multidimencional
    cv::Mat bgsFrame, firstFrame;
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    bool first = true;
    cv::Point prevPoint(0,0);

    int maxEmptyFrames = 5;
    int emptyFramesCount = 0;
    int carCount = 0;

    blobId = 0;

    if (!checked) { // Si !checked detiene el video si no lo procesa
        ui->startButton->setText("Start");
        cap.release();
    }
    else {
        ui->startButton->setText("Stop");

        if (ui->fileRadioButton->isChecked()) { // si el "radio button" esta seleccionado ejecuta el video si no la webcam
            cap.open(ui->lineVideo->text().toStdString().c_str());
        }
        else if(ui->webcamRadioButton->isChecked()) {
            cap.open(0);
        }
        else if(ui->rtspRadioButton->isChecked()){

            cap.open(ui->rtspUrlEdit->text().toStdString().c_str());
        }

        if(cap.isOpened()){
          frameWidht = cap.get(CV_CAP_PROP_FRAME_WIDTH) ;
          frameHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT) ;
        }
    }

    cv::namedWindow("Reproductor", cv::WINDOW_KEEPRATIO); // creamos una ventana la cual permita redimencionar
    cv::resizeWindow("Reproductor", 640,480);

    cv::namedWindow("ROI", cv::WINDOW_KEEPRATIO); // creamos una ventana la cual permita redimencionar
    cv::resizeWindow("ROI", 640,480);

    cv::namedWindow("BGS", cv::WINDOW_KEEPRATIO); // creamos una ventana la cual permita redimencionar
    cv::resizeWindow("BGS", 640,480);

    while (checked) // bucle hasta que se presione "parar video"
    {

        if(ui->fileRadioButton->isChecked()){
            break;
        }

        cap >> frame; // obtiene un nuevo frame del video o camara

        origFrame = frame;

        if (frame.empty()){
            std::cout<<"Frame empty"<<std::endl;


            emptyFramesCount++;
            if(emptyFramesCount > maxEmptyFrames)
                    //break;
                cap.open(ui->rtspUrlEdit->text().toStdString().c_str());

            continue; // detiene el bucle si elframe esta vacio

        } else {
            emptyFramesCount = 0;
        }

        if(first){
            cv::cvtColor(frame, firstFrame, cv::COLOR_BGR2GRAY);
            first = false;

            if(pointsROI.size() == 4){
                mask = cv::Mat::zeros(firstFrame.size(),firstFrame.type());
                std::vector<std::vector<cv::Point> > fillContAll;
                fillContAll.push_back(pointsROI);
                cv::fillPoly(mask, fillContAll, Scalar(255, 0, 0));
                dstFrame = cv::Mat::zeros(firstFrame.size(), firstFrame.type());
            }

            continue;
        }

        if(pointsROI.size() == 4){
            frame.copyTo(maskFrame,mask);
            BGS(maskFrame, bgsFrame);
        } else {
            BGS(frame, bgsFrame);
        }

        //std::cout << " =---Init---- " << std::endl;

        prevCars.clear();
        for(int i=0; i < cars.size(); i++){
            if(cars[i].getState() != OUT)
                prevCars.push_back(cars[i]);
        }

        //AbsDiffBGS(firstFrame,frame,bgsFrame);

        //cv::findContours( bgsFrame, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
        cv::findContours( bgsFrame, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        cars.clear();

        for( int i = 0; i< contours.size(); i++ ){

            double area = cv::contourArea(contours[i]);

            if(area < 4000)
                continue;

            cv::Rect boundingRect;
            std::vector<Point> contours_poly;

            //std::cout << " Area: " << area << std::endl;
            cv::Moments m = cv::moments(contours[i], false);
            cv::Point center = cv::Point(m.m10/m.m00, m.m01/m.m00);
            //std::cout << " Center: " << center<< std::endl;
            drawContours( frame, contours, i, Scalar(0, 255, 0) , 2, 8, hierarchy, 0, Point() );

            cv::approxPolyDP( Mat(contours[i]), contours_poly, 3, true);
            boundingRect = cv::boundingRect(contours_poly);

            Car c = Car(IN, center, boundingRect);

            cars.push_back(c);

            /*for(int i = 0; i < cars.size(); i++){

                cv::Point p = cars.at(i).getPosition();

                double dist = distance(center, p);
                std::cout << "Distance" << dist << std::endl;

                if(dist < 5){
                    isNew = false;
                    cars.at(i).updatePosition(center);
                }

                if(p.x > 490){
                    cars.erase(cars.begin() + i);
                    blobId--;
                }

            }*/


        }

        //std::cout << "cars len  "<<cars.size() << std::endl;

        for(int i=0; i < prevCars.size(); i++)
            prevCars[i].setTracked(false);


        for(int i=0; i < cars.size(); i++){
             for(int j=0; j < prevCars.size(); j++){
                 if(prevCars[j].getTracked()) continue;

                 //std::cout << "prevcars  "<<prevCars[j].getActual() << std::endl;
                 //std::cout << "cars  "<<cars[i].getActual() << std::endl;

                 double dist = distance(cars[i].getActual(),prevCars[j].getActual() );
                 //std::cout << " Distance " << dist << std::endl;

                 if(dist < 8){

                    prevCars[j].setTracked(true);

                    if(cars[i].getActual().x >= 500 && prevCars[j].getActual().x < 500){
                        carCount++;
                        std::cout << " Count: " << carCount << std::endl;
                    }

                    cars[i].setState(MOVE);
                    cars[i].setOrigin(prevCars[j].getActual());
                    //std::cout << " Origin: " << cars[i].getOrigin() << std::endl;

                 }

             }
         }

        for(int i=0; i < prevCars.size(); i++){
            if(!prevCars[i].getTracked()){
                prevCars[i].setState(OUT);
                cars.push_back(prevCars[i]);
            }
        }
        //std::cout << " =---End---- " << std::endl;


        //DrawCrossingLine(frame, pointsLine);

        if(pointsROI.size() == 4){
            cv::polylines(frame, pointsROI,true, Scalar(255, 255, 255));
            frame.copyTo(dstFrame,mask);
        }

        cv::imshow("Reproductor", origFrame); // se muestran los frames

        if(pointsROI.size() == 4)
            cv::imshow("ROI", dstFrame);

        cv::imshow("BGS", bgsFrame); // se muestran los frames

        char key = (char)cv::waitKey(2); //espera 20ms por la tecla ESC
        /*if (key == 27)
            break; //detiene el bucle*/
    }

    ui->startButton->setChecked(false);
    ui->startButton->setText("Start");
    cap.release();
    cv::destroyAllWindows();
}

/**
 * Set ROI for selected Video
 * @brief MainWindow::SetROIVideo
 * @param checked
 *
 */
void MainWindow::SetROIVideo(bool checked)
{

    cv::destroyAllWindows(); // Para cerrar todas las ventanas

    cv::Mat frame; // Frame como array multidimencional

    std::vector<cv::Point> pointsTmp;

    if (!checked) { // Si !checked detiene el video si no lo procesa
        ui->roiSetButton->setText("Set");
        pointsTmp.clear();
        cap.release();
    }
    else {
        ui->roiSetButton->setText("Stop");

        if (ui->fileRadioButton->isChecked()) { // si el "radio button" esta seleccionado ejecuta el video si no la webcam
            cap.open(ui->lineVideo->text().toStdString().c_str());
        }
        else if(ui->webcamRadioButton->isChecked()) {
            cap.open(0);
        }
        else if(ui->rtspRadioButton->isChecked()){

            cap.open(ui->rtspUrlEdit->text().toStdString().c_str());
        }

        if(cap.isOpened()){
          frameWidht = cap.get(CV_CAP_PROP_FRAME_WIDTH) ;
          frameHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT) ;
        }
    }

    cv::namedWindow("ROI", cv::WINDOW_KEEPRATIO); // creamos una ventana la cual permita redimencionar
    cv::resizeWindow("ROI", 640,480);

    while (checked) // bucle hasta que se presione "parar video"
    {
        cap >> frame; // obtiene un nuevo frame del video o camara
        if (frame.empty())
            break; // detiene el bucle si elframe esta vacio


        cv::setMouseCallback("ROI", cvROIMouseCallBackFunc, &pointsTmp);

        if(pointsTmp.size() > 0 &&  pointsTmp.size() <= 3){
            for(int i=0; i < pointsTmp.size(); i++){
                cv::circle( frame,  pointsTmp[i], 8.0, Scalar( 0, 0, 255 ), cv::FILLED, cv::LINE_8 );
            }
        }

        if(pointsTmp.size() == 4){
            cv::polylines(frame, pointsTmp,true, Scalar(255, 255, 255));
            pointsROI = pointsTmp;
        }
        cv::imshow("ROI", frame); // se muestran los frames

        char key = (char)cv::waitKey(20); //espera 20ms por la tecla ESC
        /*if (key == 27)
            break; //detiene el bucle*/
    }

    ui->roiSetButton->setChecked(false);
    ui->roiSetButton->setText("Set");
    pointsTmp.clear();
    cap.release();
    cv::destroyAllWindows();
}

/**
 * Set Line for counting
 * @brief MainWindow::SetLineVideo
 * @param checked
 *
 */
void MainWindow::SetLineVideo(bool checked)
{

    cv::destroyAllWindows(); // Para cerrar todas las ventanas

    cv::Mat frame; // Frame como array multidimencional

    std::vector<cv::Point> pointsTmp;

    if (!checked) { // Si !checked detiene el video si no lo procesa
        ui->countLineSetButton->setText("Set");
        pointsTmp.clear();
        cap.release();
    }
    else {
        ui->countLineSetButton->setText("Stop");

        if (ui->fileRadioButton->isChecked()) { // si el "radio button" esta seleccionado ejecuta el video si no la webcam
            cap.open(ui->lineVideo->text().toStdString().c_str());
        }
        else if(ui->webcamRadioButton->isChecked()) {
            cap.open(0);
        }
        else if(ui->rtspRadioButton->isChecked()){

            cap.open(ui->rtspUrlEdit->text().toStdString().c_str());
        }

        if(cap.isOpened()){
          frameWidht = cap.get(CV_CAP_PROP_FRAME_WIDTH) ;
          frameHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT) ;
        }
    }

    cv::namedWindow("Line", cv::WINDOW_KEEPRATIO); // creamos una ventana la cual permita redimencionar
    cv::resizeWindow("Line", 640,480);

    while (checked) // bucle hasta que se presione "parar video"
    {
        cap >> frame; // obtiene un nuevo frame del video o camara
        if (frame.empty())
            break; // detiene el bucle si elframe esta vacio


        cv::setMouseCallback("Line", cvLineMouseCallBackFunc, &pointsTmp);

        if(pointsTmp.size() == 1){

            cv::circle( frame,  pointsTmp[0], 12.0, Scalar( 0, 0, 255 ), cv::FILLED, cv::LINE_8 );
        }

        if(pointsTmp.size() == 2){
            cv::line( frame,
                    pointsTmp[0],
                    pointsTmp[1],
                    Scalar(0,0,255), 2, 8);
            pointsLine = pointsTmp;
        }
        cv::imshow("Line", frame); // se muestran los frames

        char key = (char)cv::waitKey(20); //espera 20ms por la tecla ESC
        /*if (key == 27)
            break; //detiene el bucle*/
    }

    ui->countLineSetButton->setChecked(false);
    ui->countLineSetButton->setText("Set");
    pointsTmp.clear();
    cap.release();
    cv::destroyAllWindows();
}



void MainWindow::on_startButton_toggled(bool checked)
{
    pMOG2 = cv::createBackgroundSubtractorMOG2(500,16,true);
    ProcessVideo(checked);
}

void MainWindow::on_videoFileToolButton_clicked()
{
    SelectVideo();
}

void MainWindow::on_roiSetButton_toggled(bool checked)
{
    SetROIVideo(checked);
}

void MainWindow::on_countLineSetButton_toggled(bool checked)
{
    SetLineVideo(checked);
}
