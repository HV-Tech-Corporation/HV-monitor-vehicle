#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>
#include <QLabel>
#include "customdial.h"

#include <QTime>

QT_CHARTS_USE_NAMESPACE

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = nullptr);

    // 차트 업데이트 슬롯
    void updateChart(const QString &timestamp, QString &costs);

private:
    // 내부 상태 업데이트 함수
    void updateStatus(int value);

    CustomDial *dial;                   // 커스텀 다이얼
    QChart *chart;                      // 차트 객체
    QLineSeries *series;                // 차트 데이터 시리즈
    QChartView *chartView;              // 차트 뷰어
    QVBoxLayout *mainLayout;            // 메인 레이아웃

    QLabel *safeLabel;                  // SAFE 상태 라벨
    QLabel *normalLabel;                // NORMAL 상태 라벨
    QLabel *warningLabel;               // WARNING 상태 라벨
    QLabel *dangerLabel;                // DANGER 상태 라벨
    
    QTime startTime;
};

#endif // CHARTWIDGET_H
