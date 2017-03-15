#ifndef CUSTOMCONFIGPAGE_H
#define CUSTOMCONFIGPAGE_H

#include "contentwidget.h"

#include <QPushButton>
#include <QLabel>

namespace dcc {

namespace display {

class CustomConfigPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit CustomConfigPage(const QString &config, QWidget *parent = 0);

signals:
    void requestModifyConfig(const QString &config) const;
    void requestDeleteConfig(const QString &config) const;

public slots:
    void onCurrentConfigChanged(const bool custom, const QString config);

private slots:
    void onDeleteBtnClicked();
    void onModifyBtnClicked();

private:
    const QString m_configName;

    QPushButton *m_modifyBtn;
    QPushButton *m_deleteBtn;
    QLabel *m_modifyTips;
    QLabel *m_deleteTips;
};

}

}

#endif // CUSTOMCONFIGPAGE_H
