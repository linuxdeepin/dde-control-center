// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "usergroupspage.h"
#include "groupitem.h"
#include "widgets/translucentframe.h"

#include <DApplicationHelper>

#include <QVBoxLayout>

#include <QMouseEvent>
#include <grp.h>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

const QString Sudo = "sudo";
const QString SysadmGroup = "sysadm";
const QString SecadmGroup = "secadm";
const QString AudadmGroup = "audadm";
const QList <QString> IgoreGroups= {Sudo, "root", SecadmGroup, AudadmGroup};

DWIDGET_USE_NAMESPACE

UserGroupsPage::UserGroupsPage(User *user, dcc::accounts::UserModel *userModel, ContentWidget *parent)
    : ContentWidget(parent)
    , m_layout (new QVBoxLayout(this))
    , m_curUser(user)
    , m_userModel(userModel)
    , m_groupItemModel(new QStandardItemModel(this))
    , m_listGrp(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_editBtn(new DCommandLinkButton(tr("Edit")))
    , m_addBtn(new DCommandLinkButton(tr("Add UserGroup")))
    , m_isItemEditting(false)
    , m_isAddItem(false)
{
    initWidget();
    initData();
    connect(m_curUser, &User::groupsChanged, this, &UserGroupsPage::changeUserGroup);
    connect(m_curUser, &User::gidChanged, this, &UserGroupsPage::onGidChanged);
    connect(m_userModel, &UserModel::allGroupsChange, this, &UserGroupsPage::setGroupInfo);
}

UserGroupsPage::~UserGroupsPage()
{

}

void UserGroupsPage::changeUserGroup(const QStringList &groups)
{
    QStringList sl = groups;
    QVector<UserGroupsInfoItem*> tmpGroupItem;
    QVector<UserGroupsInfoItem*> tmpItem;
    foreach (auto var, m_vecItem) {
        if (sl.contains(var->getTitle())) {
            var->setInGroup(true);
            tmpGroupItem.append(var);
        } else {
            tmpItem.append(var);
            var->setInGroup(false);
        }
    }

    m_vecItem.clear();
    m_vecItem.append(tmpGroupItem);
    m_vecItem.append(tmpItem);
    m_listGrp->itemCount();
    for (int i = 0; i < m_vecItem.count(); ++i) {
        m_listGrp->moveItem(m_vecItem[i], i);
    }
}

void UserGroupsPage::GroupClicked(UserGroupsInfoItem *item, const QString &groupName)
{
    if (m_isAddItem)
        return;
    if (m_isItemEditting || !item->isEnabled()) {
        m_isItemEditting = false;
        return;
    }
    QStringList curUserGroup;
    int row_count = m_vecItem.count();
    for (int i = 0; i < row_count; ++i) {
        UserGroupsInfoItem *itemGroup = m_vecItem[i];
        if (itemGroup && itemGroup->isInGroup()) {
            curUserGroup << itemGroup->getTitle();
        }
    }
    bool state = item->isInGroup();
    state == true ? (void)curUserGroup.removeOne(groupName) : curUserGroup.append(groupName);
    Q_EMIT requestSetGroups(m_curUser, curUserGroup);
}

void UserGroupsPage::setGroupInfo(const QStringList &group)
{
    for (const QString &item : group) {
        UserGroupsInfoItem *userInfoItem = new UserGroupsInfoItem(this);
        userInfoItem->setTitle(item);
        userInfoItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        DFontSizeManager::instance()->bind(userInfoItem, DFontSizeManager::T6);
        m_listGrp->appendItem(userInfoItem);
        m_vecItem.append(userInfoItem);
        connect(userInfoItem, &UserGroupsInfoItem::itemClicked, this, &UserGroupsPage::GroupClicked);
        connect(userInfoItem, &UserGroupsInfoItem::editClicked, this, &UserGroupsPage::onItemEdit);
        connect(userInfoItem, &UserGroupsInfoItem::editTextFinished, this, &UserGroupsPage::editTextFinished);
        connect(userInfoItem, &UserGroupsInfoItem::removeClicked, this, &UserGroupsPage::removeClicked);
    }
    changeUserGroup(m_curUser->groups());
}

void UserGroupsPage::onGidChanged(const QString &gid)
{
    bool ok;
    int iGid = gid.toInt(&ok, 10);
    if (!ok)
        return;

    const group *group = getgrgid(static_cast<__gid_t>(iGid));
    if (nullptr == group || nullptr == group->gr_name)
        return;

    m_groupName = QString(group->gr_name);
    int managerAccountsCount = getAdministratorAccountsCount();

    for (int i = 0; i < m_vecItem.count(); ++i) {
        auto item = m_vecItem[i];
        if (nullptr == item)
            continue;
        bool value = false;
        do {
            if (item->getTitle() == m_groupName) {
                break;
            }
            if (item->getTitle() == Sudo) {
                if (!(m_curUser && m_curUser->online()) && !(managerAccountsCount == 1 && m_curUser->userType() == User::UserType::Administrator)) {
                    value = true;
                }
            } else {
                value = true;
            }

            // 对于sysadm组，不管有没有加入等保，都需要将其置灰
            if (item->getTitle() == SysadmGroup) {
                value = false;
            }

            // 等保三级系统模式下，置灰sudo, root, secadm, audadm组
            if (m_userModel->getIsSecurityHighLever()) {
                for (auto &group : IgoreGroups) {
                    if (item->getTitle() == group) {
                        value = false;
                    }
                }
            }
        } while (0);
        item->setEnabled(value);
    }
}

void UserGroupsPage::cancelAddGroup()
{
    m_isAddItem = false;
    m_isItemEditting = false;
    m_vecItem.pop_back();
    onGidChanged(m_curUser->gid());
}

void UserGroupsPage::editTextFinished(UserGroupsInfoItem *item, QString group)
{
    m_isItemEditting = false;
    QString oldGroup = item->getTitle();
    QString newGroup = group;
    if (oldGroup == newGroup)
        return;
    if (group.isEmpty()) {
        item->showAlertMessage(tr("please enter group name!"));
        return;
    }
    QStringList userGroup = m_userModel->getAllGroups();
    if (userGroup.contains(group)) {
        item->showAlertMessage(tr("also has this group!"));
        return;
    } else if (QRegExp("\\d*").exactMatch(group)) {     //check the numeric
        item->showAlertMessage(tr("can't set all numeric group name!"));
        return;
    }
    if (oldGroup.isEmpty()) {
        // Create group
        Q_EMIT requestCreateGroup(newGroup, 0, false);
        item->setVisible(false);
        m_listGrp->setFocus();
        return;
    }
    // Nodify group
    Q_EMIT requestModifyGroup(oldGroup,newGroup,0);
    item->setVisible(false);
    m_listGrp->setFocus();
}

void UserGroupsPage::removeClicked(UserGroupsInfoItem *item, QString group)
{
    Q_EMIT requestDeleteGroup(group);
}

void UserGroupsPage::addGroupItem()
{
    if (m_isItemEditting || m_isAddItem || m_editBtn->text() == tr("Done"))
        return;
    foreach (auto var, m_vecItem) {
        var->setEnabled(false);
    }
    UserGroupsInfoItem *userInfoItem = new UserGroupsInfoItem(this);
//    userInfoItem->setTitle(item);
    userInfoItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    DFontSizeManager::instance()->bind(userInfoItem, DFontSizeManager::T6);
    m_listGrp->appendItem(userInfoItem);
    m_vecItem.append(userInfoItem);
    connect(userInfoItem, &UserGroupsInfoItem::itemClicked, this, &UserGroupsPage::GroupClicked);
    connect(userInfoItem, &UserGroupsInfoItem::editClicked, this, &UserGroupsPage::onItemEdit);
    connect(userInfoItem, &UserGroupsInfoItem::cancelAddGroup, this, &UserGroupsPage::cancelAddGroup);
    connect(userInfoItem, &UserGroupsInfoItem::editTextFinished, this, &UserGroupsPage::editTextFinished);
    userInfoItem->setEditTitle(true);
    userInfoItem->setShowIcon(true);
    m_isAddItem = true;
}

void UserGroupsPage::onItemEdit()
{
    m_isItemEditting = true;
}

void UserGroupsPage::onGroupListChanged(const QStringList &value)
{
    m_isItemEditting = false;
    m_isAddItem = false;
    m_editBtn->setText(tr("Edit"));
    initData();
}

void UserGroupsPage::initWidget()
{
    //~ contents_path /accounts/Group
    m_groupTip = new QLabel(tr("Group"));
    this->setAccessibleName("UserGroupsPage");
    DFontSizeManager::instance()->bind(m_groupTip, DFontSizeManager::T5, QFont::DemiBold);

    TranslucentFrame * mainWidget = new TranslucentFrame(this);
    setContentsMargins(0, 10, 0, 10);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setMargin(0);

    m_groupTip->setContentsMargins(10, 10, 10, 10);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_groupTip->setSizePolicy(sizePolicy);
    QHBoxLayout *layoutGroupTip = new QHBoxLayout(this);
    layoutGroupTip->addWidget(m_groupTip);
    layoutGroupTip->addWidget(m_editBtn);
    m_layout->addLayout(layoutGroupTip);

    connect(m_editBtn, &DCommandLinkButton::clicked, this, [this] {
        if (m_isItemEditting) {
            m_isItemEditting = false;
            return;
        }
        if (m_isAddItem)
            return;
        bool isEdit = m_editBtn->text() == tr("Edit");
        foreach (auto var, m_vecItem) {
            var->setEditStatus(isEdit);
        }
        m_editBtn->setText(isEdit ? tr("Done") : tr("Edit"));
    });

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(10, 0, 10, 0);
//    vLayout->addWidget(m_groupListView);
    vLayout->addWidget(m_listGrp);
    vLayout->addWidget(m_addBtn);
    m_layout->addLayout(vLayout);

    mainWidget->setLayout(m_layout);
    setContent(mainWidget);

    connect(m_addBtn, &DCommandLinkButton::clicked, this, &UserGroupsPage::addGroupItem);
}

void UserGroupsPage::initData()
{
    QStringList userGroup = m_userModel->getAllGroups();
    m_listGrp->clear();
    m_vecItem.clear();
    for (QString item : userGroup) {
        GroupItem *it = new GroupItem(item);
        it->setCheckable(false);
        // 退出等保后，之前等保用户的用户组信息并没有被清除, 需要进行对应处理
        // 对于secadm和audadm,隐藏处理
        if (!m_userModel->getIsSecurityHighLever()) {
            if (it->text() == SecadmGroup || it->text() == AudadmGroup) {
                continue;
            }
        }
        UserGroupsInfoItem *userInfoItem = new UserGroupsInfoItem(this);
        userInfoItem->setTitle(item);
        userInfoItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        DFontSizeManager::instance()->bind(userInfoItem, DFontSizeManager::T6);
        m_listGrp->appendItem(userInfoItem);
        m_vecItem.append(userInfoItem);
        connect(userInfoItem, &UserGroupsInfoItem::itemClicked, this, &UserGroupsPage::GroupClicked);
        connect(userInfoItem, &UserGroupsInfoItem::editClicked, this, &UserGroupsPage::onItemEdit);
        connect(userInfoItem, &UserGroupsInfoItem::editTextFinished, this, &UserGroupsPage::editTextFinished);
        connect(userInfoItem, &UserGroupsInfoItem::removeClicked, this, &UserGroupsPage::removeClicked);
    }

    changeUserGroup(m_curUser->groups());
    onGidChanged(m_curUser->gid());
}

int UserGroupsPage::getAdministratorAccountsCount()
{
    if (!m_userModel) {
        return 0;
    }
    int count = 0;
    for (auto data : m_userModel->userList()) {
        if (data->userType() == User::UserType::Administrator) {
            count++;
        }
    }
    return count;
}

UserGroupsInfoItem::UserGroupsInfoItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout)
    , m_title(new QLabel)
    , m_removeBtn(new DIconButton(this))
    , m_checkedBtn(new DIconButton(this))
    , m_editBtn(new DIconButton(this))
    , m_editTitle(new DLineEdit(this))
    , m_itemName("")
    , m_currentpa(DApplicationHelper::instance()->palette(this))
    , m_isInGroup(false)
    , m_editStatus(false)
{
    setFixedHeight(36);

    m_editBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_editBtn->setFlat(true);//设置背景透明
//    m_editBtn->setVisible(false);

    m_editTitle->setClearButtonEnabled(false);
    m_editTitle->setVisible(false);
    m_editTitle->lineEdit()->setFrame(false);
//    m_editTitle->lineEdit()->setValidator(new QRegExpValidator(QRegExp("[\\w\\-]+"),m_editTitle));

    m_removeBtn->setFlat(true);
    m_removeBtn->setIcon(DStyle::StandardPixmap::SP_DeleteButton);
    m_removeBtn->setFixedSize(QSize(24, 24));
    m_removeBtn->setIconSize(QSize(16, 16));
    m_removeBtn->setVisible(false);
    DStyle::setFocusRectVisible(m_removeBtn, false);

    m_checkedBtn->setFlat(true);
    m_checkedBtn->setIcon(QIcon(":/accounts/themes/common/icons/list_select.png"));
    m_checkedBtn->setFixedSize(QSize(24, 24));
    m_checkedBtn->setIconSize(QSize(16, 16));
    m_checkedBtn->setVisible(false);
    DStyle::setFocusRectVisible(m_checkedBtn, false);

    m_layout->setContentsMargins(10, 5, 10, 5);
    m_layout->addWidget(m_title, 0, Qt::AlignLeft);
    m_layout->addWidget(m_editBtn, 0, Qt::AlignLeft);
    m_layout->addWidget(m_editTitle, 0 , Qt::AlignLeft);
    m_editTitle->lineEdit()->setAttribute(Qt::WA_InputMethodEnabled, false);
    m_layout->addStretch();
    m_layout->addWidget(m_removeBtn, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_checkedBtn, 0, Qt::AlignVCenter);
    setLayout(m_layout);

    connect(m_removeBtn, &DIconButton::clicked, this, [this] {
        if (m_editTitle->isVisible()) {
            Q_EMIT cancelAddGroup();
            this->deleteLater();
        }
        else
            Q_EMIT removeClicked(this, m_title->text());
    });
    connect(m_editBtn, &DIconButton::clicked, this, [this] {
        setEditTitle(true);
        Q_EMIT editClicked(m_editTitle->isVisible());
    });
    connect(m_editTitle->lineEdit(), &QLineEdit::textChanged, this, [this] {
        m_editTitle->setAlert(false);
        m_editTitle->hideAlertMessage();
    });
    connect(m_editTitle->lineEdit(), &QLineEdit::editingFinished, this, [this] {
        Q_EMIT editTextFinished(this, m_editTitle->text());
        setEditTitle(false);
        if (!m_editStatus && m_removeBtn->isVisible()) {
            m_title->setVisible(false);
            m_editBtn->setVisible(false);
            m_editTitle->setVisible(true);
        }
    });

    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged,
            this, [=](Dtk::Gui::DGuiApplicationHelper::ColorType themeType) {
                Q_UNUSED(themeType);
                DApplicationHelper::instance()->resetPalette(this);
                m_currentpa = DApplicationHelper::instance()->palette(this);
            });
    installEventFilter(this);
}

