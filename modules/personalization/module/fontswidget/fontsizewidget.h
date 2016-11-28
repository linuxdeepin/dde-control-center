#ifndef FONTSIZEWIDGET_H
#define FONTSIZEWIDGET_H


#include "translucentframe.h"
#include "../../widget/fontitem.h"
#include "../../model/fontsizemodel.h"
#include "titledslideritem.h"
#include "dccslider.h"
#include <settingsgroup.h>
#include <QObject>
#include <QSlider>

using namespace dcc;
using namespace dcc::widgets;

class FontSizeWidget : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit FontSizeWidget(QWidget *parent = 0);

signals:
    void requestSetFontSize(const int value);

public slots:
    void setModel(FontSizeModel * const model);
    void setFontSize(int size);

private:
    int delayToSliderValue(const int delay) const;
    void setFontSizeTip(int size);
    QString delayToLiteralString(const int delay) const;

private:
    QVBoxLayout   *m_mainlayout;
    SettingsGroup *m_mainWidget;
    TitledSliderItem *m_sizeWidget;
    QSlider          *m_fontSlider;
};

#endif // FONTSIZEWIDGET_H
