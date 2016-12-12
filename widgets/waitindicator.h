#ifndef WAITINDICATOR_H
#define WAITINDICATOR_H

#include "settingsitem.h"
#include <dpicturesequenceview.h>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class WaitIndicator : public SettingsItem
{
    Q_OBJECT

public:
    explicit WaitIndicator(QFrame* parent = 0);
    void setPictureSequence(const QStringList &sequence);
    void setTitle(const QString& title);
    void setTitleDisplay(bool display);

    void play();
    void pause();
    void stop();
    void setSpeed(int speed);

private:
    QLabel* m_title;
    DPictureSequenceView *m_view;
};

}
}

#endif // WAITINDICATOR_H
