#ifndef COPYRIGHTWIDGET_H
#define COPYRIGHTWIDGET_H

#include "contentwidget.h"

class CopyrightWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit CopyrightWidget(QWidget* parent = 0);
    void setLicenseTitle(const QString& title);
    void setLicenseBody(const QString& body);

private:
    QString getLicense(const QString& filePath, const QString& type) const;

private:
    QLabel* m_title;
    QLabel* m_body;
};

#endif // COPYRIGHTWIDGET_H
