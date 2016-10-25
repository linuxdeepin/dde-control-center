#ifndef OPTIONWIDGET_H
#define OPTIONWIDGET_H

#include "settingsitem.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QPushButton>
#include <dimagebutton.h>

namespace dcc {

DWIDGET_USE_NAMESPACE

class OptionWidget : public SettingsItem
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)

public:
    explicit OptionWidget(QWidget *parent = 0);
    const QString id()    const;
    const QString name()  const;
    const QString displayName() const;
    const QString mime() const;
    bool checked() const;
    void setCheckedIcon(const QPixmap &icon);
    QHBoxLayout *layout();

public slots:
    void setName(const QString &name);
    void setDisplayName(const QString &displayname);
    void setId(const QString &id);
    void setIcon(const QString &icon);
    void setIcon(const QPixmap &icon);
    void setChecked(bool checked);
    void setDelete();
    void setMime(const QString &mime);

private:
    QHBoxLayout  *m_mainlayout;
    QLabel       *m_optionIcon;
    QString       m_optionName;
    DImageButton *m_delete;
    QLabel       *m_checkedIconLabel;
    QLabel       *m_displayName;
    bool          m_checked           = false;
    QString       m_mime;
    QString       m_optionid;

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void set(QString mime, QString id);
    void deleteClicked();
    void checkedChanged(bool checked) const;
    void checkedIconChanged(const QPixmap &icon) const;
};

}

#endif // OPTIONWIDGET_H
