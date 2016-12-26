#ifndef MIRRORSWIDGET_H
#define MIRRORSWIDGET_H

#include "contentwidget.h"
#include "mirroritem.h"
#include "settingsgroup.h"

#include <types/mirrorinfolist.h>

class QStackedLayout;

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

private slots:
    void setCurItem(MirrorItem* item);
    void onSpeedInfoAvailable(const QMap<QString, int> &info);

private:
    void setDefaultMirror(const MirrorInfo& mirror);
    void setMirrorInfoList(const MirrorInfoList& list);

private:
    MirrorItem* m_curItem;
    MirrorInfo m_defaultMirror;
    SettingsGroup *m_loadingGroup;
    SettingsGroup *m_mirrorListGroup;
    QStackedLayout *m_layout;
};

}
}
#endif // MIRRORSWIDGET_H
