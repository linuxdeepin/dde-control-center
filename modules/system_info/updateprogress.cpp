#include "updateprogress.h"

DUI_USE_NAMESPACE

UpdateProgress::UpdateProgress(QWidget *parent) :
    DCircleProgress(parent)
{
    QStringList seq;
    for (int i(0); i != 61; ++i)
        seq.append(QString(":/images/loading/images/eLoading/eLoading_%1.png").arg(i, 2, 10, QChar('0')));
    for (int i(0); i != 30; ++i)
        seq.append(QString());

    m_picSeq = new DPictureSequenceView(this);
    m_picSeq->setPictureSequence(seq);
    m_picSeq->setFixedSize(33, 33);
    m_picSeq->move(-2, -3);

    this->setProperty("backgroundColor", QColor(255, 255, 255, 200));
}

