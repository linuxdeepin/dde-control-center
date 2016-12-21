#ifndef FONTSIZEWIDGET_H
#define FONTSIZEWIDGET_H

#include "translucentframe.h"
#include <settingsgroup.h>
#include <QObject>
#include <QSlider>

namespace dcc
{
namespace widgets
{
class TranslucentFrame;
class SettingsGroup;
class TitledSliderItem;
}
namespace personalization
{
class FontSizeModel;
class FontSizeWidget : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit FontSizeWidget(QWidget *parent = 0);

signals:
    void requestSetFontSize(const int value);

public slots:
    void setModel(FontSizeModel *const model);
    void setFontSize(int size);

private:
    int delayToSliderValue(const int delay) const;
    void setFontSizeTip(int size);
    QString delayToLiteralString(const int delay) const;

private:
    QVBoxLayout   *m_mainlayout;
    widgets::SettingsGroup *m_mainWidget;
    widgets::TitledSliderItem *m_sizeWidget;
    QSlider          *m_fontSlider;
};
}
}

#endif // FONTSIZEWIDGET_H
