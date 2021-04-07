#include "customwidget.h"
#include "QPainter"

CustomWidget::CustomWidget(QWidget *parent) : QWidget(parent)
{
    m_type = DGuiApplicationHelper::instance()->themeType();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CustomWidget::onThemeTypeChanged);
}

void CustomWidget::paintEvent(QPaintEvent *event)
{
    if (DGuiApplicationHelper::LightType == m_type) {
        QPainter painter(this);
        QColor color(255, 255, 255);
        color.setAlphaF(0.9);
        painter.fillRect(rect(), color);
    } else if (DGuiApplicationHelper::DarkType == m_type) {
        QPainter painter(this);
        QColor color(20, 20, 20);
        color.setAlphaF(0.9);
        painter.fillRect(rect(), color);
    }
    return QWidget::paintEvent(event);
}

void CustomWidget::onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType)
{
    m_type = themeType;
    update();
}
