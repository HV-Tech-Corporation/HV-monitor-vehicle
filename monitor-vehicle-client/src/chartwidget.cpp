#include "chartwidget.h"
#include "customdial.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent),
      dial(new CustomDial(this)), // CustomDial을 사용
      chart(new QChart()),
      series(new QLineSeries()),
      chartView(new QChartView(chart, this)),
      mainLayout(new QVBoxLayout(this)),
      startTime(QTime::currentTime()) // 프로그램 시작 시간을 기록
{
    // 다이얼과 박스 레이아웃
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(15); // 간격 조정

    // 다이얼 초기화
    dial->setMinimumSize(200, 200); // 원형 프로그레스바 크기 조정
    dial->setMaximumSize(200, 200);
    topLayout->addWidget(dial, 2); // 다이얼의 비율 설정

    // 상태 박스 초기화
    QGridLayout *statusLayout = new QGridLayout();
    safeLabel = new QLabel("SAFE");
    normalLabel = new QLabel("NORMAL");
    warningLabel = new QLabel("WARNING");
    dangerLabel = new QLabel("DANGER");

    QList<QLabel *> labels = {safeLabel, normalLabel, warningLabel, dangerLabel};
    for (QLabel *label : labels) {
        label->setStyleSheet("background-color: gray; color: white; font-size: 14px; font-weight: bold;");
        label->setFixedSize(100, 100);
        label->setAlignment(Qt::AlignCenter);
    }

    statusLayout->addWidget(safeLabel, 0, 0);
    statusLayout->addWidget(normalLabel, 0, 1);
    statusLayout->addWidget(warningLabel, 1, 0);
    statusLayout->addWidget(dangerLabel, 1, 1);
    topLayout->addLayout(statusLayout, 3);

    // 차트 초기화
    chart->addSeries(series);

    // 그래프 배경 스타일 설정
    chart->setBackgroundBrush(QBrush(QColor("#1D1D35"))); // 어두운 배경색

    // X축 초기화
    auto *axisX = new QValueAxis();
    axisX->setTitleText("Time");
    axisX->setLabelFormat("%d"); // 정수형 포맷
    axisX->setGridLineVisible(false); // X축 그리드 라인 비활성화
    axisX->setLinePenColor(Qt::white);         // X축 선 색상
    axisX->setLabelsColor(Qt::white);          // X축 텍스트 색상
    chart->setAxisX(axisX, series);

    // Y축 초기화 및 범위 설정
    auto *axisY = new QValueAxis();
    axisY->setTitleText("Costs");
    axisY->setRange(0, 400);                    // Y축 범위 설정
    axisY->setGridLineVisible(false);           // Y축 그리드 라인 비활성화
    axisY->setLinePenColor(Qt::white);         // Y축 선 색상
    axisY->setLabelsColor(Qt::white);          // Y축 텍스트 색상
    chart->setAxisY(axisY, series);

    // 데이터 포인트 및 라인 스타일 설정
    series->setColor(QColor("#9575CD"));         // 라인 색상
    series->setPointsVisible(false);            // 데이터 포인트 표시 비활성화
    series->setPointLabelsVisible(false);       // 데이터 포인트 라벨 비활성화

    chart->setTitle("Time vs Costs");
    chart->setTitleBrush(QBrush(Qt::white));    // 제목 색상
    chart->setAnimationOptions(QChart::NoAnimation);

    // 메인 레이아웃 구성
    mainLayout->addLayout(topLayout, 3);
    mainLayout->addWidget(chartView, 2);
}

void ChartWidget::updateChart(const QString &timestamp, QString &costs) {
    qDebug() << "Updating chart with timestamp:" << timestamp << ", costs:" << costs;

    // 프로그램 시작 이후 경과 시간 계산
    QTime currentTime = QTime::currentTime();
    int elapsedSeconds = startTime.secsTo(currentTime); // 경과 시간 (초)

    // 초를 분:초 형식으로 변환
    QString elapsedTime = QString("%1:%2")
                              .arg(elapsedSeconds / 60, 2, 10, QChar('0'))
                              .arg(elapsedSeconds % 60, 2, 10, QChar('0'));

    qDebug() << "Elapsed time (mm:ss):" << elapsedTime;

    // 다이얼 업데이트
    dial->setValue(costs.toInt());

    // 상태 업데이트
    updateStatus(costs.toInt());

    // 시리즈에 데이터 추가
    static int timeCounter = 0; // 데이터 포인트의 X값 (순서)
    series->append(timeCounter++, costs.toInt());

    // X축 범위 업데이트
    auto *axisX = static_cast<QValueAxis *>(chart->axisX());
    axisX->setRange(0, timeCounter > 10 ? timeCounter : 10); // 최근 10초 표시
}

void ChartWidget::updateStatus(int value) {
    // 모든 상태 초기화
    safeLabel->setStyleSheet("background-color: gray; color: white; font-size: 14px; font-weight: bold;");
    normalLabel->setStyleSheet("background-color: gray; color: white; font-size: 14px; font-weight: bold;");
    warningLabel->setStyleSheet("background-color: gray; color: white; font-size: 14px; font-weight: bold;");
    dangerLabel->setStyleSheet("background-color: gray; color: white; font-size: 14px; font-weight: bold;");

    // 현재 값에 따라 상태 활성화
    if (value < 8) {
        safeLabel->setStyleSheet("background-color: #2B5B43; color: white; font-size: 14px; font-weight: bold;"); // 어두운 녹색
    } else if (value < 20 && value >= 8) {
        normalLabel->setStyleSheet("background-color: #C4AA4D; color: black; font-size: 14px; font-weight: bold;"); // 칙칙한 황금색
    } else if (value < 100 && value >= 20) {
        warningLabel->setStyleSheet("background-color: #B3651A; color: white; font-size: 14px; font-weight: bold;"); // 어두운 주황색
    } else {
        dangerLabel->setStyleSheet("background-color: #7F1D1D; color: white; font-size: 14px; font-weight: bold;"); // 어두운 빨간색
    }
}
