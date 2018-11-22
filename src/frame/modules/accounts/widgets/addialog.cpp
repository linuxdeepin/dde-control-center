/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "addialog.h"

#include <QImageReader>
#include <QVBoxLayout>
#include <DThemeManager>
#include <QDebug>
#include <DHiDPIHelper>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::accounts;

ADDialog::ADDialog(QWidget *parent)
    : DDialog(parent)
    , m_server(new DLineEdit)
    , m_username(new DLineEdit)
    , m_password(new DPasswordEdit)
    , m_clickType(Cancle)
{
    initUI();
}

void ADDialog::setUserModel(UserModel *model)
{
    m_model = model;

#ifdef DCC_ENABLE_ADDOMAIN
    addButton(model->isJoinADDomain() ? tr("Leave domain") : tr("Join domain"), true);
#endif
}

void ADDialog::initUI()
{
    setIconPixmap(DHiDPIHelper::loadNxPixmap(":/accounts/themes/dark/icons/default_avatar.svg"));

    m_server->setPlaceholderText(tr("Domain Server"));
    m_username->setPlaceholderText(tr("Admin account"));
    m_password->setPlaceholderText(tr("Password"));

    m_server->setClearButtonEnabled(true);
    m_username->setClearButtonEnabled(true);

    addContent(m_server);
    addContent(m_username);
    addContent(m_password);

    addButton(tr("Cancel"));

    setSpacing(10);

    connect(this, &ADDialog::buttonClicked, this, [=] (int index, const QString &text) {
        Q_UNUSED(text);

        m_clickType = (ButtonType)index;
    });
}

void ADDialog::setVisible(bool visible)
{
    if (m_clickType == Cancle) {
        DDialog::setVisible(visible);
        return;
    }

    const bool serverIsEmpty = m_server->text().isEmpty();
    const bool usernameIsEmpty = m_username->text().isEmpty();
    const bool passwordIsEmpty = m_password->text().isEmpty();

    m_server->setAlert(serverIsEmpty);
    m_username->setAlert(usernameIsEmpty);
    m_password->setAlert(passwordIsEmpty);

    if (serverIsEmpty || usernameIsEmpty || passwordIsEmpty) {
        return;
    }

    DDialog::setVisible(visible);

    if (m_clickType == Submit) {
        m_clickType = Cancle;
        Q_EMIT requestInfos(m_server->text(), m_username->text(), m_password->text());
    }
}
