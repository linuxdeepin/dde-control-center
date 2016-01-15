#ifndef UPDATEPROGRESS_H
#define UPDATEPROGRESS_H

#include <libdui/dcircleprogress.h>
#include <libdui/dpicturesequenceview.h>

DUI_USE_NAMESPACE

class UpdateProgress : public DCircleProgress
{
public:
    explicit UpdateProgress(QWidget *parent = nullptr);

    inline void showLoading() {m_picSeq->show();}
    inline void hideLoading() {m_picSeq->hide();}

private:
    DPictureSequenceView *m_picSeq;
};

#endif // UPDATEPROGRESS_H
