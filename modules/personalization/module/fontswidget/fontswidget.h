#ifndef FONTSWIDGET_H
#define FONTSWIDGET_H

#include "contentwidget.h"

#include <QObject>
#include <QVBoxLayout>
#include <translucentframe.h>
#include <QDebug>
namespace dcc
{
class ContentWidget;
namespace widgets
{
class TranslucentFrame;
}
namespace personalization
{
class FontSizeWidget;
class FontSettingsWidget;
class PersonalizationModel;
class FontsWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit FontsWidget(QWidget *parent = 0);

signals:
    void showStandardFont();
    void showMonoFont();
    void requestSetFontSize(const int value);

public slots:
    void setModel(PersonalizationModel *const model);

private:
    QVBoxLayout        *m_mainlayout;
    widgets::TranslucentFrame   *m_widget;
    FontSizeWidget     *m_fontSizeWidget;
    FontSettingsWidget *m_fontSettingsWidget;
};

}
}

#endif // FONTSWIDGET_H
