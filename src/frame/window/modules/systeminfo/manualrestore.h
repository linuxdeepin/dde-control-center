#pragma once

#include "namespace.h"
#include <QWidget>
#include <dfilechooseredit.h>

DWIDGET_USE_NAMESPACE

class QLabel;
class QPushButton;
class RestoreItem;
class QCheckBox;

namespace DCC_NAMESPACE {
namespace systeminfo {
class ManualRestore : public QWidget
{
    Q_OBJECT
public:
    explicit ManualRestore(QWidget *parent = nullptr);

private Q_SLOTS:
    void onItemChecked();
    void restore();
    void restoreSystem();
    void restoreManual();

private:
    enum class ActionType {
        RestoreSystem,
        ManualRestore
    };

private:
    QCheckBox* m_saveUserDataCheckBox;
    DFileChooserEdit* m_directoryChooseWidget;
    QLabel          * m_tipsLabel;
    QPushButton* m_backupBtn;
    RestoreItem* m_systemRestore;
    RestoreItem* m_manualRestore;
    ActionType m_actionType;
};
}
}
