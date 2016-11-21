#ifndef SPEEDSLIDER_H
#define SPEEDSLIDER_H
#include "settingsitem.h"
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QObject>
#include <QDebug>
using namespace dcc;
class WacomPressure : public SettingsItem
{
    Q_OBJECT
public:
    explicit WacomPressure(QObject *parent = 0);

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

#endif // SPEEDSLIDER_H
