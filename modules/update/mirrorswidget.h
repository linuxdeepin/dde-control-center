#ifndef MIRRORSWIDGET_H
#define MIRRORSWIDGET_H

#include "contentwidget.h"
#include "mirroritem.h"
#include "settingsgroup.h"

#include <types/mirrorinfolist.h>

using namespace dcc;

namespace dcc{
namespace update{

class UpdateModel;

class MirrorsWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit MirrorsWidget(UpdateModel *model, QWidget* parent = 0);

    void setModel(UpdateModel *model);

signals:
    void requestSetDefaultMirror(const MirrorInfo &mirror);
    void requestTestMirrorSpeed();

private slots:
    void setCurItem(MirrorItem* item);
    void onSpeedInfoAvailable(const QMap<QString, int> &info);
    void testButtonClicked();

    void sortMirrorsBySpeed();

private:
    void setDefaultMirror(const MirrorInfo& mirror);
    void setMirrorInfoList(const MirrorInfoList& list);

private:
    enum TestProgress {
        NotStarted,
        Running,
        Done
    };

    MirrorItem* m_curItem;
    MirrorInfo m_defaultMirror;
    TestProgress m_testProgress;

    QPushButton *m_testButton;
    SettingsGroup *m_mirrorListGroup;
    QVBoxLayout *m_layout;
};

}
}
#endif // MIRRORSWIDGET_H
