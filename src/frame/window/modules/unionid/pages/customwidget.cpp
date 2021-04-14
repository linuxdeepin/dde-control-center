#include "customwidget.h"
#include "QPainter"
#include <QGraphicsBlurEffect>

CustomWidget::CustomWidget(QWidget *parent) : QWidget(parent)
{
//    QGraphicsBlurEffect *blurEffect =  new QGraphicsBlurEffect(this);
//    blurEffect->setBlurRadius(0.8);
//    setGraphicsEffect(blurEffect);
//    blurEffect->deleteLater();
    m_type = DGuiApplicationHelper::instance()->themeType();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CustomWidget::onThemeTypeChanged);
}

//void CustomWidget::paintEvent(QPaintEvent *event)
//{
//    QPainter *painter = new QPainter(this);

//    if (DGuiApplicationHelper::LightType == m_type) {
//        QColor color(255, 255, 255);
//        color.setAlphaF(0.6);
//        painter->fillRect(rect(), color);
//    } else if (DGuiApplicationHelper::DarkType == m_type) {
//        QColor color(20, 20, 20);
//        color.setAlphaF(0.6);
//        painter->fillRect(rect(), color);
//    }

//    painter->end();
//    delete painter;
//    return QWidget::paintEvent(event);
//}

void CustomWidget::onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType)
{
    m_type = themeType;
    update();
}