UserGroupsInfoItem::~UserGroupsInfoItem()
{
    m_editTitle->lineEdit()->disconnect();
}

void UserGroupsInfoItem::setTitle(const QString &title)
{
    title.isEmpty() ? m_layout->removeWidget(m_title) : m_title->setText(title);
    m_itemName = title;
}

void UserGroupsInfoItem::appendItem(QWidget *widget)
{
    m_layout->addWidget(widget, 0, Qt::AlignLeft);
}

void UserGroupsInfoItem::setShowIcon(bool state)
{
    m_removeBtn->setVisible(state);
}

void UserGroupsInfoItem::setEditTitle(bool state)
{
    m_title->setVisible(!state);
    m_editTitle->setVisible(state);
    if (state) {
        m_editTitle->setVisible(true);
        m_editTitle->lineEdit()->setText(m_title->text());
        m_editTitle->lineEdit()->selectAll();
        m_editTitle->lineEdit()->setFocus();
        Q_EMIT editClicked(true);
    }
    m_editBtn->setVisible(!state);
}

void UserGroupsInfoItem::setHideTitle(bool state)
{
    m_title->setVisible(state);
    m_editTitle->setVisible(state);
}

bool UserGroupsInfoItem::onNameEditFinished()
{
    QString editName = m_editTitle->lineEdit()->text();
    if (editName.isEmpty())
        return false;
    //正则表达式判断是否由字母、数字、中文、下划线组成
//    bool regResult = editName.contains(QRegularExpression("(^[\\w\u4e00-\u9fa5]+$)"));
    bool regResult = editName.contains(QRegularExpression("^[A-Za-z0-9]+$)"));
//    if (editName.size() > 15) {
//        QString errMsg = regResult ? tr("No more than 15 characters") : tr("Use letters, numbers and underscores only, and no more than 15 characters");
//        showAlertMessage(errMsg);
//        return false;
//    } else {
        if (regResult) {
            QString errMsg = tr("Use letters, numbers and underscores only");
            showAlertMessage(errMsg);
            return false;
        }
//    }
    return true;
}

