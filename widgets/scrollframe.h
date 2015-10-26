#ifndef SCROLLFRAME_H
#define SCROLLFRAME_H

#include <QWidget>
#include <QBoxLayout>

#include <libdui/dscrollarea.h>
#include <libdui/dboxwidget.h>

DUI_USE_NAMESPACE

class ScrollFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ScrollFrame(QWidget *parent = 0);

    QBoxLayout *headerLayout() const;
    QBoxLayout *mainLayout() const;

signals:
    void sizeChanged(QSize size);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void init();

    DScrollArea *m_scrollArea = nullptr;
    DVBoxWidget *m_headerWidget = nullptr;
    DVBoxWidget *m_mainWidget = nullptr;
};

#endif // SCROLLFRAME_H
