#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QNetworkReply>
#include <QScrollArea>

#include <QtConcurrent>
#include <QDomDocument>

#define SERVER_IP "192.168.10.121"
#define SERVER_PORT "8080"

#define START_STREAM_PATH         "/start_stream"
#define PAUSE_STREAM_PATH         "/pause_stream"
#define RESUME_STREAM_PATH        "/resume_stream"
#define REWIND_STREAM_PATH         "/rewind_stream"
#define PREPROCESS_DETECTION_PATH "/preprocess_detection"

#include "datastorage.h"

static int secondsCounter = 0; // 초 단위 카운터 초기화

int flags=0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , videoThread(new VideoThread(this))
    , networkManager(new NetworkManager(this))
    , networkAccessManager(new QNetworkAccessManager(this))
    , chartWidget(new ChartWidget(this))
    , dataStorage(new DataStorage(this)) // DataStorage 동적 초기화
{
    ui->setupUi(this);
    
    // 데이터베이스 초기화
    QString dbPath = QCoreApplication::applicationDirPath() + "/data/datastorage.db"; // 상대 경로
    QDir dbDir(QCoreApplication::applicationDirPath() + "/data");

    // 경로가 존재하지 않으면 생성
    if (!dbDir.exists()) {
        if (!dbDir.mkpath(".")) {
            qWarning() << "Failed to create database directory at" << dbDir.path();
        }
    }

    if (!dataStorage->initDatabase(dbPath)) {
        qWarning() << "Failed to initialize database!";
    }
    else {
        qDebug() << "Database initialized at" << dbPath;   
    }

    ui->videoLabel->setFixedSize(800, 600);

    connect(videoThread, &VideoThread::frameCaptured, this, &MainWindow::updateFrame);

    videoThread->start();

    stackedWidget = ui->stackedWidget;

    imageScrollArea = new QScrollArea();
    stackedWidget->addWidget(chartWidget);
    stackedWidget->addWidget(imageScrollArea);

    connect(ui->graph_button, &QPushButton::clicked, this, &MainWindow::showChartPage);
    connect(ui->bestshot_button, &QPushButton::clicked, this, &MainWindow::showImagePage);
    connect(ui->video_start_button, &QPushButton::clicked, this, &MainWindow::sendResumeRequest);
    connect(ui->video_stop_button, &QPushButton::clicked, this, &MainWindow::sendStopRequest);
    connect(ui->video_resume_button, &QPushButton::clicked, this, &MainWindow::sendRewindRequest);
    connect(ui->exit_button, &QPushButton::clicked, this, &MainWindow::onExitButtonClicked);
    connect(ui->func_start_button, &QPushButton::clicked, this, &MainWindow::onFunctionStartButtonClicked);
    connect(ui->func_stop_button, &QPushButton::clicked, this, &MainWindow::onFunctionStopButtonClicked);
    
    connect(ui->best_resume_button, &QPushButton::clicked, this, &MainWindow::onTestButtonClicked);
    
    
    connect(this, &MainWindow::imageLoaded, this, &MainWindow::addImageToScrollArea);
    connect(this, &MainWindow::updateChartData, chartWidget, &ChartWidget::updateChart);
}

MainWindow::~MainWindow()
{
    if (videoThread && videoThread->isRunning()) {
        videoThread->requestInterruption();
        videoThread->quit();
        videoThread->wait();
    }
    delete videoThread;
    delete dataStorage;
    delete ui;
}

void MainWindow::updateFrame(const QImage &frame)
{
    QImage resizedFrame = frame.scaled(800, 600, Qt::KeepAspectRatio);
    ui->videoLabel->setPixmap(QPixmap::fromImage(resizedFrame));
}

QString MainWindow::saveImageToFile(const QByteArray &imageData) {
    if (!imageData.startsWith("\xFF\xD8") || !imageData.endsWith("\xFF\xD9")) {
        qWarning() << "Invalid JPEG data received.";
        return QString();
    }

    QString savePath = QCoreApplication::applicationDirPath() + "/bestshot_images";
    QDir dir(savePath);

    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Failed to create directory:" << savePath;
        return QString();
    }

    static int imageIndex = 0;
    QString fileName = QString("image_%1.jpg").arg(imageIndex++, 3, 10, QChar('0'));
    QString filePath = dir.filePath(fileName);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(imageData);
        file.close();
        qDebug() << "Image saved to:" << filePath;
        return filePath;
    } else {
        qWarning() << "Failed to save image:" << filePath;
        return QString();
    }
}


