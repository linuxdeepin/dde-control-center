#ifndef SCROLLFRAME_H
#define SCROLLFRAME_H

#include <QWidget>
#include <QBoxLayout>
#include <QPointer>

#include <libdui/dscrollarea.h>
#include <libdui/dboxwidget.h>
#include <libdui/dstackwidget.h>

DUI_USE_NAMESPACE

class ScrollFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ScrollFrame(QWidget *parent = 0);

    QBoxLayout *headerLayout() const;
    QBoxLayout *mainLayout() const;

    void pushWidget(QWidget *widget);
    void popCurrentWidget();
    void popAllWidget();

private slots:
    void onCurrentWidgetSizeChanged(const QSize &size);

signals:
    void sizeChanged(QSize size);
    void currentMainWidgetChanged(const QWidget *widget);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void init();
    void setStackWidgetHeight(int height);

    DScrollArea *m_scrollArea = nullptr;
    DStackWidget *m_stackWidget = nullptr;
    QPointer<const DBoxWidget> m_currentWidget;
    DVBoxWidget *m_headerWidget = nullptr;
    DVBoxWidget *m_mainWidget = nullptr;
};

#endif // SCROLLFRAME_H
