#ifndef POWERSWITCHWIDGET_H
#define POWERSWITCHWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include <dswitchbutton.h>

#include "doublecornerwidget.h"

DWIDGET_USE_NAMESPACE
class PowerSwitchWidget: public QWidget {
    Q_OBJECT
public:
    PowerSwitchWidget(QString title, QWidget* parent=0);
    ~PowerSwitchWidget();
//    using DoubleCornerWidget::setDirection;
signals:
    void checkedChanged(bool checked);
public slots:
    void setChecked(bool checked);
private:
    QString m_title;
    QLabel* m_titleLabel;
    DSwitchButton* m_switchButton;
    QHBoxLayout* m_Layout;
};
DWIDGET_USE_NAMESPACE
#endif // POWERSWITCHWIDGET_H
