#ifndef FONTSETTINGSWIDGET_H
#define FONTSETTINGSWIDGET_H

#include "contentwidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "../../model/fontmodel.h"
#include <QObject>
#include <QVBoxLayout>
#include <translucentframe.h>
#include <QDebug>

using namespace dcc::widgets;
class FontSettingsWidget : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit FontSettingsWidget(QWidget *parent = 0);
    void setFontStandModel(FontModel * const model);
    void setFontMonoModel(FontModel * const model);

signals:
    void showStandardFont();
    void showMonoFont();

private:
    QVBoxLayout    *m_mainlayout;
    SettingsGroup  *m_fontWidget;
    NextPageWidget *m_standard;
    NextPageWidget *m_mono;
};

#endif // FONTSETTINGSWIDGET_H
