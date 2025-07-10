// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "accountscontroller.h"
#include "pwqualitymanager.h"
#include "avatarlistmodel.h"
#include "dccfactory.h"
#include "accountlistmodel.h"

#include <QFileInfo>
#include <QPainter>
#include <QUrl>
#include <QVariantMap>
#include <QRegularExpression>
#include <QPainterPath>
#include <QBuffer>
#include <QTimer>

#include <DSysInfo>

#include <grp.h>

#define FACEID_NUM    5
#define Faceimg_SIZE 200

DCORE_USE_NAMESPACE
namespace dccV25 {

DCC_FACTORY_CLASS(AccountsController)

static bool isUserGroupName(int gid, const QString &name)
{
    QString groupName;
    const group *curGroup = getgrgid(static_cast<__gid_t>(gid));
    if (nullptr != curGroup && nullptr != curGroup->gr_name)
        groupName = QString(curGroup->gr_name);

    return groupName == name;
}

AccountsController::AccountsController(QObject *parent)
    : QObject{ parent }
{
    qmlRegisterType<CreationResult>("AccountsController", 1, 0, "CreationResult");
    
    m_model = new UserModel(this);
    m_worker = new AccountsWorker(m_model, this);

    connect(m_model, &UserModel::userAdded, this, [this]() {
        Q_EMIT userIdListChanged();
    });
    connect(m_model, &UserModel::userRemoved, this, [this](User *user) {
        Q_EMIT userRemoved(user->id());
        Q_EMIT userIdListChanged();
    });

    connect(m_model, &UserModel::avatarChanged, this, &AccountsController::avatarChanged);
    connect(m_model, &UserModel::autoLoginChanged, this, &AccountsController::autoLoginChanged);
    connect(m_model, &UserModel::quickLoginChanged, this, &AccountsController::quickLoginChanged);
    connect(m_model, &UserModel::nopasswdLoginChanged, this, &AccountsController::nopasswdLoginChanged);
    connect(m_model, &UserModel::groupsChanged, this, [this](const QString &userId, const QStringList &groups) {
        updateSingleUserGroups(userId);
        Q_EMIT groupsChanged(userId, groups);
    });
    connect(m_model, &UserModel::passwordModifyFinished, this, &AccountsController::passwordModifyFinished);
    connect(m_model, &UserModel::userTypeChanged, this, &AccountsController::userTypeChanged);
    connect(m_model, &UserModel::fullnameChanged, this, &AccountsController::fullnameChanged);
    connect(m_model, &UserModel::passwordAgeChanged, this, &AccountsController::passwordAgeChanged);

    connect(m_worker, &AccountsWorker::showSafetyPage, this, &AccountsController::showSafetyPage);
    connect(m_model, &UserModel::allGroupsChange, this, [this]() {
        if (!m_isCreatingUser) {
            updateAllGroups();
        }
        this->groupsUpdate();
    });
    connect(m_worker, &AccountsWorker::updateGroupFailed, this, &AccountsController::groupsUpdateFailed);
    connect(m_worker, &AccountsWorker::createGroupFailed, this, [this]() {
        requestClearEmptyGroup(currentUserId());
    });
    connect(m_worker, &AccountsWorker::updateGroupFinished, this, [this]() {
        updateAllGroups();
        this->groupsUpdate();
    });
    
    connect(m_worker, &AccountsWorker::accountCreationFinished, this, [this](CreationResult *result) {
        m_isCreatingUser = false;
        Q_EMIT accountCreationFinished(result->type(), result->message());
        
        if (result->type() == CreationResult::NoError) {
            QTimer::singleShot(100, this, [this]() {
                if (!m_model->userList().isEmpty()) {
                    updateSingleUserGroups(m_model->userList().last()->id());
                }
            });
        }
    });

    connect(m_model, &UserModel::quickLoginVisibleChanged, this, &AccountsController::quickLoginVisibleChanged);

    QMetaObject::invokeMethod(m_worker, "active", Qt::QueuedConnection);
}

AccountsController::~AccountsController() { }

QString AccountsController::avatar(const QString &id) const
{
    User *user = m_model->getUser(id);
    return user ? user->currentAvatar() : QString();
}

void AccountsController::setAvatar(const QString &id, const QString &url)
{
    if (User *user = m_model->getUser(id))
        m_worker->setAvatar(user, QUrl::fromUserInput(url).toLocalFile());
}

QStringList AccountsController::avatars(const QString &id, const QString &filter, const QString &section)
{
    User *user = m_model->getUser(id);
    if (!user)
        return {};

    QStringList res;
    const auto &list = user->avatars();
    std::copy_if(list.begin(), list.end(), std::back_inserter(res),
                 [filter, section](const QString &iconFile) {
        bool exists = QFileInfo::exists(QUrl(iconFile).toLocalFile());
        const QString subPath = section.isEmpty() ? "" : section + "/";
        return exists && iconFile.contains(filter + "/" + subPath);
    });

    std::sort(res.begin(), res.end(), [=](const QString &path1, const QString &path2){
        return path1 < path2;
    });

    if (filter.contains("icons/local")) {
        res.prepend("add"); // add button...
    }

    return res;
}

QString AccountsController::userName(const QString &id) const
{
    User *user = m_model->getUser(id);
    return user ? user->name() : QString();
}

QString AccountsController::fullName(const QString &id) const
{
    User *user = m_model->getUser(id);
    return user ? user->fullname() : QString();
}

void AccountsController::setFullname(const QString &id, const QString &name)
{
    if (User *user = m_model->getUser(id))
        if (name.simplified() != user->fullname())
            m_worker->setFullname(user, name.simplified()); // 去除空白字符
}

int AccountsController::userType(const QString &id) const
{
    User *user = m_model->getUser(id);
    return user ? user->userType() : 0;
}

void AccountsController::setUserType(const QString &id, int index)
{
    if (User *user = m_model->getUser(id))
        if (index != user->userType())
            m_worker->setAdministrator(user, User::Administrator == index);
}

QString AccountsController::userTypeName(const QString &id) const
{
    auto ut = userTypes();
    return ut.value(userType(id));
}

bool AccountsController::isSystemAdmin(const User *user) const
{
    if (!user)
        return false;

    // 本地管理员账户不一定是等保三级的管理员账户，要区分判断
    if (m_model->getIsSecurityHighLever())
        return user->securityLever() == SecurityLever::Sysadm;

    return user->userType() == User::UserType::Administrator;
}

int AccountsController::adminCount() const
{
    int cnt = 0;
    // 等保三级系统模式下
    if (m_model->getIsSecurityHighLever())
        return 1;

    for (auto user : m_model->userList()) {
        if (user->userType() == User::UserType::Administrator)
            cnt++;
    }
    return cnt;
}

void AccountsController::checkPwdLimitLevel(int lvl)
{
    // 企业版控制中心用户创建屏蔽安全中心登录安全的接口需求
    if ((DSysInfo::uosEditionType() == DSysInfo::UosEnterprise) ||
        (DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC))
        return ;

    // needShowSafetyPage
    m_worker->checkPwdLimitLevel(lvl);
}

bool AccountsController::isDeleteAble(const QString &id) const
{
    User *editUser = m_model->getUser(id);
    if (!editUser)
        return false;

    User *curLoginUser = m_model->currentUser();
    auto isOnlyAdmin = [this, editUser] {         // 是最后一个管理员
        return isSystemAdmin(editUser)            // 是管理员
                && adminCount() == 1;             // 管理员只有一个
    };

    if (m_model->getIsSecurityHighLever()) {
        return curLoginUser && curLoginUser->securityLever() == SecurityLever::Sysadm
                && !editUser->isCurrentUser();
    }
    return !editUser->isCurrentUser() // 不是当前用户
            && !editUser->online()    // 未登录
            && !isOnlyAdmin();        // 不是最后一个管理员
}

bool AccountsController::isAutoLoginVisable() const
{
    return m_model->isAutoLoginVisable();
}

bool AccountsController::isQuickLoginVisible() const
{
    return m_model->isQuickLoginVisible();
}

bool AccountsController::autoLogin(const QString &id) const
{
    User *user = m_model->getUser(id);
    return user ? user->autoLogin() : false;
}

bool AccountsController::quickLogin(const QString &id) const
{
    User *user = m_model->getUser(id);
    return user ? user->quickLogin() : false;
}

void AccountsController::setAutoLogin(const QString &id, const bool enable)
{
    if (User *user = m_model->getUser(id))
        if (user->autoLogin() != enable)
            m_worker->setAutoLogin(user, enable);
}

void AccountsController::setQuickLogin(const QString &id, const bool enable)
{
    if (User *user = m_model->getUser(id))
        if (user->quickLogin() != enable)
            m_worker->setQuickLogin(user, enable);
}

QString AccountsController::getOtherUserAutoLogin() const
{
    for (auto user : m_model->userList()) {
        if (user->name() != currentUserName() && user->autoLogin()) {
            return user->name();
        }
    }
    return "";
}

bool AccountsController::isNoPassWordLoginVisable() const
{
    return m_model->isNoPassWordLoginVisable();
}

bool AccountsController::nopasswdLogin(const QString &id)
{
    User *user = m_model->getUser(id);
    return user ? user->nopasswdLogin() : false;
}

void AccountsController::setNopasswdLogin(const QString &id, const bool enable)
{
    if (User *user = m_model->getUser(id))
        if (user->nopasswdLogin() != enable)
            m_worker->setNopasswdLogin(user, enable);
}

bool AccountsController::isOnline(const QString &id)
{
    User *user = m_model->getUser(id);
    return user ? user->online() : false;
}

bool AccountsController::needShowGroups()
{
#ifdef QT_DEBUG
    // alwayls show groups for test
    return true;
#endif

    return !DSysInfo::isCommunityEdition();
}

QStringList AccountsController::allGroups() const
{
    return m_model->getAllGroups();
}

QStringList AccountsController::groups(const QString &id)
{
    if (!needShowGroups())
        return QStringList();

    if (m_groups.contains(id))
        return m_groups.value(id);

    updateGroups(id);

    return m_groups.value(id);
}

void AccountsController::updateGroups(const QString &id)
{
    QStringList ag = allGroups();
    
    QHash<QString, bool> containsCache;
    for (const QString &group : ag) {
        containsCache[group] = groupContains(id, group);
    }
    
    std::sort(ag.begin(), ag.end(), [&containsCache](const QString &left, const QString &right){
        bool isLeftContains = containsCache.value(left);
        bool isRightContains = containsCache.value(right);

        if (!(isLeftContains ^ isRightContains)) { // both true or both false
            return left.compare(right, Qt::CaseInsensitive) < 0;
        } else {
            return isLeftContains ? true : false;
        }
    });
    m_groups[id] = ag;
}

void AccountsController::updateAllGroups()
{
    if (!m_model)
        return;
    
    QSet<QString> existingUserIds;
    QList<User *> userList = m_model->userList();
    
    QStringList allGroupsList = allGroups();
    
    for (const auto user : userList) {
        QString id = user->id();
        existingUserIds.insert(id);
        
        QHash<QString, bool> containsCache;
        for (const QString &group : allGroupsList) {
            containsCache[group] = groupContains(id, group);
        }
        
        QStringList containedGroups;
        QStringList notContainedGroups;
        
        for (const QString &group : allGroupsList) {
            if (containsCache.value(group)) {
                containedGroups.append(group);
            } else {
                notContainedGroups.append(group);
            }
        }
        
        std::sort(containedGroups.begin(), containedGroups.end(), [](const QString &a, const QString &b) {
            return a.compare(b, Qt::CaseInsensitive) < 0;
        });
        
        std::sort(notContainedGroups.begin(), notContainedGroups.end(), [](const QString &a, const QString &b) {
            return a.compare(b, Qt::CaseInsensitive) < 0;
        });
        
        QStringList sortedGroups = containedGroups;
        sortedGroups.append(notContainedGroups);
        
        m_groups[id] = sortedGroups;
    }
    
    QStringList keysToRemove;
    for (const auto &id : m_groups.keys()) {
        if (!existingUserIds.contains(id)) {
            keysToRemove.append(id);
        }
    }
    
    for (const auto &id : keysToRemove) {
        m_groups.remove(id);
    }
}

void AccountsController::updateSingleUserGroups(const QString &id)
{
    if (!m_model || !m_model->getUser(id))
        return;
        
    updateGroups(id);
}

bool AccountsController::groupContains(const QString &id, const QString &name) const
{
    User *user = m_model->getUser(id);
    if (!user)
        return false;

    if (isUserGroupName(user->gid().toInt(), name))
        return true;

    auto presetGroups = user->groups();
    return presetGroups.contains(name);
}

bool AccountsController::groupEnabled(const QString &id, const QString &name) const
{
    User *user = m_model->getUser(id);
    if (!user)
        return false;

    const QString Sudo = "sudo";
    const QString SysadmGroup = "sysadm";
    const QString SecadmGroup = "secadm";
    const QString AudadmGroup = "audadm";
    const QList <QString> IgoreGroups= {Sudo, "root", SecadmGroup, AudadmGroup};

    if (isUserGroupName(user->gid().toInt(), name))
        return false;

    // 对于sysadm组，不管有没有加入等保，都需要将其置灰
    if (name == SysadmGroup) {
        return false;
    }
    // 等保三级系统模式下，置灰sudo, root, secadm, audadm组
    if (m_model->getIsSecurityHighLever()) {
        for (auto &group : IgoreGroups) {
            if (name == group)
                return false;
        }
    }

    bool ret = (name != Sudo) ? true : false;
    bool isLastAdmin = adminCount() == 1 && user->userType() == User::Administrator;
    if (!ret && !user->online() && !isLastAdmin) {
        ret = true;
    }

    return ret;
}

bool AccountsController::groupEditAble(const QString &id, const QString &name) const
{
    User *user = m_model->getUser(id);
    if (!user) {
        return false;
    }

    bool ok;
    int iGid = user->gid().toInt(&ok, 10);
    if (!ok)
      return false;

    if (isUserGroupName(iGid, name))
        return false;

    // gid < 1000
    if (m_model->isDisabledGroup(name))
        return false;

    return true;
}

bool AccountsController::groupExists(const QString &name) const
{
    QStringList existingGroups = allGroups();
    return existingGroups.contains(name);
}

void AccountsController::createGroup(const QString &name)
{
    m_worker->createGroup(name, 0, false);
}

void AccountsController::deleteGroup(const QString &name)
{
    m_worker->deleteGroup(name);
}

void AccountsController::modifyGroup(const QString &oldName, const QString &newName)
{
    m_worker->modifyGroup(oldName, newName, 0);
}

void AccountsController::setGroup(const QString &id, const QString &group, bool on)
{
    if (User *user = m_model->getUser(id)) {
        QStringList groups = user->groups();

        if (on && !groups.contains(group)) {
            groups.append(group);
        } else if (!on && groups.contains(group)) {
            groups.removeOne(group);
        } else {
            qWarning() << "setGroups" << user->name() << groups << group << on;
            return;
        }

        m_worker->setGroups(user, groups);
    }
}

void AccountsController::addUser(const QVariantMap &info)
{
    m_isCreatingUser = true;
    
    const int type = info["type"].toInt();
    const QString name = info["name"].toString().simplified();
    const QString fullname = info["fullname"].toString().simplified();
    const QString pwd = info["pwd"].toString();
    const QString pwdRepeat = info["pwdRepeat"].toString();
    const QString pwdHint = info["pwdHint"].toString();

    User *user = new User(this);
    m_worker->randomUserIcon(user);
    user->setName(name.simplified());
    user->setFullname(fullname.simplified());
    user->setPassword(pwd);
    user->setRepeatPassword(pwdRepeat);
    user->setPasswordHint(pwdHint);

    /* 设置用户组 */
    if (type == 1) {
        user->setUserType(User::UserType::Administrator);
    } else if (type == 0) {
        user->setUserType(User::UserType::StandardUser);
    } else {
        Q_ASSERT_X(0, __FUNCTION__, "invalide userType " + type);
        // user->setGroups("custom");
        // user->setUserType(User::UserType::StandardUser);
    }

    m_worker->createAccount(user);
}

void AccountsController::removeUser(const QString &id, const bool deleteHome)
{
    if (User *user = m_model->getUser(id))
        m_worker->deleteAccount(user, deleteHome);
}

void AccountsController::setPassword(const QString &id, const QVariantMap &info)
{
    if (User *user = m_model->getUser(id)) {
        const QString oldPwd = info["oldPwd"].toString();
        const QString newPwd = info["pwd"].toString();
        const QString pwdRepeat = info["pwdRepeat"].toString();

        if (user->isCurrentUser())
            m_worker->setPassword(user, oldPwd, newPwd, pwdRepeat);
        else
            m_worker->resetPassword(user, newPwd);
    }
}

void AccountsController::setPasswordHint(const QString &id, const QString &pwdHint)
{
    if (User *user = m_model->getUser(id)) {
        if (!pwdHint.simplified().isEmpty())
            m_worker->setPasswordHint(user, pwdHint);
    }
}

int AccountsController::passwordAge(const QString &id) const
{
    User *user = m_model->getUser(id);
    qDebug() << "passwordAge" << user->passwordAge();
    return user ? user->passwordAge() : -1;
}

void AccountsController::setPasswordAge(const QString &id, const int age)
{
    if (User *user = m_model->getUser(id)) {
        if (user->passwordAge() != age)
            m_worker->setMaxPasswordAge(user, age);
    }
}

QSortFilterProxyModel *AccountsController::avatarFilterModel()
{
    if (m_avatarFilterModel)
        return m_avatarFilterModel;

    m_avatarFilterModel = new QSortFilterProxyModel(this);
    User *user = m_model->currentUser();
    AvatarListModel *sourceModel = new AvatarListModel(user, this);
    m_avatarFilterModel->setSourceModel(sourceModel);

    m_avatarFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    return m_avatarFilterModel;
}

QAbstractListModel *AccountsController::avatarTypesModel()
{
    if (m_avatarTypesModel)
        return m_avatarTypesModel;

    m_avatarTypesModel = new AvatarTypesModel(this);
    return m_avatarTypesModel;
}

QAbstractListModel *AccountsController::accountsModel()
{
    if (m_accountsModel)
        return m_accountsModel;

    m_accountsModel = new AccountListModel(this);
    connect(this, &AccountsController::avatarChanged, m_accountsModel, [this](const QString &userId, const QString &) {
        int idIdx = userIdList().indexOf(userId);
        if (idIdx < 0)
            return;
        auto index = m_accountsModel->index(idIdx);
        m_accountsModel->dataChanged(index, index, {AccountListModel::AvatarRole});
    });
    connect(this, &AccountsController::groupsChanged, m_accountsModel, [this](const QString &userId, const QStringList &) {
        int idIdx = userIdList().indexOf(userId);
        if (idIdx < 0)
            return;
        auto index = m_accountsModel->index(idIdx);
        m_accountsModel->dataChanged(index, index, {AccountListModel::UserTypeRole});
    });
    connect(m_model, &UserModel::onlineChanged, m_accountsModel, [this](const QString &userId, const bool&) {
        int idIdx = userIdList().indexOf(userId);
        if (idIdx < 0)
            return;
        auto index = m_accountsModel->index(idIdx);
        m_accountsModel->dataChanged(index, index, {AccountListModel::OnlineRole});
        emit onlineUserListChanged();
    });

    connect(this, &AccountsController::userIdListChanged, static_cast<AccountListModel *>(m_accountsModel), &AccountListModel::reset);

    return m_accountsModel;
}

QAbstractListModel *AccountsController::groupsModel(const QString &id)
{
    if (m_groupsModel) {
        static_cast<GroupListModel*>(m_groupsModel)->setUserId(id);
        return m_groupsModel;
    }

    auto groupsModel = new GroupListModel(id, this);
    m_groupsModel = groupsModel;
    return m_groupsModel;
}

int AccountsController::passwordLevel(const QString &pwd)
{
    return PwqualityManager::instance()->GetNewPassWdLevel(pwd);
}

QString AccountsController::checkUsername(const QString &name)
{
    QString alertMsg;
    do {
        if (name.size() < 3 || name.size() > 32) {
            alertMsg = tr("Username must be between 3 and 32 characters");
            break;
        }

        QRegularExpression letterOrNum("^[A-Za-z0-9]+");
        if (!letterOrNum.match(name).hasMatch()) {
            alertMsg = tr("The first character must be a letter or number");
            break;
        }

        QRegularExpression onlyNums("^\\d+$");
        if (onlyNums.match(name).hasMatch()) {
            alertMsg = tr("Your username should not only have numbers");
            break;
        }
#define ErrCodeExist 4
       // username existed check, not check fullname!
       // reply ==> (false, "the username existed", 4)
        QDBusPendingReply<bool, QString, int> reply = m_worker->isUsernameValid(name);
        if (!reply.argumentAt(0).toBool() && ErrCodeExist == reply.argumentAt(2).toInt()) {
            alertMsg = tr("The username has been used by other user accounts");
            break;
        }

        // check fullname
        QList<User *> userList = m_model->userList();
        auto ret = std::any_of(userList.begin(), userList.end(), [name](const User * user) {
            return name == user->fullname();
        });

        if (ret) {
            alertMsg = tr("The username has been used by other user accounts");
            break;
        }

    } while (false);

    return alertMsg;
}

QString AccountsController::checkFullname(const QString &name)
{
    QString alertMsg;
    do {
        if (name.simplified().isEmpty()) {
            break;
        }

        if (name.size() > 32) {
            alertMsg = tr("The full name is too long");
            break;
        }
#define ErrCodeSystemUsed 6
        // 欧拉版会自己创建 shutdown 等 root 组账户且不会添加到 userList 中，导致无法重复性算法无效，
        // 先通过 isUsernameValid 校验这些账户再通过重复性算法校验
        // vaild == false && code == 6 是用户名已存在
        QDBusPendingReply<bool, QString, int> reply = m_worker->isUsernameValid(name);
        if (!reply.argumentAt(0).toBool() &&
            ErrCodeSystemUsed == reply.argumentAt(2).toInt()) {
            alertMsg = tr("The full name has been used by other user accounts");
            break;
        }

        QList<User *> userList = m_model->userList();
        auto ret = std::any_of(userList.begin(), userList.end(), [name](User *user) {
            return name == user->fullname() || name == user->name();
        });
        /* 与已有的用户全名和用户名进行重复性校验 */
        if (ret) {
            alertMsg = tr("The full name has been used by other user accounts");
            break;
        }

        QList<QString> groupList = m_model->getAllGroups();
        ret = std::any_of(groupList.begin(), groupList.end(), [name](const QString &group) {
            return name == group;
        });
        if (ret) {
            alertMsg = tr("The full name has been used by other user accounts");
            break;
        }
    } while (false);

    return alertMsg;
}

QString AccountsController::checkPassword(const QString &name, const QString &pwd)
{
    auto error = PwqualityManager::instance()->verifyPassword(name, pwd);
    if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
        QString msg = PwqualityManager::instance()->getErrorTips(error);

        checkPwdLimitLevel(passwordLevel(pwd));
        return msg;
    }

