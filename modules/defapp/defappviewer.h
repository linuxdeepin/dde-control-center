/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef DEFAPPVIEWER_H
#define DEFAPPVIEWER_H

#include "contentwidget.h"
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QFileInfo>

namespace dcc
{
namespace widgets {
class SwitchWidget;
class SettingsGroup;
class TranslucentFrame;
}
namespace defapp
{
class DefCategoryWidget;
class DefAppModel;
class DefAppViewer : public ContentWidget
{
    Q_OBJECT

public:
    explicit DefAppViewer(QWidget *parent = 0);
    QWidget *defappDetail();
    void setModel(DefAppModel * const model);

signals:
    void requestSetDefaultApp(const QString &category, const QJsonObject &item);
    void requestDelUserApp(const QString &name, const QJsonObject &item);
    void autoOpenChanged(const bool state);
    void requestFrameAutoHide(const bool autoHide) const;
    void requestCreateFile(const QString &category, const QFileInfo &info);

private:

    DefCategoryWidget *m_modBrowser = nullptr;
    DefCategoryWidget *m_modMail = nullptr;
    DefCategoryWidget *m_modText = nullptr;
    DefCategoryWidget *m_modMusic = nullptr;
    DefCategoryWidget *m_modVideo = nullptr;
    DefCategoryWidget *m_modPicture = nullptr;
    DefCategoryWidget *m_modTerminal = nullptr;
};
}
}

#endif // DEFAPPVIEWER_H
