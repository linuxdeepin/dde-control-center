#ifndef FINGERWIDGET_H
#define FINGERWIDGET_H

#include <dpicturesequenceview.h>
#include <QWidget>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace accounts {
class FingerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FingerWidget(QWidget *parent = nullptr);

    void setFrequency(const QString &value);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    DPictureSequenceView *m_view;
    QLabel *m_tipLbl;
};
}
}

#endif // FINGERWIDGET_H
