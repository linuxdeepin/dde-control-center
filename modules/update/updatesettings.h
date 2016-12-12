#ifndef UPDATECTROLMIRRORS_H
#define UPDATECTROLMIRRORS_H

#include "contentwidget.h"
#include "switchwidget.h"
#include "nextpagewidget.h"

using namespace dcc::widgets;

namespace dcc{
namespace update{

class UpdateModel;

class UpdateSettings : public ContentWidget
{
    Q_OBJECT

public:
    explicit UpdateSettings(UpdateModel* model, QWidget *parent = 0);
    void setDefaultMirror(const QString& value);

signals:
    void mirrors();
    void autoUpdate(bool autoUpdate);

public slots:
    void setAutoUpdate(bool update);

private:
    UpdateModel* m_model;
    SwitchWidget* m_autoUpdate;
    NextPageWidget* m_updateMirrors;
};

}
}
#endif // UPDATECTROLMIRRORS_H
