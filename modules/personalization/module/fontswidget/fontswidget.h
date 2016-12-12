#ifndef FONTSWIDGET_H
#define FONTSWIDGET_H

#include "contentwidget.h"
#include "fontsizewidget.h"
#include "fontsettingswidget.h"
#include "nextpagewidget.h"
#include "personalization/personalizationmodel.h"
#include <QObject>
#include <QVBoxLayout>
#include <translucentframe.h>
#include <QDebug>
using namespace dcc;
using namespace dcc::widgets;

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
    void setModel(PersonalizationModel * const model);

private:
    QVBoxLayout        *m_mainlayout;
    TranslucentFrame   *m_widget;
    FontSizeWidget     *m_fontSizeWidget;
    FontSettingsWidget *m_fontSettingsWidget;
};

#endif // FONTSWIDGET_H
