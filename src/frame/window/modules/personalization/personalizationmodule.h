/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#pragma once

#include "interface/namespace.h"
#include "interface/moduleinterface.h"

#include <QObject>

namespace dcc
{
namespace personalization
{
class PersonalizationModel;
class PersonalizationWork;
}
}

namespace DCC_NAMESPACE {
namespace personalization {
class PersonalizationList;
class PersonalizationModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit PersonalizationModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~PersonalizationModule();

    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual void contentPopped(QWidget *const w) override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;

Q_SIGNALS:
    void requestSetCurrentIndex(int row);

private Q_SLOTS:
    void showGenaralWidget();
    void showIconThemeWidget();
    void showCursorThemeWidget();
    void showFontThemeWidget();
    void showWallpaperPage();

private:
    dcc::personalization::PersonalizationModel  *m_model;
    dcc::personalization::PersonalizationWork   *m_work;
};
}
}
