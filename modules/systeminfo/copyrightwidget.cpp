#include "copyrightwidget.h"
#include "translucentframe.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QFile>
#include <dboxwidget.h>
#include <QLocale>
#include <QFutureWatcher>
#include <QtConcurrent>

#include "labels/tipslabel.h"

DWIDGET_USE_NAMESPACE

namespace dcc{
namespace systeminfo{

const QString getLicense(const QString &filePath, const QString &type)
{
    QString lang = QLocale::system().name();
    if (lang != "zh_CN" && lang != "zh_TW")
        lang = "en";

    QString path = QString(filePath).arg(lang).arg(type);
    QFile license(path);
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return std::move(buf);
}

QPair<QString, QString> loadLicenses()
{
    const QString title = getLicense(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "title");
    const QString body = getLicense(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "body");

    return QPair<QString, QString>(title, body);
}

CopyrightWidget::CopyrightWidget(QWidget *parent)
    :ContentWidget(parent)
{
    TranslucentFrame* widget = new TranslucentFrame;
    widget->setObjectName("copyrightWidget");
    QVBoxLayout *layout =new QVBoxLayout();

    m_title = new TipsLabel;
    m_body = new TipsLabel;
    m_body->setWordWrap(true);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignCenter);
    layout->addWidget(m_body);

    widget->setLayout(layout);
    setContent(widget);
    setTitle(tr("License"));

    QFutureWatcher<QPair<QString, QString>> *w = new QFutureWatcher<QPair<QString, QString>>(this);
    w->setFuture(QtConcurrent::run(loadLicenses));

    connect(w, &QFutureWatcher<QPair<QString, QString>>::finished, this, [=] {
        const auto r = w->result();

        m_title->setText(r.first);
        m_body->setText(r.second);
    });
}

}
}
