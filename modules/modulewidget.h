#ifndef MODULEWIDGET_H
#define MODULEWIDGET_H

#include "labels/largelabel.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

namespace dcc {
namespace widgets {
class LargeLabel;
}
}

namespace dcc {

class ModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleWidget();
    void setTitle(const QString &title);

private:
    bool event(QEvent *event);

protected:
    QLabel *m_moduleIcon;
    dcc::widgets::LargeLabel *m_moduleTitle;
    QVBoxLayout *m_centralLayout;
};

}

#endif // MODULEWIDGET_H
