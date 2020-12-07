/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

public:
    int showPath(const QString &path);
    void setDefaultWidget();

Q_SIGNALS:
    void requsetSpeakerPage();
    void requestMicrophonePage();
    void requestAdvancedPage();
    void requsetSoundEffectsPage();

private:
    void initMenuUI();

private:
    QList<ListSubItem> m_menuMethod;
    dcc::widgets::MultiSelectListView *m_menuList{nullptr};
    QModelIndex m_currentIdx;
};
}

}


#endif // SoundWidget_H_V20
