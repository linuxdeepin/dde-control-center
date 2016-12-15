#ifndef DOUTESTWIDGET_H
#define DOUTESTWIDGET_H

#include "settingsitem.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QStringList>
#include <dpicturesequenceview.h>

DWIDGET_USE_NAMESPACE

class QMouseEvent;
namespace dcc
{
namespace widgets
{
class SettingsItem;
}
namespace mouse
{
class DouTestWidget : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit DouTestWidget(QWidget *parent = 0);

    enum State {
        BOW,RAISE
    };

    struct DoubleTestPic {
        QStringList double_1;
        QStringList double_2;
        QStringList click_1;
        QStringList click_2;
    };

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QVBoxLayout           *m_mainlayout;
    DPictureSequenceView  *m_testWidget;
    State                  m_state;
    DoubleTestPic          m_doubleTest;
};

}
}


#endif // DOUTESTWIDGET_H
