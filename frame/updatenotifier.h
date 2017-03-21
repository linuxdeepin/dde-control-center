#ifndef UPDATENOTIFIER_H
#define UPDATENOTIFIER_H

#include <QFrame>

#include <dimagebutton.h>

#include "labels/normallabel.h"
#include "labels/tipslabel.h"

#include <com_deepin_lastore_updater.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;

class QLabel;
class UpdateNotifier : public QFrame
{
    Q_OBJECT
public:
    explicit UpdateNotifier(QWidget *parent = 0);

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    QLabel *m_icon;
    NormalLabel *m_title;
    TipsLabel *m_content;
    DImageButton *m_closeButton;

    QSettings *m_settings;

    QStringList m_updatablePkgs;
    com::deepin::lastore::Updater *m_updaterInter;

    void ignoreUpdates();
    void updatablePkgsChanged(const QStringList &value);
    bool comparePkgLists(QStringList val1, QStringList val2);
};

#endif // UPDATENOTIFIER_H
