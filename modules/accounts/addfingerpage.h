#ifndef ADDFINGERPAGE_H
#define ADDFINGERPAGE_H

#include "contentwidget.h"
#include "buttontuple.h"
#include "widgets/fingerwidget.h"
#include "fingermodel.h"

#include <QWidget>

namespace dcc {
namespace accounts {
class AddFingerPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit AddFingerPage(const QString &thumb, QWidget *parent = nullptr);

    void setFingerModel(FingerModel *model);
    void setUsername(const QString &name);

signals:
    void requestSaveThumb(const QString &name);
    void requestReEnrollStart(const QString &thumb);

private slots:
    void saveThumb();
    void reEnrollStart();
    void onEnrollStatusChanged(FingerModel::EnrollStatus status);

private:
    FingerModel *m_model;
    widgets::ButtonTuple *m_buttonTuple;
    FingerWidget *m_fingerWidget;
    int m_frequency;
    QString m_thumb;
    QString m_username;
};
}
}

#endif // ADDFINGERPAGE_H
