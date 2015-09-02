#ifndef BOOTMENULIST_H
#define BOOTMENULIST_H

#include <QFrame>
#include <QVBoxLayout>

class BootMenuList : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QColor normalColor READ normalColor WRITE setNormalColor NOTIFY normalColorChanged)
    Q_PROPERTY(QColor enabledColor READ enabledColor WRITE setEnabledColor NOTIFY enabledColorChanged)

public:
    explicit BootMenuList(QWidget *parent = 0);
    QColor normalColor() const;
    QColor enabledColor() const;

public slots:
    void addButtons(const QStringList& listLabels);
    void checkButtonByIndex(int index);
    void setNormalColor(QColor normalColor);
    void setEnabledColor(QColor enabledColor);

signals:
    void normalColorChanged(QColor normalColor);
    void enabledColorChanged(QColor enabledColor);

private:
    QVBoxLayout *m_layout;
    QColor m_normalColor;
    QColor m_enabledColor;
};

#endif // BOOTMENULIST_H
