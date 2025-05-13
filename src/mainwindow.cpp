#include "mainwindow.h"
#include <QPixmap>
#include <QFileDialog>
#include <QImageReader>
#include <QImage>
#include <QtDebug>
#include <QMessageBox>
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    sourceComboBox = new QComboBox(this);
    sourceComboBox->addItem("Camera");
    sourceComboBox->addItem("Mock Server");
    sourceComboBox->addItem("File");

    loadButton = new QPushButton("Load Image", this);
    saveButton = new QPushButton("Save Image", this);

    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadImageClicked);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveImageClicked);

    rawImageLabel = new QLabel("Raw Image", this);
    rawImageLabel->setFixedSize(300, 300);
    rawImageLabel->setStyleSheet("QLabel { background-color : lightgray; }");

    processedImageLabel = new QLabel("Processed Image", this);
    processedImageLabel->setFixedSize(300, 300);
    processedImageLabel->setStyleSheet("QLabel { background-color : lightgray; }");

    QHBoxLayout *imageLayout = new QHBoxLayout;
    imageLayout->addWidget(rawImageLabel);
    imageLayout->addWidget(processedImageLabel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(sourceComboBox);
    mainLayout->addWidget(loadButton);
    mainLayout->addLayout(imageLayout);
    mainLayout->addWidget(saveButton);

    centralWidget->setLayout(mainLayout);
}

MainWindow::~MainWindow() {}

void MainWindow::loadFromCamera()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        cv::Mat image = cv::imread(fileName.toStdString());
        lastProcessedImage = detectHotspots(image);

        QImage raw((const uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
        QImage proc((const uchar*)lastProcessedImage.data, lastProcessedImage.cols, lastProcessedImage.rows, lastProcessedImage.step, QImage::Format_RGB888);

        rawImageLabel->setPixmap(QPixmap::fromImage(raw.rgbSwapped()).scaled(rawImageLabel->size(), Qt::KeepAspectRatio));
        processedImageLabel->setPixmap(QPixmap::fromImage(proc.rgbSwapped()).scaled(processedImageLabel->size(), Qt::KeepAspectRatio));
    }
}
/*
cv::Mat MainWindow::detectHotspots(const cv::Mat& inputImage, int thresholdValue)
{
    cv::Mat gray, thresh, output;
    cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thresh, thresholdValue, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    output = inputImage.clone();
    for (const auto& contour : contours) {
        cv::Rect bbox = cv::boundingRect(contour);
        cv::rectangle(output, bbox, cv::Scalar(0, 0, 255), 2);
    }

    return output;
}*/
/*
cv::Mat MainWindow::detectHotspots(const cv::Mat& inputImage, int thresholdValue)
{
    cv::Mat gray, thresh, output;
    cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thresh, thresholdValue, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    output = inputImage.clone();
    double globalMaxTemp = 0.0;
    cv::Point labelPosition;

    for (const auto& contour : contours) {
        cv::Rect bbox = cv::boundingRect(contour);
        cv::rectangle(output, bbox, cv::Scalar(0, 0, 255), 2);

        // Find maximum pixel intensity in this contour's bounding box
        cv::Mat roi = gray(bbox);
        double minVal, maxVal;
        cv::minMaxLoc(roi, &minVal, &maxVal);

        // Map intensity to temperature (20°C to 100°C)
        double temperature = 20.0 + (maxVal / 255.0) * 80.0;

        if (temperature > globalMaxTemp) {
            globalMaxTemp = temperature;
            labelPosition = bbox.tl(); // top-left corner of the max temp box
        }
    }

    // Show max temperature
    if (globalMaxTemp > 0.0) {
        std::string label = cv::format("Max Temp: %.1f°C", globalMaxTemp);
        cv::putText(output, label, labelPosition, cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(255, 255, 255), 2);
    }

    return output;
}
*/

/*
cv::Mat MainWindow::detectHotspots(const cv::Mat& inputImage, int thresholdValue)
{
    cv::Mat gray, thresh, output;
    cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thresh, thresholdValue, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    output = inputImage.clone();
    for (const auto& contour : contours) {
        cv::Rect bbox = cv::boundingRect(contour);
        cv::rectangle(output, bbox, cv::Scalar(0, 0, 255), 2);

        // Find maximum intensity inside the contour region
        cv::Mat roi = gray(bbox);
        double minVal, maxVal;
        cv::minMaxLoc(roi, &minVal, &maxVal);

        // Simulate temperature mapping (20°C to 100°C)
        double temperature = 100 + (maxVal / 255.0) * 80.0;

        // Display temperature on the image
        std::string label = cv::format("%.1f°C", temperature);
        cv::putText(output, label, cv::Point(bbox.x, bbox.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    cv::Scalar(255, 255, 255), 1);
    }

    return output;
}
*/


