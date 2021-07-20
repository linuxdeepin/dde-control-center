#pragma once

#include "interface/namespace.h"
#include <QWidget>
#include <DButtonBox>
#include <DPushButton>

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/titlevalueitem.h"

#define RESTORE_TOOL_PATH "/usr/local/bin/update"

DWIDGET_USE_NAMESPACE

using dcc::widgets::SettingsGroup;
using dcc::widgets::TitleAuthorizedItem;

namespace DCC_NAMESPACE {


namespace systeminfo {

class CRestoreClientPage :public dcc::ContentWidget {
    Q_OBJECT
public:
    explicit  CRestoreClientPage(QWidget *parent=nullptr);

private:
    void initUI();
    void restore();
private:
};
}
}
