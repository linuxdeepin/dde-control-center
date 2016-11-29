#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include "modulewidget.h"
#include "displaymodel.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"

#include <QPushButton>

namespace dcc {

namespace display {

class DisplayWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit DisplayWidget();

    void setModel(DisplayModel *model);

signals:
    void showResolutionPage() const;
    void requestRotate() const;
    void requestCustom() const;

private slots:
    void onScreenListChanged() const;

private:
    DisplayModel *m_model;
    NextPageWidget *m_resolution;
    QPushButton *m_rotate;
    QPushButton *m_custom;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWIDGET_H
