#include "mirroritem.h"
#include <QHBoxLayout>
#include <QProcess>

MirrorItem::MirrorItem(QFrame *parent)
    :SettingsItem(parent)
{
    m_selectedBtn = new DImageButton;
    m_selectedBtn->setNormalPic(":/update/icon/select.png");
    m_selectedBtn->setVisible(false);
    m_mirrorName = new QLabel;
    m_mirrorSpeed = new QLabel;

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(m_mirrorName);
    layout->addStretch();
    layout->addWidget(m_selectedBtn);
    layout->addWidget(m_mirrorSpeed);

    setLayout(layout);
    setFixedHeight(45);
}

void MirrorItem::setMirrorInfo(const MirrorInfo &info)
{
    m_info = info;
    setMirrorName(info.m_name);
    testMirrorSpeed(info.m_url);
}

void MirrorItem::setSelected(bool state)
{
    if (state == m_selected)
        return;

    m_selected = state;
    m_selectedBtn->setVisible(state);

    emit selectStateChanged(state);
}

QSize MirrorItem::sizeHint() const
{
    return QSize(width(), height());
}

void MirrorItem::testMirrorSpeed (const QString &mirrorAdr)
{
    QStringList args;
    args << mirrorAdr << "-s" << "1";

    QProcess *process = new QProcess;
    connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &MirrorItem::testMirrorSpeed_finish);

    process->start("netselect", args);
}

void MirrorItem::testMirrorSpeed_finish(int ret)
{
    QProcess *process = qobject_cast<QProcess *>(sender());

    if (!process)
        return;

    // process exit with an error
    if (ret)
    {
        //: the mirror cant reachable
        m_mirrorSpeed->setText(tr("Timeout"));
        return process->deleteLater();
    }

    const QString output = process->readAllStandardOutput().trimmed();
    const QStringList result = output.split(' ');
    const int time = result.first().toInt();

    if (result.first().isEmpty())
        m_mirrorSpeed->setText(tr("Timeout"));
    else if (time > 2000)
        m_mirrorSpeed->setText(tr("Slow"));
    else if (time > 200)
        m_mirrorSpeed->setText(tr("Medium"));
    else
        m_mirrorSpeed->setText(tr("Fast"));

    process->deleteLater();
}

void MirrorItem::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked(this);

    QFrame::mouseReleaseEvent(e);
}