void UserGroupsInfoItem::setEditStatus(bool status)
{
    m_editStatus = status;
    m_removeBtn->setVisible(status);
    m_editBtn->setVisible(!status);
    if (m_isInGroup)
        m_checkedBtn->setVisible(!status);
    else
        m_checkedBtn->setVisible(false);
}

void UserGroupsInfoItem::showAlertMessage(const QString &errMsg)
{
    m_editTitle->setAlert(true);
    m_editTitle->showAlertMessage(errMsg, parentWidget()->parentWidget(), 2000);
}

bool UserGroupsInfoItem::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            if (!m_editStatus)
                Q_EMIT itemClicked(this, m_itemName);
        }
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

void UserGroupsInfoItem::enterEvent(QEvent *event)
{
//    DPalette pa = DApplicationHelper::instance()->palette(this);
//    DStyleHelper styleHelper;
//    styleHelper = DStyleHelper(this->style());

//    QBrush brush;
//    if (styleHelper.dstyle()) {
//        brush = styleHelper.dstyle()->generatedBrush(DStyle::SS_HoverState, pa.itemBackground(), DPalette::Normal, DPalette::ItemBackground);
//    }
//    pa.setBrush(DPalette::Window, Qt::transparent);
//    pa.setBrush(DPalette::ItemBackground, brush);
//    DApplicationHelper::instance()->setPalette(this, pa);

//    if (m_editTitle->isVisible())
//        m_editBtn->hide();
//    else
//        m_editBtn->show();

    QFrame::enterEvent(event);
}

void UserGroupsInfoItem::leaveEvent(QEvent *event)
{
//    DApplicationHelper::instance()->setPalette(this, m_currentpa);
//    m_editBtn->hide();
    QFrame::leaveEvent(event);
}
