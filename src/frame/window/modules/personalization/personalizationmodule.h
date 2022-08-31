// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
class MainWindow;
namespace personalization {
class PersonalizationList;
class PersonalizationModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit PersonalizationModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~PersonalizationModule();

    virtual void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual void contentPopped(QWidget *const w) override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

Q_SIGNALS:
    void requestSetCurrentIndex(int row);

private Q_SLOTS:
    void showGenaralWidget();
    void showIconThemeWidget();
    void showCursorThemeWidget();
    void showFontThemeWidget();

protected:
    void initSearchData() override;

private:
    dcc::personalization::PersonalizationModel  *m_model;
    dcc::personalization::PersonalizationWork   *m_work;
    MainWindow *m_pMainWindow;
};
}
}
