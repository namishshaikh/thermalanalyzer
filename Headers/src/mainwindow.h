#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadImageClicked();
    void onSaveImageClicked();

private:
    QWidget *centralWidget;
    QComboBox *sourceComboBox;
    QPushButton *loadButton;
    QPushButton *saveButton;
    QLabel *rawImageLabel;
    QLabel *processedImageLabel;
    int imageCounter = 1;
    cv::Mat lastProcessedImage;

    void loadFromCamera();
    void loadFromMockServer();
    cv::Mat processImage(const cv::Mat &inputImage);
    cv::Mat detectHotspots(const cv::Mat& inputImage, int thresholdValue = 200);
};

#endif // MAINWINDOW_H

