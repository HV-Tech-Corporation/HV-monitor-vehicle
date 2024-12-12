#ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

class CustomDial : public QWidget
{
    Q_OBJECT
public:
    explicit CustomDial(QWidget *parent = nullptr);

    void setValue(int value);  // 다이얼 값 설정
    int getValue() const;      // 현재 다이얼 값 가져오기

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int currentValue;          // 현재 다이얼 값
    QString title;             // 다이얼 제목
};

#endif // CUSTOMDIAL_H
