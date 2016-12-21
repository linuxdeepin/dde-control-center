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

private slots:
    void setCurItem(MirrorItem* item);

private:
    void setDefaultMirror(const MirrorInfo& mirror);
    void setMirrorInfoList(const MirrorInfoList& list);

private:
    MirrorItem* m_curItem;
    SettingsGroup* m_group;
    MirrorInfo m_defaultMirror;
};

}
}
#endif // MIRRORSWIDGET_H
