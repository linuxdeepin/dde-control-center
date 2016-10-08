#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>

class ContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContentWidget(QWidget *parent = 0);

signals:
    void back();

protected:
    QLabel *m_title;
    QScrollArea *m_contentArea;
};

#endif // CONTENTWIDGET_H
