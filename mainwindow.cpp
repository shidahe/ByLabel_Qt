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

}
