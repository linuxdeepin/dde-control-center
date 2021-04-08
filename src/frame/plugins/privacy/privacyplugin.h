/*
 * Copyright (C) 2020 Deepin Technology Co., Ltd.
 *
 * Author:     xiaoyaobing <xiaoyaobing@uniontech.com>
 *
 * Maintainer: xiaoyaobing <xiaoyaobing@uniontech.com>
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

#ifndef PRIVACYPLUGIN_H
#define PRIVACYPLUGIN_H

#include "privacywidget.h"
#include "locationwidget.h"

#include <interface/namespace.h>
#include <interface/moduleinterface.h>
#include <interface/frameproxyinterface.h>
#include <widgets/backbutton.h>

#include <QObject>
#include <QIcon>
#include <QPointer>

using namespace dcc;
using namespace DCC_NAMESPACE;

enum PrivacyType {
    Locayion,
    Photo,
    Bluetooth,
    Microphone
};

class PrivacyPlugin : public QObject , public ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ModuleInterface_iid)
    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)
public:
    PrivacyPlugin();
    ~PrivacyPlugin() override;

    QIcon icon() const override;

    virtual void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;

    /**
     * @brief translationPath 返回系统语言翻译文件路径
     * @return 翻译文件路径
     */
    QString translationPath() const override;
    void deactive() override;
    void contentPopped(QWidget *const w) override;
    void reset() override;

private Q_SLOTS:
    void ShowPrivacyView(int index);
private:
    QTranslator *m_translator{nullptr};
    PrivacyWidget *m_privacyWidget{nullptr};
    LocationWidget *m_locationWidget{nullptr};
};

#endif // PRIVACYPLUGIN_H
