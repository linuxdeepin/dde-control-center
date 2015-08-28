#ifndef DISPLAYMODEITEM_H
#define DISPLAYMODEITEM_H

#include <QLabel>

class DisplayModeItem : public QLabel
{
    Q_OBJECT

    Q_PROPERTY(QString iconName READ iconName WRITE setIconName)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool active READ active WRITE setActive)

public:
    explicit DisplayModeItem(bool active = false, QLabel *parent = 0);

    QString iconName() const;
    QString title() const;
    QString text() const;
    bool active() const;

public slots:
    void setIconName(QString iconName);
    void setTitle(QString title);
    void setText(QString text);
    void setActive(bool active);

private:
    QString m_iconName;
    QString m_iconPath;
    QLabel *m_title;
    QLabel *m_text;
    bool m_active;
};

#endif // DISPLAYMODEITEM_H
