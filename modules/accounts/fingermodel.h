#ifndef FINGERMODEL_H
#define FINGERMODEL_H

#include <QObject>

static const QStringList thumbsLists =  { "right-little-finger","left-index-finger",
                                          "right-index-finger", "left-middle-finger" ,
                                          "right-thumb" , "left-ring-finger" ,"left-little-finger" ,
                                          "right-middle-finger" , "left-thumb" ,"right-ring-finger"};

namespace dcc {
namespace accounts {
class FingerModel : public QObject
{
    Q_OBJECT
public:
    explicit FingerModel(QObject *parent = nullptr);

    struct UserThumbs {
        QString username;
        QStringList userThumbs;

        bool operator ==(const UserThumbs &user) {
            return username == user.username && userThumbs == user.userThumbs;
        }
    };

    enum EnrollStatus{
        Ready,
        Next,
        Finished
    };

    bool isVaild() const;
    void setIsVaild(bool isVaild);

    EnrollStatus enrollStatus() const;
    void setEnrollStatus(const EnrollStatus &enrollStatus);

    void addUserThumbs(const UserThumbs &thubs);
    void cleanUserThumbs(const QString &user);
    QList<UserThumbs> thumbsList() const;

signals:
    void vaildChanged(const bool isVaild);
    void enrollStatusChanged(EnrollStatus status);
    void thumbsListChanged(const QList<UserThumbs> &thumbs);

private:
    bool m_isVaild;
    EnrollStatus m_enrollStatus;
    QList<UserThumbs> m_thumbsList;
};
}
}

#endif // FINGERMODEL_H
