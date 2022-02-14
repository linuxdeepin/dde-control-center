#pragma once

#include "interface/moduleinterface.h"
#include "widgets/settingsitem.h"
#include "modules/authentication/widgets/disclaimersdialog.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DSuggestButton;
class DCommandLinkButton;
DWIDGET_END_NAMESPACE

namespace dcc {
namespace authentication {
// 免责声明
class DisclaimersItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit DisclaimersItem(DisclaimersObj disobj, QWidget *parent = nullptr);

public Q_SLOTS:
    void showDisclaimers();
    void setAcceptState(const bool &state);

Q_SIGNALS:
    void requestStateChange(bool state);
    void requestSetWindowEnabled(bool checked = false);

private:
    QHBoxLayout *m_layout;
    QCheckBox *m_acceptCheck;
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_disclaimersBtn; // 免责声明按钮
    DisclaimersObj m_state;
};

}
}
