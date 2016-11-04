#include "openwithotherdialog.h"


#include <QListWidget>
#include <QListWidgetItem>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>

#include <dscrollbar.h>


OpenWithOtherDialog::OpenWithOtherDialog(QWidget *parent) :
    DDialog(parent)
{
    setModal(true);
    initUI();
    initConnect();
}

OpenWithOtherDialog::~OpenWithOtherDialog()
{

}

void OpenWithOtherDialog::initUI()
{
    setFixedSize(300, 400);
    m_appListWidget = createOpenWithListWidget();

    QLabel* m_titleLabel = new QLabel(tr("Choose File"), this);
    m_titleLabel->setFixedSize(200, 40);
    m_titleLabel->move(50, 0);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    QStringList buttonTexts;
    buttonTexts << tr("Cancel") << tr("Choose");

    addContent(m_appListWidget);
    addButtons(buttonTexts);
    setSpacing(0);
    setDefaultButton(1);

}

void OpenWithOtherDialog::initConnect()
{
    connect(this, SIGNAL(buttonClicked(int,QString)), this, SLOT(handleButtonClicked(int,QString)));
}

QListWidget *OpenWithOtherDialog::createOpenWithListWidget()
{
    QListWidget* listWidget = new QListWidget;
    m_OpenWithButtonGroup = new QButtonGroup(listWidget);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setVerticalScrollBar(new DScrollBar);

    QString path = "/usr/share/applications/";
    QFileInfoList list = GetFileList(path);
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QListWidgetItem* item = new QListWidgetItem;

        QCheckBox* itemBox = new QCheckBox;
        //        itemBox->setStyleSheet(getQssFromFile(":/light/OpenWithOtherDialog.theme"));
        itemBox->setText(fileInfo.baseName());
        itemBox->setFixedHeight(36);
        itemBox->setProperty("app",fileInfo.fileName());
        m_OpenWithButtonGroup->addButton(itemBox);
        listWidget->addItem(item);
        listWidget->setItemWidget(item, itemBox);
    }

    int listHeight = 2;
    for(int i=0; i < listWidget->count(); i++){
        QListWidgetItem* item = listWidget->item(i);
        item->setFlags(Qt::NoItemFlags);
        int h = listWidget->itemWidget(item)->height();
        item->setSizeHint(QSize(item->sizeHint().width(), h));
        listHeight += h;
    }

    if (listHeight >= 36 * 9){
        listWidget->setFixedHeight(36 * 9);
    }else{
        listWidget->setFixedHeight(listHeight);
    }


    return listWidget;
}

void OpenWithOtherDialog::handleButtonClicked(int index, QString text)
{
    Q_UNUSED(text)
    if (index == 1){
        if (m_OpenWithButtonGroup->checkedButton()){
            QString app = m_OpenWithButtonGroup->checkedButton()->property("app").toString();
            desktopFile = app;
        }
    }
}

QFileInfoList OpenWithOtherDialog::GetFileList(const QString &path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }

    return file_list;
}

QString OpenWithOtherDialog::getQssFromFile(QString filename)
{
    QFile f(filename);
    QString qss = "";
    if (f.open(QFile::ReadOnly))
    {
        qss = QLatin1String(f.readAll());
        f.close();
    }
    return qss;
}
