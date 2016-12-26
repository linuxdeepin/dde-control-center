#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>

#include "common.h"

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

namespace dcc{
namespace update{

class DownloadProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit DownloadProgressBar(QWidget* parent = 0);

    void setMessage(const QString &message);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void clicked();

private:
    dcc::widgets::NormalLabel *m_message;
};

}
}
#endif // DOWNLOADPROGRESSBAR_H
