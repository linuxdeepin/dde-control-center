#include "copyrightwidget.h"
#include "translucentframe.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QFile>
#include <dboxwidget.h>
#include <QLocale>

using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

namespace dcc{
namespace systeminfo{

CopyrightWidget::CopyrightWidget(QWidget *parent)
    :ContentWidget(parent)
{
    TranslucentFrame* widget = new TranslucentFrame;
    QVBoxLayout *layout =new QVBoxLayout();
    m_title = new QLabel();
    m_title->setText(getLicense(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "title"));

    m_body = new QLabel();
    m_body->setWordWrap(true);
    m_body->setText(getLicense(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "body"));

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignCenter);
    layout->addWidget(m_body);

    widget->setLayout(layout);
    setContent(widget);
    setTitle(tr("License"));
}

QString CopyrightWidget::getLicense(const QString &filePath, const QString &type) const
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

    return buf;
}

}
}
