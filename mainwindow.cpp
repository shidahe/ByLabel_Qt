#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //resize(800,600);

    //this->image = new QImage();
}

MainWindow::~MainWindow()
{
    //delete image;
    delete ui;
}

void MainWindow::on_actionOpen_Single_Image_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(
                    this, "open image file",
                    ".",
                    "Image files (*.bmp *.jpg *.tif *.tiff *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)");
        if(fileName != "")
        {
            std::string fn = fileName.toStdString();
            cv::Mat cvImg = cv::imread(fn);

            ui->myGraphicsView->showImage(cvImg);
        }

//            if(!cvImg.empty()){
//                if(3 == cvImg.channels()){
//                    cv::cvtColor(cvImg, cvImg, CV_RGB2BGR);
//                }else if(1 == cvImg.channels()){
//                    cv::cvtColor(cvImg, cvImg, CV_GRAY2BGR);
//                }else{
//                    QMessageBox::information(this,tr("Warning"),tr("This is not a single or three channels image!"));
//                }

//                QImage imgTemp = QImage((uchar*) cvImg.data, cvImg.cols, cvImg.rows, cvImg.step, QImage::Format_RGB888);
//                image = &imgTemp;

//                QGraphicsScene *scene = new QGraphicsScene;
//                scene->addPixmap(QPixmap::fromImage(*image));
//                ui->myGraphicsView->setScene(scene);
//                //ui->graphicsView->resize(image->width(), image->height());
//                ui->myGraphicsView->show();

//            }else{
//                QMessageBox::information(this,tr("Error"),tr("Open Image Failed, please check the image format!"));
//            }
//        }
}
