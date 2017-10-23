#ifndef FINGERWIDGET_H
#define FINGERWIDGET_H

#include <dpicturesequenceview.h>
#include <QWidget>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace accounts {
class FingerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FingerWidget(QWidget *parent = nullptr);

private:
    DPictureSequenceView *m_view;
};
}
}

#endif // FINGERWIDGET_H
