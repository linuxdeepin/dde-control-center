// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SoundWidget_H_V20
#define SoundWidget_H_V20

#include "interface/namespace.h"
#include "window/utils.h"
#include "window/insertplugin.h"

#include <DListView>

#include <QWidget>
#include <QMetaMethod>
#include <QModelIndex>

class QListView;

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {

namespace sound {

class SoundWidget : public QWidget
{
    Q_OBJECT
public:
    SoundWidget(QWidget *parent = nullptr);
    virtual ~SoundWidget();

public:
    int showPath(const QString &path);
    void showDefaultWidget();
    void setSubItemHidden(const QString &item, bool hide);

Q_SIGNALS:
    void requsetSpeakerPage();
    void requestMicrophonePage();
    void requestAdvancedPage();
    void requsetSoundEffectsPage();
    void requsetDeviceManagesPage();
    void requestUpdateSecondMenu(bool);

private:
    void initUi();
    void initMembers();
    void initConnections();
    bool configContent(const QString & configName);

private:
    QList<ListSubItem> m_itemList;
    dcc::widgets::MultiSelectListView *m_listView;
    QStandardItemModel *m_itemModel;
    QModelIndex m_currentIdx;
};
}

}


#endif // SoundWidget_H_V20
