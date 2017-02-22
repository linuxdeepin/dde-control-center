#ifndef MODULEWIDGETHEADER_H
#define MODULEWIDGETHEADER_H

#include <QFrame>

namespace dcc {

namespace widgets {

class ModuleWidgetHeader : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleWidgetHeader(QWidget *parent = 0);

signals:
    void clicked() const;

private:
    void mouseReleaseEvent(QMouseEvent *e);
};

}

}

#endif // MODULEWIDGETHEADER_H
