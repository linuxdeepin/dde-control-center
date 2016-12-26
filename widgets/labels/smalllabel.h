#ifndef SMALLLABEL_H
#define SMALLLABEL_H

#include <QLabel>

namespace dcc {
namespace widgets {

class SmallLabel : public QLabel
{
    Q_OBJECT

public:
    explicit SmallLabel(QWidget *parent = 0);
    explicit SmallLabel(const QString &text, QWidget *parent = 0);
};

}
}

#endif // SMALLLABEL_H
