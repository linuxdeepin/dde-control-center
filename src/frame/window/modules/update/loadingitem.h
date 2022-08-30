// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
