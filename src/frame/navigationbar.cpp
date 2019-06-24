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

#include "navigationbar.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QSignalMapper>
#include <QCoreApplication>

static const QStringList ModuleList = {
#ifndef DISABLE_ACCOUNT
    "accounts",
#endif
#ifndef DISABLE_CLOUD_SYNC
    "cloudsync",
#endif
#ifndef DISABLE_DISPALY
    "display",
#endif
#ifndef DISABLE_DEFAULT_APPLICATIONS
    "defapp",
#endif
#ifndef DISABLE_PERSONALIZATION
    "personalization",
#endif
#ifndef DISABLE_NETWORK
    "network",
#endif
#ifndef DISABLE_BLUETOOTH
    "bluetooth",
#endif
#ifndef DISABLE_SOUND
    "sound",
#endif
#ifndef DISABLE_DATETIME
    "datetime",
#endif
#ifndef DISABLE_POWER
    "power",
#endif
#ifndef DISABLE_MOUSE
    "mouse",
#endif
#ifndef DISABLE_KAYBOARD
    "keyboard",
#endif
#ifndef DISABLE_WACOM
    "wacom",
#endif
#ifndef DISABLE_SYS_UPDATE
    "update",
#endif
#ifndef DISABLE_SYSINFO
    "systeminfo",
#endif
};

NavigationBar::NavigationBar(QWidget *parent)
    : QWidget(parent)
    , m_arrowRectangle(new DArrowRectangle(DArrowRectangle::ArrowRight))
    , m_navLabel(new QLabel)
{
    m_arrowRectangle->setWindowFlags(m_arrowRectangle->windowFlags() | Qt::WindowDoesNotAcceptFocus);
    m_arrowRectangle->setContent(m_navLabel);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(20);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addLayout(buttonsLayout);
    centralLayout->addStretch();
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(20);

    setLayout(centralLayout);
//    setStyleSheet("background-color: cyan;");

    // init buttons
    for (const auto &module : ModuleList)
    {
        DImageButton *b = new DImageButton;
        b->setCheckedPic(QString(":/%1/themes/dark/icons/nav_%1.svg").arg(module));
        b->setNormalPic(QString(":/%1/themes/dark/icons/nav_%1_normal.svg").arg(module));
        b->setCheckable(true);
        b->setVisible(true);
        b->installEventFilter(this);

        buttonsLayout->addWidget(b);
        m_navigationButtons.insert(module, b);

        qDebug() << module;

        connect(b, &DImageButton::clicked, this, &NavigationBar::onNavigationButtonClicked);
    }

    // init first button to checked
    m_navigationButtons[ModuleList.first()]->clicked();

    setModuleVisible("bluetooth", false);
    setModuleVisible("wacom", false);
}

QString NavigationBar::currentModuleName() const
{
    if (!m_checkedButton.isNull())
        return m_navigationButtons.key(m_checkedButton.data());

    return QString();
}

void NavigationBar::setModuleVisible(const QString &module, bool visible)
{
    if (m_navigationButtons.contains(module))
        m_navigationButtons[module]->setVisible(visible);
}

void NavigationBar::setModuleChecked(const QString &module)
{
    DImageButton* b = m_navigationButtons[module];

    Q_ASSERT(b);

    setModuleChecked(b);
}

bool NavigationBar::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Enter) {
        DImageButton *btn = static_cast<DImageButton*>(watched);
        const QPoint p { mapToGlobal(btn->geometry().topLeft()) };
        const QString str { transModuleName(m_navigationButtons.key(btn)) };
        const int width { fontMetrics().width(str) };
        m_navLabel->setText(str);
        m_navLabel->setFixedWidth(width);
        m_arrowRectangle->setWidth(width);
        m_arrowRectangle->show(p.x(), p.y() + btn->height() / 2);
    }

    if (event->type() == QEvent::Leave || event->type() == QEvent::Hide) {
        m_arrowRectangle->hide();
    }

    return QWidget::eventFilter(watched, event);
}

void NavigationBar::onNavigationButtonClicked()
{
    DImageButton *b = static_cast<DImageButton *>(sender());
    Q_ASSERT(b);

    setModuleChecked(b);

    Q_EMIT requestModule(m_navigationButtons.key(b));
}

void NavigationBar::setModuleChecked(DImageButton *button)
{
    if (m_checkedButton == button)
        return button->setChecked(true);

    if (!m_checkedButton.isNull())
        m_checkedButton->setChecked(false);

    m_checkedButton = button;
    m_checkedButton->setChecked(true);
}

QString NavigationBar::transModuleName(const QString &moduleName) const
{
  const QStringList MODULES = {
      "accounts", "cloudsync", "display", "defapp",   "personalization",
      "network",  "bluetooth", "sound",   "datetime", "power",
      "mouse",    "keyboard",  "wacom",   "update",   "systeminfo"};

  static const QStringList modules_trans = {
      QT_TRANSLATE_NOOP("dcc::accounts::AccountsWidget", "Accounts"),
      QT_TRANSLATE_NOOP("dcc::sync::SyncWidget", "Cloud Sync"),
      QT_TRANSLATE_NOOP("dcc::display::DisplayWidget", "Display"),
      QT_TRANSLATE_NOOP("dcc::defapp::DefAppViewer", "Default Applications"),
      QT_TRANSLATE_NOOP("dcc::personalization::PersonalizationWidget",
                        "Personalization"),
      QT_TRANSLATE_NOOP("dcc::network::NetworkModuleWidget", "Network"),
      QT_TRANSLATE_NOOP("dcc::bluetooth::BluetoothModule", "Bluetooth"),
      QT_TRANSLATE_NOOP("dcc::sound::SoundWidget", "Sound"),
      QT_TRANSLATE_NOOP("dcc::datetime::Datetime", "Time and Date"),
      QT_TRANSLATE_NOOP("dcc::power::PowerWidget", "Power Management"),
      QT_TRANSLATE_NOOP("dcc::mouse::MouseWidget", "Mouse"),
      QT_TRANSLATE_NOOP("dcc::keyboard::KeyboardWidget",
                        "Keyboard and Language"),
      QT_TRANSLATE_NOOP("dcc::wacom::WacomWidget", "Wacom"),
      QT_TRANSLATE_NOOP("dcc::update::UpdateCtrlWidget", "Update"),
      QT_TRANSLATE_NOOP("dcc::systeminfo::SystemInfoWidget",
                        "System Information"),
  };

  static const QStringList modules_scope = {
      "dcc::accounts::AccountsWidget",
      "dcc::sync::SyncWidget",
      "dcc::display::DisplayWidget",
      "dcc::defapp::DefAppViewer",
      "dcc::personalization::PersonalizationWidget",
      "dcc::network::NetworkModuleWidget",
      "dcc::bluetooth::BluetoothModule",
      "dcc::sound::SoundWidget",
      "dcc::datetime::Datetime",
      "dcc::power::PowerWidget",
      "dcc::mouse::MouseWidget",
      "dcc::keyboard::KeyboardWidget",
      "dcc::wacom::WacomWidget",
      "dcc::update::UpdateCtrlWidget",
      "dcc::systeminfo::SystemInfoWidget",
  };

  const int idx = moduleName.isEmpty() ? -1 : MODULES.indexOf(moduleName);

  if (idx == -1)
    return tr("Navigation");
  else
    return QCoreApplication::translate(
        modules_scope[idx].toStdString().c_str(),
        modules_trans[idx].toStdString().c_str());
}
