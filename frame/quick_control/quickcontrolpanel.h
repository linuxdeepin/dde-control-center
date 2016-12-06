#ifndef QUICKCONTROLPANEL_H
#define QUICKCONTROLPANEL_H

#include <QWidget>
#include <QStackedLayout>
#include <QPushButton>

class QuickControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit QuickControlPanel(QWidget *parent = 0);

signals:
    void requestDetailConfig() const;

private:
    QStackedLayout *m_itemStack;
};

#endif // QUICKCONTROLPANEL_H
