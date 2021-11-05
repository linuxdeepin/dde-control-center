#ifndef IRISWIDGET_H
#define IRISWIDGET_H

#pragma once

#include "interface/namespace.h"
#include "authenticationinfoitem.h"
#include "widgets/settingsgroup.h"

#include <DCommandLinkButton>

#include <QVector>
#include <QWidget>

namespace dcc {
namespace authentication {
class CharaMangerModel;
}
}

namespace DCC_NAMESPACE {
namespace authentication {

class IrisWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IrisWidget(dcc::authentication::CharaMangerModel *model, QWidget *parent = nullptr);
    ~IrisWidget();

private:
    void initUI();
    void initConnect();

private Q_SLOTS:
    void addIrisButton(const QString &newIrisName);

Q_SIGNALS:
    void requestAddIris(const QString &driverName, const int &charaType, const QString &charaName);
    void requestDeleteIrisItem(const int &charaType, const QString &charaName);
    void requestRenameIrisItem(const int &charaType, const QString &oldName, const QString &newName);
    void noticeEnrollCompleted(const QString &driverName, const int &CharaType);

public Q_SLOTS:
    void onIrisListChanged(const QStringList &irislist);

private:
    dcc::authentication::CharaMangerModel *m_model;
    dcc::widgets::SettingsGroup *m_listGrp;
    DCommandLinkButton *m_clearBtn;
    QVector<AuthenticationInfoItem*> m_vecItem;
};

}
}

#endif // IRISWIDGET_H
