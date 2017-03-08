#ifndef NAVGATIONVIEW_H
#define NAVGATIONVIEW_H

#include <QListView>

namespace dcc {

class NavgationView : public QListView
{
    Q_OBJECT

public:
    explicit NavgationView(QWidget *parent = nullptr);

protected:
    void leaveEvent(QEvent *e);
};

}

#endif // NAVGATIONVIEW_H
