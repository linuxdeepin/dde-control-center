#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>

namespace dcc {

class ContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ContentWidget(QWidget *parent = 0);

    void setTitle(const QString &title);
    void sendBackSignal();
    QWidget *setContent(QWidget * const w);

signals:
    void back() const;

public slots:
    void scrollToWidget(QWidget * const w);

private:
    bool eventFilter(QObject *watched, QEvent *event);

protected:
    QLabel *m_title;
    QScrollArea *m_contentArea;

    QWidget *m_content;
};

}

#endif // CONTENTWIDGET_H
