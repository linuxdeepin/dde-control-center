#ifndef SIDEBARVIEW_H
#define SIDEBARVIEW_H

#include <QListView>

#include "dtipsframe.h"
#include "modulemetadata.h"

class SidebarModel;
class SidebarView : public QListView
{
    Q_OBJECT

public:
    SidebarView(QWidget *parent = nullptr);
    ~SidebarView();

    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

signals:
    void moduleSelected(const ModuleMetaData &meta) const;

protected:
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    DTipsFrame *m_tips;
};

#endif // SIDEBARVIEW_H
