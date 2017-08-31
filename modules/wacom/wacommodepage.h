#ifndef WACOMMODEPAGE_H
#define WACOMMODEPAGE_H

#include "contentwidget.h"
#include <QWidget>

namespace dcc {
namespace widgets {
class OptionItem;
}
namespace wacom {

class WacomModePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit WacomModePage(QWidget *parent = nullptr);

    void setMode(const bool mode);

signals:
    void requestSetMode(const bool mode);

private slots:
    void modeSelected();

private:
    dcc::widgets::OptionItem *m_penMode;
    dcc::widgets::OptionItem *m_mouseMode;
};
}
}


#endif // WACOMMODEPAGE_H
