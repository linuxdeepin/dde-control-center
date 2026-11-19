// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DEFAPPMODEL_H
#define DEFAPPMODEL_H

#include "category.h"
#include "categorymodel.h"
#include "dccfactory.h"

#include <QObject>

class Category;
class DefaultAppData;
class DefAppWorker;
class DefAppWorkerOld;
class QTimer;

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

    // 读取失败后的自动重试退避：从 2s 起翻倍，封顶 30s。
    // 不设硬次数上限——上限耗尽会让持续故障变成永久 NotReady，
    // 而退避本身已把"无限打后端"限制成低频探测。
    static constexpr int kInitialRetryDelayMs = 2000;
    static constexpr int kMaxRetryDelayMs = 30000;
    static constexpr int nextRetryDelay(int current)
    {
        return current >= kMaxRetryDelayMs / 2 ? kMaxRetryDelayMs : current * 2;
    }

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

    // from DccFactory_v20，由 DCC_FULL_FACTORY_CLASS 转发
    static void initialized();
    void active();
    void deactive();
    dccV25::DataReply get(const QVariantMap &param, const QString &caller,
                          const QString &callerPath);
    dccV25::DataReply set(const QVariantMap &param, const QString &caller,
                          const QString &callerPath);

    // 数据快照访问，主线程调用
    inline Category *categoryAt(int category) const
    {
        return (category >= 0 && category < Count && m_categoryModel[category])
                ? m_categoryModel[category]->category()
                : nullptr;
    }
    DefaultAppData *dataChannel() const { return m_dataChannel; }
    // 把 App 描述暴露给写入校验之外的结果汇总
    bool applyDefaultById(const QString &key, const QString &desktopId);

Q_SIGNALS:
    void propertiesChanged(const QVariantMap &properties);

private:
    void startBackend();
    // 主线程：把 Category 当前状态同步进数据快照
    void refreshSnapshot();
    // 主线程：对所有读取失败的类别重新发起一次读取
    void retryUnresolvedCategories();
    void writeDefaultToBackend(const QString &categoryName, const App &app);

private:
    CategoryModel *m_categoryModel[Count];
    DefAppWorker *m_work;
    DefAppWorkerOld *m_oldwork;
    DefaultAppData *m_dataChannel;
    QTimer *m_retryTimer;
    bool m_isOldInterface;
    bool m_backendStarted = false;
    bool m_deactivated = false;
    // 当前重试退避间隔；成功后回到 kInitialRetryDelayMs，失败则翻倍封顶
    int m_retryDelayMs = kInitialRetryDelayMs;
};
#endif // DEFAPPMODEL_H
