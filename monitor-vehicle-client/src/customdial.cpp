#include "customdial.h"
#include <QPainter>
#include <QPaintEvent>
#include <QtMath> // qDegreesToRadians, qCos, qSin 사용을 위해 추가

CustomDial::CustomDial(QWidget *parent)
    : QWidget(parent),
      currentValue(0)
{
    setMinimumSize(450, 600); // 크기를 2배로 키움
}

void CustomDial::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 정사각형 영역 계산
    int size = qMin(width(), height()); // 너비와 높이 중 작은 값을 기준으로 정사각형 결정
    QRectF rect((width() - size) / 2, (height() - size) / 2, size, size);

    // Progress 원 배경 그리기
    int thickness = 10; // 테두리 두께를 설정
    QRectF innerRect = rect.adjusted(thickness, thickness, -thickness, -thickness);

    painter.setPen(QPen(QColor("#5150b5"), thickness, Qt::SolidLine, Qt::RoundCap));
    painter.drawArc(innerRect, 225 * 16, -270 * 16); // 원형 배경

    // Progress 내부 원 그리기
    painter.setPen(QPen(QColor("#9575CD"), thickness, Qt::SolidLine, Qt::RoundCap));
    int progressAngle = -currentValue * 2.7 * 16;
    painter.drawArc(innerRect, 225 * 16, progressAngle); // 진행 표시

    // Progress 끝 점 그리기
    painter.setPen(QPen(QColor(255, 255, 255), 8, Qt::SolidLine, Qt::RoundCap));
    double endAngle = 225 - currentValue * 2.7;
    QPointF endPoint(
        innerRect.center().x() + innerRect.width() / 2 * qCos(qDegreesToRadians(endAngle)),
        innerRect.center().y() - innerRect.height() / 2 * qSin(qDegreesToRadians(endAngle))
    );
    painter.drawPoint(endPoint);

    // Value 텍스트 그리기
    painter.setPen(QPen(QColor(117, 143, 143)));
    painter.setFont(QFont("Arial", 30, QFont::Bold)); // 글자 크기 증가
    painter.drawText(rect, Qt::AlignCenter, QString::number(currentValue));

    // RISK 텍스트 그리기
    QRectF riskRect = rect.adjusted(0, size * 0.50 , 0, 0); // 숫자 아래 위치 조정
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(riskRect, Qt::AlignCenter, "RISK");
}

void CustomDial::setValue(int value)
{
    currentValue = qBound(0, value, 100); // 값 제한
    update();
}
