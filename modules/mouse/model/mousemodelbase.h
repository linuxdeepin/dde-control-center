#ifndef MOUSEMODELBASE_H
#define MOUSEMODELBASE_H

#include <QObject>
#include <QDebug>

class MouseModelBase : public QObject
{
    Q_OBJECT
public:
    explicit MouseModelBase(QObject *parent = 0);
    inline void setName(const QString &name) { m_name = name;}
    inline const QString getName() { return m_name;}
    void setSliderValue(const int &value);
    inline bool getSliderValue() {return m_sliderValue;}
    void setSliderMaxValue(const int &value);
    void setSliderMinValue(const int &value);
    inline int getSliderMaxValue() {return m_sliderMaxValue;}
    inline int getSliderMinValue() {return m_sliderMinValue;}
    void setSliderStep(const int &value);
    inline int getSliderStep() {return m_sliderStep;}

signals:
    void sliderValueChanged(const int &value);

private:
    QString m_name;
    int m_sliderValue;
    int m_sliderMaxValue;
    int m_sliderMinValue;
    int m_sliderStep;
};
#endif // MOUSEMODELBASE_H
