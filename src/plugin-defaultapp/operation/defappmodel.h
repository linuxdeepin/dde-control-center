// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DEFAPPMODEL_H
#define DEFAPPMODEL_H

#include "category.h"
#include "categorymodel.h"
#include "dccfactory.h"

#include <QObject>
#include <QVariantMap>

class Category;
class DefAppWorker;
class DefAppWorkerOld;

class DefAppModel : public QObject
{
    Q_OBJECT
public:
    explicit DefAppModel(QObject *parent = 0);
    ~DefAppModel();

    enum DefAppCategory {
        Browser = 0,
        Mail,
        Text,
        Music,
        Video,
        Picture,
        Terminal,
        Count,
    };

public Q_SLOTS:

    inline CategoryModel *browser() const { return m_categoryModel[Browser]; }

    inline CategoryModel *mail() const { return m_categoryModel[Mail]; }

    inline CategoryModel *text() const { return m_categoryModel[Text]; }

    inline CategoryModel *music() const { return m_categoryModel[Music]; }

    inline CategoryModel *video() const { return m_categoryModel[Video]; }

    inline CategoryModel *picture() const { return m_categoryModel[Picture]; }

    inline CategoryModel *terminal() const { return m_categoryModel[Terminal]; }

    inline Category *getModBrowser() const { return m_categoryModel[Browser]->category(); }

    inline Category *getModMail() const { return m_categoryModel[Mail]->category(); }

    inline Category *getModText() const { return m_categoryModel[Text]->category(); }

    inline Category *getModMusic() const { return m_categoryModel[Music]->category(); }

    inline Category *getModVideo() const { return m_categoryModel[Video]->category(); }

    inline Category *getModPicture() const { return m_categoryModel[Picture]->category(); }

    inline Category *getModTerminal() const { return m_categoryModel[Terminal]->category(); }

    // v2 工厂接口：由 DCC_FULL_FACTORY_CLASS 转发
    static void initialized();
    void active();
    void deactive();
    dccV25::DataReply get(const QVariantMap &param, const QString &caller);
    dccV25::DataReply set(const QVariantMap &param, const QString &caller);

    // 数据通道用：类别下标即 DefAppCategory，越界或为空返回 nullptr
    inline Category *categoryAt(int category) const
    {
        return (category >= 0 && category < Count && m_categoryModel[category])
                ? m_categoryModel[category]->category()
                : nullptr;
    }
    // 把某类别的默认项写入后端（与 UI 走同一条 Worker 路径）
    bool applyDefaultById(int category, const QString &desktopId);

Q_SIGNALS:
    // 模块级变更通知，由 DCC_FULL_FACTORY_CLASS 转发到 DccFactory_v20
    void propertiesChanged(const QVariantMap &properties);

private:
    void startBackend();
    CategoryModel *m_categoryModel[Count];
    DefAppWorker *m_work;
    DefAppWorkerOld *m_oldwork;
    bool m_isOldInterface;
    bool m_backendStarted = false;
};
#endif // DEFAPPMODEL_H
