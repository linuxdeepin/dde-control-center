#ifndef SIDEBARVIEW_H
#define SIDEBARVIEW_H

#include <QListView>

#include "dtipsframe.h"
#include "modulemetadata.h"

class SidebarView : public QListView
{
    Q_OBJECT

public:
    SidebarView(QWidget *parent = nullptr);
    ~SidebarView();

signals:
    void moduleSelected(const ModuleMetaData &meta) const;

protected:
    void leaveEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    DTipsFrame *m_tips;
};

#endif // SIDEBARVIEW_H