    return QString();
}

QVariantMap AccountsController::checkPasswordResult(int code, const QString &msg, const QString &name, const QString &pwd)
{
    if (code == 0)
        return QVariantMap();

    QVariantMap res;
    // reset password error
    if (code < 0) {
        res["pwd"] = QVariant::fromValue(msg);
        return res;
    }

    auto error = PwqualityManager::instance()->verifyPassword(name, pwd);
    QString errMsg =  PwqualityManager::instance()->getErrorTips(error);

    // new password error
    if (error != PW_NO_ERR) {
        res["pwd"] = QVariant::fromValue(errMsg);
        return res;
    }

    // current password error
    if (msg.startsWith("Current Password: passwd:", Qt::CaseInsensitive)) {
        res["oldPwd"] = QVariant::fromValue(tr("Wrong password"));
        return res;
    }

    checkPwdLimitLevel(passwordLevel(pwd));

    // new password err
    res["pwd"] = QVariant::fromValue(errMsg);
    return res;
}

void AccountsController::showDefender()
{
    m_worker->showDefender();
}

void AccountsController::playSystemSound(int soundType)
{
    m_worker->playSystemSound(soundType);
}

QString AccountsController::currentUserName() const
{
    return m_model->getCurrentUserName();
}

QStringList AccountsController::userIdList() const
{
    QStringList ids;
    for (const auto user : m_model->userList()) {
        if (user->name() == currentUserName())
            continue;

        ids << user->id();
    }
    return ids;
}

QStringList AccountsController::onlineUserList() const
{
    return m_model->getOnlineUsers();
}

QString AccountsController::currentUserId() const
{
    for (const auto user : m_model->userList()) {
        if (user->name() == m_model->getCurrentUserName())
            return user->id();
    }
    return QString();
}

bool AccountsController::curUserIsSysAdmin() const
{
    User *curUser = m_model->currentUser();
    if (curUser)
        return isSystemAdmin(curUser);

    return false;
}

QStringList AccountsController::userTypes(bool createUser /* = false*/) const
{
    QStringList types{ tr("Standard User"), tr("Administrator") };
    if (createUser && DSysInfo::UosServer == DSysInfo::uosType()) {
        types << tr("Customized");
    }
    return types;
}

} // namespace dccV25

#include "accountscontroller.moc"
