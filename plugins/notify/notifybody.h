#ifndef NOTIFYBODY_H
#define NOTIFYBODY_H

#include <QWidget>
#include <QTextDocument>
#include <QTextOption>

class NotifyBody : public QWidget
{
    Q_OBJECT
public:
    explicit NotifyBody(QWidget *parent = 0);
    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QTextDocument *m_td;

private:
    const QString holdTextInRect(const QFontMetrics &fm, const QString &text, const QRect &rect) const;
};

#endif // NOTIFYBODY_H
