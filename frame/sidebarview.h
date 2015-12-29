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

    bool firstItemVisible();
    bool lastItemVisible();

    QSize sizeHint() const Q_DECL_OVERRIDE;
    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

public slots:
    void scrollDown();
    void scrollUp();

signals:
    void moduleSelected(const ModuleMetaData &meta) const;
    void scrolled(int pos) const;
    void sizeChanged(const QSize &size) const;

protected:
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;

private:
    DTipsFrame *m_tips;
};

#endif // SIDEBARVIEW_H