cv::Mat MainWindow::detectHotspots(const cv::Mat& inputImage, int thresholdValue)
{
    cv::Mat gray, thresh, output;
    cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

    // Apply higher threshold to ignore minor temperature changes
    cv::threshold(gray, thresh, thresholdValue, 255, cv::THRESH_BINARY);

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Prepare for drawing
    output = inputImage.clone();
    double globalMaxTemp = 0.0;
    QPoint labelPosition;

    for (const auto& contour : contours) {
        if (cv::contourArea(contour) < 500) continue; // Ignore small areas

        cv::Rect bbox = cv::boundingRect(contour);
        cv::Mat roi = gray(bbox);

        double minVal, maxVal;
        cv::minMaxLoc(roi, &minVal, &maxVal);

        // Convert intensity to temperature: assuming 20°C to 100°C range
        double temperature = 20.0 + (maxVal / 255.0) * 80.0;

        if (temperature > 60.0 && temperature > globalMaxTemp) {
            globalMaxTemp = temperature;
            labelPosition = QPoint(bbox.x, bbox.y);
        }

        if (temperature > 60.0) {
            // Only draw red box if temperature is above 60
            cv::rectangle(output, bbox, cv::Scalar(0, 0, 255), 2);
        }
    }

    // Draw max temperature on top-left or hotspot location
    if (globalMaxTemp > 60.0) {
        QString tempText = QString("Max Temp: %1°C").arg(globalMaxTemp, 0, 'f', 1);
        cv::putText(output, tempText.toStdString(), cv::Point(labelPosition.x(), labelPosition.y() - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
    }

    lastProcessedImage = output.clone(); // Save the processed image
    return output;
}

void MainWindow::onLoadImageClicked()
{
    QString source = sourceComboBox->currentText();

    if (source == "Camera") {
        loadFromCamera();
    } else if (source == "Mock Server") {
        loadFromMockServer();
    } else if (source == "File") {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
        if (!fileName.isEmpty()) {
            cv::Mat image = cv::imread(fileName.toStdString());
            lastProcessedImage = detectHotspots(image);

            QImage raw((const uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
            QImage proc((const uchar*)lastProcessedImage.data, lastProcessedImage.cols, lastProcessedImage.rows, lastProcessedImage.step, QImage::Format_RGB888);

            rawImageLabel->setPixmap(QPixmap::fromImage(raw.rgbSwapped()).scaled(rawImageLabel->size(), Qt::KeepAspectRatio));
            processedImageLabel->setPixmap(QPixmap::fromImage(proc.rgbSwapped()).scaled(processedImageLabel->size(), Qt::KeepAspectRatio));
        }
    }
}

void MainWindow::onSaveImageClicked()
{
    if (lastProcessedImage.empty()) {
        QMessageBox::warning(this, "No Image", "Please load and process an image first.");
        return;
    }

    QString defaultName = QString("Untitled%1.jpg").arg(imageCounter++);
    QString savePath = QFileDialog::getSaveFileName(this, "Save Processed Image", defaultName, "Images (*.png *.jpg *.bmp)");

    if (!savePath.isEmpty()) {
        cv::imwrite(savePath.toStdString(), lastProcessedImage);
        QMessageBox::information(this, "Saved", "Image saved to: " + savePath);
    }
}

void MainWindow::loadFromMockServer()
{
    QTcpSocket socket;
    socket.connectToHost("127.0.0.1", 8080);

    if (!socket.waitForConnected(3000)) {
        qDebug() << "Connection failed!";
        return;
    }

    int imgSize = 0;
    if (!socket.waitForReadyRead(3000)) {
        qDebug() << "No data received for image size!";
        return;
    }
    socket.read(reinterpret_cast<char*>(&imgSize), sizeof(imgSize));
    qDebug() << "Image size:" << imgSize;

    QByteArray imageData;
    while (imageData.size() < imgSize) {
        if (!socket.waitForReadyRead(3000)) {
            qDebug() << "Timeout while waiting for image data!";
            break;
        }
        imageData.append(socket.read(imgSize - imageData.size()));
    }

    qDebug() << "Total bytes received:" << imageData.size();

    QImage image;
    if (!image.loadFromData(imageData)) {
        qDebug() << "Failed to load image from data!";
        return;
    }

    cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
    cv::Mat matBGR;
    cv::cvtColor(mat, matBGR, cv::COLOR_RGBA2BGR);

    lastProcessedImage = detectHotspots(matBGR);

    QPixmap rawPixmap = QPixmap::fromImage(image);
    QImage processedQImage((const uchar*)lastProcessedImage.data, lastProcessedImage.cols, lastProcessedImage.rows, lastProcessedImage.step, QImage::Format_RGB888);

    rawImageLabel->setPixmap(rawPixmap.scaled(rawImageLabel->size(), Qt::KeepAspectRatio));
    processedImageLabel->setPixmap(QPixmap::fromImage(processedQImage.rgbSwapped()).scaled(processedImageLabel->size(), Qt::KeepAspectRatio));
}
