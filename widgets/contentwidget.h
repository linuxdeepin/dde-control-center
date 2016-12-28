#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QWidget>
#include <QLabel>

class QVBoxLayout;
class QScrollArea;

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
    void appear() const;
    void disappear() const;

public slots:
    void scrollToWidget(QWidget * const w);

private:
    bool eventFilter(QObject *watched, QEvent *event);

protected:
    QLabel *m_title;
    QVBoxLayout *m_contentTopLayout;
    QScrollArea *m_contentArea;

    QWidget *m_content;
};

}

#endif // CONTENTWIDGET_H
