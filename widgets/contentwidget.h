#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QLabel>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class QVBoxLayout;
class QScrollArea;
class QPropertyAnimation;
class QPushButton;
namespace dcc {

class ContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContentWidget(QWidget *parent = 0);

    void setTitle(const QString &title);
    void sendBackSignal();
    QWidget *content() const { return m_content; }
    QWidget *setContent(QWidget * const w);
    void stopScroll();

signals:
    void back() const;
    void appear() const;
    void disappear() const;

public slots:
    void scrollToWidget(QWidget * const w);

private:
    bool eventFilter(QObject *watched, QEvent *event);

protected:
    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

protected:
    DImageButton *m_navgationBtn;
    QLabel *m_title;
    QVBoxLayout *m_contentTopLayout;
    QScrollArea *m_contentArea;

    QWidget *m_content;

    QPropertyAnimation *m_animation;
    double m_speedTime;
    int m_speed;
};

}

#endif // CONTENTWIDGET_H
