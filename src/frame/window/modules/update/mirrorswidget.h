// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <types/mirrorinfolist.h>
#include <dimagebutton.h>
#include <DListView>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace update {
class UpdateModel;
class MirrorItem;
}

namespace widgets {
class SettingsGroup;
}
}

namespace DCC_NAMESPACE {
namespace update {

class MirrorSourceItem;

class MirrorsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MirrorsWidget(dcc::update::UpdateModel *model, QWidget *parent = 0);
    ~MirrorsWidget();

    void setModel(dcc::update::UpdateModel *model);

protected:
    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:
    void requestSetDefaultMirror(const MirrorInfo &mirror);
    void requestTestMirrorSpeed();
    void notifyDestroy();

private Q_SLOTS:
    void onSpeedInfoAvailable(const QMap<QString, int> &info);
    void testButtonClicked();

    void sortMirrorsBySpeed();

private:
    void setDefaultMirror(const MirrorInfo &mirror);
    void setMirrorInfoList(const MirrorInfoList &list);

private:
    enum TestProgress {
        NotStarted,
        Running,
        Done
    };

    MirrorInfo m_defaultMirror;
    TestProgress m_testProgress;
    QPushButton *m_testButton;
    Dtk::Widget::DListView *m_view;
    QStandardItemModel *m_model;
    int m_mirrorSourceNo;
    QWidget *m_listWidget;
};

} // namespace update
} // namespace DCC_NAMESPACE

