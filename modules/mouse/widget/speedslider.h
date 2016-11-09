#ifndef DOUSLIDER_H
#define DOUSLIDER_H
#include "settingsitem.h"
#include <QLabel>
#include "dslider.h"
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QObject>
#include <QDebug>
DWIDGET_USE_NAMESPACE
using namespace dcc;
class SpeedSlider : public SettingsItem
{
    Q_OBJECT
public:
    explicit SpeedSlider(QObject *parent = 0);

signals:
    void requestSetSliderValue(const int &value);

public slots:
    void setTitle(const QString &title);
    void setValue(const int &value);
    void setMaxValue(const int &value);
    void setMinValue(const int &value);
    void setStep(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    QSlider     *m_speedSlider;
    QLabel      *m_title;
};

#endif // DOUSLIDER_H
