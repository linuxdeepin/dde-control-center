#ifndef NAVITEMWIDGET_H
#define NAVITEMWIDGET_H

#include <QWidget>

class NavItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NavItemWidget(const QString &id, QWidget *parent = 0);

signals:
    void itemClicked(const QString &id) const;
    void itemEntered(const QString &id) const;

protected:
    void paintEvent(QPaintEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

private:
    const QString m_id;
    bool m_hover;
};

#endif // NAVITEMWIDGET_H
