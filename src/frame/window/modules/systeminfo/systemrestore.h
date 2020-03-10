#pragma once

#include "interface/namespace.h"
#include <QWidget>
#include <DButtonBox>

DWIDGET_USE_NAMESPACE

class QPushButton;
namespace DCC_NAMESPACE {
namespace systeminfo {
class ManualBackup;
class ManualRestore;
class SystemRestore : public QWidget
{
    Q_OBJECT
public:
    SystemRestore(QWidget *parent = nullptr);

private:
    DButtonBox* m_buttonBox;
    ManualBackup* m_backupPage;
    ManualRestore* m_restorePage;
};
}  // namespace systeminfo
}  // namespace DCC_NAMESPACE
