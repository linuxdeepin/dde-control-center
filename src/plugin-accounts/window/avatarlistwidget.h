// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "avatarlistframe.h"
#include "avatarlistview.h"
#include "interface/namespace.h"
#include "operation/accountsworker.h"

#include <DBlurEffectWidget>
#include <DDialog>
#include <DListView>

#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QWidget>

#include <optional>

DWIDGET_BEGIN_NAMESPACE
class DFrame;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {
class CustomAddAvatarWidget;
class CustomAvatarWidget;

class AvatarListDialog : public Dtk::Widget::DAbstractDialog
{
    Q_OBJECT
public:
    // 头像选择项
    struct AvatarItem
    {
        QString name;
        QString icon;
        int role;
        bool isLoader;

        AvatarItem(const QString &_name,
                   const QString &_icon,
                   const int &_role,
                   const bool &_isLoader)
            : name(_name)
            , icon(_icon)
            , role(_role)
            , isLoader(_isLoader)
        {
        }
    };

    enum AvatarItemRole { AvatarItemNameRole = DTK_NAMESPACE::UserRole + 1, AvatarItemIconRole };

    explicit AvatarListDialog(User *user, AccountsWorker *m_worker, QWidget *parent = nullptr);
    virtual ~AvatarListDialog();

    QString getAvatarPath() const;

    inline std::optional<QString> get_path() const { return m_path; }

private:
    CustomAvatarWidget *getCustomAvatarWidget();
    void handleListViewRequestUpdate(bool isSave, const int &role, const int &type);
    void handleRequestDeleteIcon(const QString &iconPath);

private:
    AccountsWorker *m_worker;
    User *m_curUser{ nullptr };
    QHBoxLayout *m_mainContentLayout;
    QVBoxLayout *m_leftContentLayout;
    QVBoxLayout *m_rightContentLayout;
    DTK_WIDGET_NAMESPACE::DListView *m_avatarSelectItem;
    QStandardItemModel *m_avatarSelectItemModel;
    AvatarListFrame *m_currentSelectAvatarWidget;
    QMap<int, AvatarListFrame *> m_avatarFrames;
    std::optional<QString> m_path;
};
} // namespace DCC_NAMESPACE
