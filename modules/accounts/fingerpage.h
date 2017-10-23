#ifndef FINGERPAGE_H
#define FINGERPAGE_H

#include "contentwidget.h"
#include "user.h"
#include "fingermodel.h"

#include <com_deepin_daemon_fprintd_device.h>

using com::deepin::daemon::fprintd::Device;

namespace dcc {

namespace widgets {
class SettingsGroup;
}

namespace accounts {

class FingerPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit FingerPage(User *user,QWidget *parent = nullptr);

    void setFingerModel(FingerModel *model);

signals:
    void requestAddThumbs(const QString &name, const QString &thumb);
    void requestCleanThumbs(User *user);

private slots:
    void onThumbsListChanged(const QList<FingerModel::UserThumbs> &thumbs);
    void addThumb();
    void cleanThumbs();

private:
    widgets::SettingsGroup *m_listGrp;
    User* m_user;
    FingerModel *m_model;
    QPushButton *m_addBtn;
    QPushButton *m_cleanBtn;
    QString m_notUseThumb;
};

}
}


#endif // FINGERPAGE_H
