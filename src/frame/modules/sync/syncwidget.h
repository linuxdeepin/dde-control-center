#ifndef SYNCWIDGET_H
#define SYNCWIDGET_H

#include "modules/modulewidget.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace dcc {
namespace sync {
class SyncWidget : public ModuleWidget {
    Q_OBJECT
public:
    explicit SyncWidget();

Q_SIGNALS:
    void requestLogin() const;

public Q_SLOTS:

private:
    QPushButton *m_accountBtn;
};
}  // namespace sync
}  // namespace dcc

#endif  // SYNCWIDGET_H
