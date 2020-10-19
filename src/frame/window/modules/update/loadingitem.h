/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "widgets/settingsitem.h"

#include <QProgressBar>
#include <QLabel>

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

namespace DCC_NAMESPACE {
namespace update {

class LoadingItem : public QWidget
{
    Q_OBJECT
public:
    explicit LoadingItem(QFrame *parent = 0);
    void setProgressValue(int value);
    void setProgressBarVisible(bool visible);
    void setMessage(const QString &message);
    void setVersionVisible(bool state);
    void setSystemVersion(const QString &version);
    void setImageVisible(bool state);
    void setImageOrTextVisible(bool state);
    void setImageAndTextVisible(bool state);

private:
    dcc::widgets::NormalLabel *m_messageLabel;
    QProgressBar *m_progress;
    QLabel *m_labelImage;
    QLabel *m_labelText;
};

} // namespace update
} // namespace DCC_NAMESPACE
