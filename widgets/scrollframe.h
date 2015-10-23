#ifndef SCROLLFRAME_H
#define SCROLLFRAME_H

#include <QWidget>
#include <QBoxLayout>

#include <libdui/dscrollarea.h>

DUI_USE_NAMESPACE

class ScrollFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ScrollFrame(QWidget *parent = 0);
    explicit ScrollFrame(QBoxLayout *layout, QWidget *parent = 0);

    void setHeaderWidget(QWidget *widget);
    QBoxLayout *mainLayout() const;
    void setMainLayout(QBoxLayout *layout);

signals:
    void sizeChanged(QSize size);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *e) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *ee) Q_DECL_OVERRIDE;

private:
    void init(QBoxLayout *layout);

    DScrollArea *m_scrollArea = nullptr;
    QWidget *m_mainWidget = nullptr;
    QBoxLayout *m_mainLayout = nullptr;
    QWidget *m_headerWidget = nullptr;
};

#endif // SCROLLFRAME_H
