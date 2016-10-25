#include "defcategorywidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"


using namespace dcc;

DefCategoryWidget::DefCategoryWidget() {
    m_userGroup = new SettingsGroup;
    m_centeralLayout->addWidget(m_userGroup);
    m_centeralLayout->addStretch(0);

    setTitle(tr("Default Applications"));
}

//添加列表
void DefCategoryWidget::add(QString name, QString id, QString mime) {
    m_optionWidget = new OptionWidget;
    m_optionWidget->setName(name);
    m_optionWidget->setDisplayName(name);
    m_optionWidget->setId(id);
    m_optionWidget->setMime(mime);
    m_optionWidget->setFixedHeight(30);
    //    m_optionWidget->setIcon();
    m_userGroup->appendItem(m_optionWidget);
    connect(m_optionWidget,&OptionWidget::set,this,&DefCategoryWidget::onSetRunner);
}

void DefCategoryWidget::onDeleteRunner() {
    //当信号过来的时候再循环遍历
    for (int cc = m_userGroup->layout()->count()-1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            //doing something for m_optionWidget
            m_optionWidget->setDelete();
        }
    }
}

void DefCategoryWidget::onSetRunner(QString mime, QString id) {
    for (int cc = m_userGroup->layout()->count()-1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            //doing something for m_optionWidget
            if(m_optionWidget->id()==id) {
                m_optionWidget->setChecked(true);
                //set dbus
                emit setDefaultApp(id,mime);
            }
            m_optionWidget->setChecked(false);
        }
    }
}

void DefCategoryWidget::onDefaultAppSet(QString id) {
    for (int cc = m_userGroup->layout()->count()-1; cc >= 0; --cc) {
        m_userGroupLayout = m_userGroup->layout()->itemAt(cc);
        m_optionWidget = qobject_cast<OptionWidget *>(m_userGroupLayout->widget());
        if (m_optionWidget != 0) {
            //doing something for m_optionWidget
            if(m_optionWidget->id() == id) {
                m_optionWidget->setChecked(true);
                qDebug()<<"已设置 "<<id;
                break;
            }
            m_optionWidget->setChecked(false);
        }
    }
}