void MainWindow::updateImageScrollArea()
{
    // 이미지 디렉토리 경로 설정
    QString directoryPath = QCoreApplication::applicationDirPath() + "/bestshot_images";
    QDir directory(directoryPath);
    QStringList imageFiles = directory.entryList(QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files);

    // 스크롤 영역 위젯 확인 (초기화되지 않았으면 새로 생성)
    QWidget *containerWidget = imageScrollArea->widget();
    QVBoxLayout *mainLayout = nullptr;

    if (!containerWidget) {
        containerWidget = new QWidget();
        mainLayout = new QVBoxLayout(containerWidget);
        containerWidget->setLayout(mainLayout);
        imageScrollArea->setWidget(containerWidget);
    } else {
        mainLayout = qobject_cast<QVBoxLayout *>(containerWidget->layout());
    }

    // 기존 레이아웃 초기화
    QLayoutItem *child;
    while ((child = mainLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // 이미지 파일을 읽어 레이아웃에 추가
    for (const QString &fileName : imageFiles) {
        QString imagePath = directoryPath + "/" + fileName;
        QString classInfo = "Unknown";
        QString ocr = "Unknown";
        addImageToScrollArea(imagePath, classInfo, ocr); // 기존 addImageToScrollArea 함수 활용
    }

    // 스크롤 영역 업데이트
    imageScrollArea->setWidgetResizable(true);

    // 로그 출력
    qDebug() << "ImageScrollArea updated with" << imageFiles.size() << "images.";
}

void MainWindow::addImageToScrollArea(const QString &imagePath, const QString &classInfo, const QString &ocr)
{
    // 스크롤 영역의 컨테이너 위젯 확인 및 레이아웃 초기화
    QWidget *containerWidget = imageScrollArea->widget();
    QVBoxLayout *mainLayout = nullptr;

    if (!containerWidget) {
        containerWidget = new QWidget();
        mainLayout = new QVBoxLayout(containerWidget);
        containerWidget->setLayout(mainLayout);
        imageScrollArea->setWidget(containerWidget);
    } else {
        mainLayout = qobject_cast<QVBoxLayout *>(containerWidget->layout());
    }

    // 개별 항목 위젯 생성
    QWidget *itemWidget = new QWidget();
    QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);

    // 왼쪽: 이미지
    QLabel *imageLabel = new QLabel();
    QPixmap pixmap(imagePath);
    imageLabel->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setFixedSize(150, 150);

    // 오른쪽: 텍스트 정보
    QVBoxLayout *textLayout = new QVBoxLayout();
    QLabel *licenseLabel = new QLabel("Image Name: " + QFileInfo(imagePath).baseName());
    licenseLabel->setStyleSheet("font-size: 14px; color: white;");
    textLayout->addWidget(licenseLabel);
    
    QLabel *classLabel = new QLabel("Class: " + classInfo);
    classLabel->setStyleSheet("font-size: 14px; color: white;");
    textLayout->addWidget(classLabel);
    
    QLabel *ocrLabel = new QLabel("OCR: " + ocr); // OCR 추가
    ocrLabel->setStyleSheet("font-size: 14px; color: white;");
    textLayout->addWidget(ocrLabel);
    
    textLayout->addStretch();

    // 레이아웃에 이미지와 텍스트 추가
    itemLayout->addWidget(imageLabel);
    itemLayout->addLayout(textLayout);

    // 스타일 설정
    itemWidget->setStyleSheet("border: 1px solid white; background-color: #333; margin: 5px; border-radius: 8px;");
    itemWidget->setFixedHeight(160);

    // 새 항목을 레이아웃 상단에 추가
    mainLayout->insertWidget(0, itemWidget);
}



void MainWindow::onFunctionStartButtonClicked()
{
    qDebug() << "Starting persistent stream...";

    QUrl url(QString("http://%1:%2%3").arg(SERVER_IP).arg(SERVER_PORT).arg(PREPROCESS_DETECTION_PATH));
    QNetworkRequest request(url);
    request.setRawHeader("Connection", "keep-alive"); // 연결을 유지하도록 설정
    QNetworkReply *reply = networkAccessManager->get(request);
}


void MainWindow::sendNetworkRequest(const QUrl &url)
{
    QNetworkRequest request(url);
    networkManager->sendGetRequest(url);
}

void MainWindow::onDetectionStartButtonClicked()
{
    currentMode = Detection;
    qDebug() << "Mode switched to Detection.";
}

void MainWindow::onFunctionStopButtonClicked()
{
    if (currentMode == Bestshot) {
        sendNetworkRequest(QUrl(QString("http://%1:%2%3").arg(SERVER_IP).arg(SERVER_PORT).arg(PREPROCESS_DETECTION_PATH)));
    } else if (currentMode == Detection) {
        sendNetworkRequest(QUrl(QString("http://%1:%2%3").arg(SERVER_IP).arg(SERVER_PORT).arg(PREPROCESS_DETECTION_PATH)));
    }
}

void MainWindow::onExitButtonClicked()
{
    // 비디오 쓰레드가 실행 중인지 확인하고 종료 요청
    if (videoThread && videoThread->isRunning()) {
        qDebug() << "Stopping video thread...";
        videoThread->requestInterruption(); // 쓰레드에 인터럽트 요청
        videoThread->quit();                // 쓰레드를 종료 상태로 전환
        videoThread->wait();                // 쓰레드가 완전히 종료될 때까지 대기
        qDebug() << "Video thread stopped.";
    }

    // 네트워크 작업이 있는 경우 정리
    if (networkAccessManager) {
        networkAccessManager->deleteLater(); // 네트워크 매니저 정리
    }

    // 프로그램 종료
    qDebug() << "Exiting application...";
    QApplication::quit(); // 애플리케이션 종료
}

void MainWindow::showImagePage()
{
    currentMode = Bestshot;
    qDebug() << "Mode switched to Bestshot.";

    updateImageScrollArea(); // 이미지를 로드하고 UI를 업데이트합니다.

    stackedWidget->setCurrentWidget(imageScrollArea); // 스크롤 영역을 현재 위젯으로 설정
}

void MainWindow::showChartPage()
{
    // 차트 페이지를 활성화
    stackedWidget->setCurrentWidget(chartWidget);
    qDebug() << "Chart page displayed.";
}

void MainWindow::sendStartRequest() {
    QUrl streamUrl(QString("http://%1:%2%3").arg(SERVER_IP).arg(SERVER_PORT).arg(START_STREAM_PATH));
    QNetworkRequest request(streamUrl);
    QNetworkReply *reply = networkAccessManager->get(request);

    static QByteArray buffer;
    static QString currentClassInfo = "Unknown"; // 현재 클래스 정보를 저장
    static QString currentNode = "Unknown";     // 현재 노드 정보를 저장
    static QString currentCosts = "Unknown";    // 현재 비용 정보를 저장
    static QString currentOcr = "Unknown";

    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
        buffer.append(reply->readAll());

        const int maxBufferSize = 10 * 1024 * 1024; // 10MB 제한
        if (buffer.size() > maxBufferSize) {
            qWarning() << "Buffer exceeded limit. Clearing old data.";
            buffer = buffer.right(maxBufferSize / 2);
        }

        while (true) {
            int boundaryIndex = buffer.indexOf("--boundary");
            if (boundaryIndex == -1) break;

            int nextBoundaryIndex = buffer.indexOf("--boundary", boundaryIndex + 10);
            if (nextBoundaryIndex == -1) break;

            QByteArray part = buffer.mid(boundaryIndex + 10, nextBoundaryIndex - boundaryIndex - 10);
            buffer.remove(0, nextBoundaryIndex);

            int headerEnd = part.indexOf("\r\n\r\n");
            if (headerEnd != -1) {
                QByteArray headers = part.left(headerEnd);
                QByteArray body = part.mid(headerEnd + 4);

                if (headers.contains("Content-Type: application/xml")) {
                    QDomDocument doc;
                    if (doc.setContent(body)) {
                        QDomElement root = doc.documentElement();
                        QString description = root.firstChildElement("description").text();
                        QString timestamp = root.firstChildElement("timestamp").text();
                        currentClassInfo = root.firstChildElement("class").text();
                        currentCosts = root.firstChildElement("costs").text();
                        currentOcr = root.firstChildElement("ocr").text();

                        if (currentClassInfo.isEmpty()) {
                            currentClassInfo = "Unknown"; // 기본 클래스 이름
                        }
                        
                        if (currentOcr.isEmpty()) {
                            currentOcr = "Unknown"; // OCR 기본값 설정
                          }

                        qDebug() << "Parsed XML:" << description << timestamp << currentClassInfo << currentCosts << currentOcr;

                        processParsedXML(description, timestamp, currentClassInfo, currentCosts, currentOcr);
                    } else {
                        qWarning() << "Failed to parse XML content.";
                    }
                } else if (headers.contains("Content-Type: image/jpeg")) {
                    int startIdx = body.indexOf("\xFF\xD8");
                    int endIdx = body.indexOf("\xFF\xD9");
                    if (startIdx != -1 && endIdx != -1) {
                        QByteArray imageData = body.mid(startIdx, endIdx - startIdx + 2);
                        QString savedPath = saveImageToFile(imageData);

                        // 이미지와 클래스 정보 연결
                        QString classInfo = currentClassInfo;
                        QString ocr = currentOcr;

                        if (!savedPath.isEmpty()) {
                            QtConcurrent::run([=]() {
                                emit imageLoaded(savedPath, classInfo, ocr);
                            });
                        }
                    }
                }
            }
        }
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Stream error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}




void MainWindow::processParsedXML(const QString &description, const QString &timestamp, 
                                  const QString &classInfo, QString &costs, const QString &ocr) {
    // 파싱된 데이터를 처리하는 함수
    qDebug() << "Processing parsed XML data:";
    qDebug() << "Description:" << description;
    qDebug() << "Timestamp:" << timestamp;
    qDebug() << "Class:" << classInfo;
    qDebug() << "Costs:" << costs;
    qDebug() << "OCR:" << ocr;
    
    savedClassInfo = classInfo; // 클래스 정보를 저장
    savedOCR = ocr;             // OCR 정보를 저장
    
    int out_costs = costs.toInt();
    
    // ** 수정사항 **  DB에 들어갈 RISK 기준 범위
    QString riskLevel;
    if (out_costs < 8) {
        riskLevel = "SAFE";
    } else if (out_costs < 20 && out_costs > 7) {
        riskLevel = "NORMAL";
    } else if (out_costs < 100 && out_costs > 19) {
        riskLevel = "WARNING";
    } else {
        riskLevel = "DANGER";
    }

    // 타임스탬프 포맷
    QString currentTimestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    // 이미지 저장
    QByteArray imageData; // 실제 JPEG 데이터를 받아야 합니다
    QString fullPath = saveImageToFile(imageData); // 이미지 전체 경로
    QString imageName;

    if (!fullPath.isEmpty()) {
        QFileInfo fileInfo(fullPath);
        imageName = fileInfo.fileName(); // 파일 이름만 추출
    } else {
        imageName = "unknown_image.jpg";
    }

    // 데이터 저장
    if (!dataStorage->saveRecord(currentTimestamp, imageName, out_costs, riskLevel)) {
        qWarning() << "Failed to save record to database.";
    } else {
        qDebug() << "Record saved successfully.";
    }
    
    emit updateChartData(timestamp, costs);
}

void MainWindow::sendResumeRequest()
{
    sendNetworkRequest(QUrl(QString("http://%1:%2%3").arg(SERVER_IP).arg(SERVER_PORT).arg(RESUME_STREAM_PATH)));
}

void MainWindow::sendStopRequest()
{
    sendNetworkRequest(QUrl(QString("http://%1:%2%3").arg(SERVER_IP).arg(SERVER_PORT).arg(PAUSE_STREAM_PATH)));
}

void MainWindow::sendRewindRequest()
{
    sendNetworkRequest(QUrl(QString("http://%1:%2%3").arg(SERVER_IP).arg(SERVER_PORT).arg(REWIND_STREAM_PATH)));
}

void MainWindow::onTestButtonClicked()
{
    QString timestamp = "2024-12-11T14:38:27";
    
    if(flags == 0){
        QString costs = "70";
        emit updateChartData(timestamp, costs);
    }
    if(flags == 1){
        QString costs = "4";
        emit updateChartData(timestamp, costs);
    }
    if(flags == 2){
        QString costs = "16";
        emit updateChartData(timestamp, costs);
    }
    if(flags == 3){
        QString costs = "83";
        emit updateChartData(timestamp, costs);
    }
    if(flags == 4){
        QString costs = "100";
        emit updateChartData(timestamp, costs);
    }
    
    if(flags >= 5){
        flags=0;
    }
    flags++;
}

void MainWindow::onRequestFinished()
{
    // sender()를 사용하여 QNetworkReply 가져오기
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        qWarning() << "Reply is null";
        return;
    }

    QByteArray response = reply->readAll();
    qDebug() << "Request finished with response:" << response.left(100); // 응답 로그 확인
    reply->deleteLater(); // 응답 객체 해제
}

void MainWindow::onErrorOccurred(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        qWarning() << "Reply is null on error";
        return;
    }

    qWarning() << "Network Error occurred:" << error;
    reply->deleteLater(); // 응답 객체 해제
}