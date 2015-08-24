#ifndef BOOTMENULIST_H
#define BOOTMENULIST_H

#include <QFrame>
#include <QVBoxLayout>

class BootMenuList : public QFrame
{
    Q_OBJECT
public:
    explicit BootMenuList(QWidget *parent = 0);

public slots:
    void addButtons(const QStringList& listLabels);
    void checkButtonByIndex(int index);

private:
    QVBoxLayout *m_layout;
};

#endif // BOOTMENULIST_H
