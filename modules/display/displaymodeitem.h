#ifndef DISPLAYMODEITEM_H
#define DISPLAYMODEITEM_H

#include <QLabel>

class DisplayModeItem : public QLabel
{
    Q_OBJECT

    Q_PROPERTY(QString iconName READ iconName WRITE setIconName)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool hover READ hover NOTIFY hoverChanged)

public:
    explicit DisplayModeItem(bool showSeparator = true, QLabel *parent = 0);
    ~DisplayModeItem();

    QString iconName() const;
    QString title() const;
    QString text() const;
    bool active() const;
    bool hover() const;

public slots:
    void setIconName(QString iconName);
    void setTitle(QString title);
    void setText(QString text);
    void setActive(bool active);

signals:
    void hoverChanged(bool hover);
    void activeChanged(bool active);

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    void setHover(bool arg);
    void updateIcon();

private:
    static DisplayModeItem *activeItem;

    QString m_iconName;
    QString m_iconPath;
    QLabel *m_title;
    QLabel *m_text;
    bool m_active;
    bool m_hover;
};

#endif // DISPLAYMODEITEM_H
