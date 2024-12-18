// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "avatarlistframe.h"

#include "avatarcropbox.h"

#include <DSlider>
#include <DIconTheme>
#include <DGuiApplicationHelper>

#include <QColor>
#include <QDateTime>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMimeData>
#include <QPainter>
#include <QPoint>
#include <QScreen>
#include <QScrollArea>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

DGUI_USE_NAMESPACE

const QString VarDirectory = QStringLiteral(VARDIRECTORY);

// 系统用户头像存放路径
const QString PersonDimensionalPath = QStringLiteral("lib/AccountsService/icons/human/dimensional");
const QString PersonDimensional2Path = QStringLiteral("lib/AccountsService/icons/human/dimensional_v2");
const QString PersonFlatPath = QStringLiteral("lib/AccountsService/icons/human/flat");
const QString AnimalDimensionalPath = QStringLiteral("lib/AccountsService/icons/animal");
const QString SceneryPath = QStringLiteral("lib/AccountsService/icons/scenery");
const QString IllustrationDimensionalPath = QStringLiteral("lib/AccountsService/icons/illustration");
const QString EmojiDimensionalPath = QStringLiteral("lib/AccountsService/icons/emoji");

// 用户自定义图像存放路径
const QString AvatarCustomPath = QStringLiteral("lib/AccountsService/icons/local");

#define BORDER_BOX_SIZE 190
#define AVATAR_ICON_SIZE 140
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 472
#define FIRST_PAGE_WIDTH 180 // 160 + 10 + 10
#define SECOND_PAGE_WIDTH 460
#define WINDOW_ROUND_SIZE 10
#define CROP_BOX_SIZE 120
#define SLIDER_MINIMUM_SIZE 1
#define SLIDER_MAX_SIZE 20
#define SLIDER_STEP 1

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
using namespace DCC_NAMESPACE;

// 头像选择项
struct AvatarItem
{
    QString name;
    QString icon;
    int role;

    AvatarItem(const QString &_name, const QString &_icon, const int &_role)
        : name(_name)
        , icon(_icon)
        , role(_role)
    {
    }
};

AvatarListFrame::AvatarListFrame(User * user, const int &role, QWidget *parent)
    : QFrame(parent)
    , m_role(role)
    , m_currentAvatarLsv(nullptr)
{
    const QString personDimensionPath = QString("%1/%2").arg(VarDirectory).arg(PersonDimensionalPath);
    const QString personDimension2Path = QString("%1/%2").arg(VarDirectory).arg(PersonDimensional2Path);
    const QString personFlatPath = QString("%1/%2").arg(VarDirectory).arg(PersonFlatPath);
    const QString animalDimensionPath = QString("%1/%2").arg(VarDirectory).arg(AnimalDimensionalPath);
    const QString sceneryPath = QString("%1/%2").arg(VarDirectory).arg(SceneryPath);
    const QString illustrationDimensionPath = QString("%1/%2").arg(VarDirectory).arg(IllustrationDimensionalPath);
    const QString emojiDimensionPath = QString("%1/%2").arg(VarDirectory).arg(EmojiDimensionalPath);
    const QString customAvatarPath = QString("%1/%2").arg(VarDirectory).arg(AvatarCustomPath);

    setFrameStyle(QFrame::NoFrame);
    setContentsMargins(0, 0, 0, 0);
    if (role == Role::Custom) {
        m_path = customAvatarPath;
        m_currentAvatarLsv = new AvatarListView(user, role, Type::Dimensional, customAvatarPath, this);
        m_currentAvatarLsv->setCurrentAvatarChecked(user->currentAvatar());
        return;
    }

    const QString &name = user->name();

    QList<AvatarRoleItem> items = {
        AvatarRoleItem{ Role::Person,
                        Type::Cartoon,
                        personDimensionPath,
                        isExistCustomAvatar(personDimensionPath, name) },
        AvatarRoleItem{ Role::Person,
                        Type::Dimensional,
                        personDimension2Path,
                        isExistCustomAvatar(personDimension2Path, name) },
        AvatarRoleItem{ Role::Person,
                        Type::Flat,
                        personFlatPath,
                        isExistCustomAvatar(personFlatPath, name) },
        AvatarRoleItem{ Role::Animal,
                        Type::Dimensional,
                        animalDimensionPath,
                        isExistCustomAvatar(animalDimensionPath, name) },
        AvatarRoleItem{ Role::Scenery,
                        Type::Dimensional,
                        sceneryPath,
                        isExistCustomAvatar(sceneryPath, name) },
        AvatarRoleItem{ Role::Illustration,
                        Type::Dimensional,
                        illustrationDimensionPath,
                        isExistCustomAvatar(illustrationDimensionPath, name) },
        AvatarRoleItem{ Role::Expression,
                        Type::Dimensional,
                        emojiDimensionPath,
                        isExistCustomAvatar(emojiDimensionPath, name) },
    };

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto addAvatar = [this, mainLayout, user](const AvatarRoleItem &item) {
        m_currentAvatarLsv = new AvatarListView(user, item.role, item.type, item.path, this);
        m_avatarViewMap[item.type] = m_currentAvatarLsv;
        m_currentAvatarLsv->setCurrentAvatarChecked(user->currentAvatar());

        QHBoxLayout *hBoxLayout = new QHBoxLayout;
        hBoxLayout->addWidget(m_currentAvatarLsv, Qt::AlignCenter);

        // 人物头像有两种, 需要添加类型标签
        if (item.role == Role::Person) {
            QLabel *dimStyleNameLabel = new QLabel(this);
            const QStringList styles { tr("Cartoon Style"), tr("Dimensional Style"), tr("Flat Style")};
            dimStyleNameLabel->setText(styles.value(item.type));

            QHBoxLayout *nameLabelLayout = new QHBoxLayout;
            nameLabelLayout->addSpacing(10);
            nameLabelLayout->addWidget(dimStyleNameLabel);
            mainLayout->addLayout(nameLabelLayout);
            mainLayout->addSpacing(2);
        }

        mainLayout->addLayout(hBoxLayout);

        connect(m_currentAvatarLsv,
                &AvatarListView::requestUpdateListView,
                this,
                &AvatarListFrame::updateListView);
    };

    for (const auto &item : items) {
        if (item.role == m_role && item.isLoader) {
            m_path = item.path;
            addAvatar(item);
        }
    }

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addLayout(mainLayout, Qt::AlignCenter);

    setLayout(layout);

    m_currentAvatarLsv = m_avatarViewMap.value(Type::Dimensional);
}

QString AvatarListFrame::getAvatarPath() const
{
    return m_currentAvatarLsv->getAvatarPath();
}

bool AvatarListFrame::isExistCustomAvatar(const QString &path, const QString &userName)
{
    QDir dir(path);
    QStringList filters{ "*.png", "*.jpg", ".jpeg", ".bmp" }; // 设置过滤类型
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();

    if (m_role != Custom) {
        return !list.isEmpty();
    }

    // 自定义账户页面检查是否存在当前用户自定义头像
    auto res = std::find_if(list.cbegin(), list.cend(), [ userName ](const QFileInfo &info)->bool{
        return info.filePath().contains(userName + "-");
    });

    if (res != list.cend()) {
        return true;
    }

    return false;
}

void AvatarListFrame::updateListView(bool isSave, const int &role, const int &type)
{
    Q_UNUSED(isSave);
    // 人物头像有两种类型,当有一种类型的item被选中时，取消另外一种类型item的选中状态
    if (role == Role::Person) {
        for (auto it = m_avatarViewMap.begin(); it != m_avatarViewMap.end(); ++it) {
            if (it.key() == type) {
                m_currentAvatarLsv = it.value();
            } else {
                it.value()->setCurrentAvatarUnChecked();
            }
        }
    }
}

CustomAddAvatarWidget::CustomAddAvatarWidget(User *user, const int &role, QWidget *parent)
    : AvatarListFrame(user, role, parent)
    , m_addAvatarFrame(new DFrame(this))
    , m_hintLabel(new QLabel(this))
    , m_addAvatarIconSpacer(new QSpacerItem(60, 60))
    , m_isDragIn(false)
    , m_isHover(false)
    , m_isPress(false)
{
    setAcceptDrops(true);
    m_addAvatarFrame->setFixedSize(400, 240);
    m_addAvatarFrame->setFrameStyle(QFrame::NoFrame);
    m_addAvatarFrame->setAcceptDrops(true);
    m_addAvatarFrame->installEventFilter(this);

    m_addAvatarDciIcon = DDciIcon::fromTheme("dcc_user_add_icon");

    m_hintLabel->setText(tr("You have not uploaded a picture, you can click or drag to upload a picture"));
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setWordWrap(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignHCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(m_addAvatarFrame);
    mainLayout->addStretch();

    QHBoxLayout *iconHLayout = new QHBoxLayout();
    iconHLayout->addStretch();
    iconHLayout->addItem(m_addAvatarIconSpacer);
    iconHLayout->addStretch();

    QVBoxLayout *avatarFrameLayout = new QVBoxLayout(m_addAvatarFrame);
    avatarFrameLayout->addStretch();
    avatarFrameLayout->addLayout(iconHLayout);
    avatarFrameLayout->addSpacing(20);
    avatarFrameLayout->addWidget(m_hintLabel);
    avatarFrameLayout->addStretch();

    installEventFilter(this);
};

CustomAddAvatarWidget::~CustomAddAvatarWidget()
{
}

void CustomAddAvatarWidget::saveCustomAvatar(const QString &avatar_path)
{
    auto saveFunc = [this](const QString &path) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QPixmap pix;
            pix.loadFromData(file.readAll());

            if (pix.isNull()) {
                // 用户上传的不是图片类型，提醒用户上传的文件类型错误
                m_hintLabel->clear();
                qWarning() << "failed to save file, maybe the file is not picture type";
                QPalette pe;
                pe.setColor(QPalette::Base, Qt::white);
                m_hintLabel->setPalette(pe);
                m_hintLabel->setText(tr("Uploaded file type is incorrect, please upload again"));

                file.close();
                return;
            }

            file.close();
        }

        if (!path.isEmpty()) {
            Q_EMIT requestUpdateCustomWidget(path);
        }
    };

    if (avatar_path.isEmpty()) {
        // open file manager to add pic
        QStringList directory = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        QFileDialog dialog;
        dialog.setNameFilter(tr("Images") + "(*.png *.bmp *.jpg *.jpeg)");
        if (!directory.isEmpty()) {
            dialog.setDirectory(directory.first());
        }
        if (dialog.exec() == QFileDialog::Accepted) {
            const QString path = dialog.selectedFiles().first();
            saveFunc(path);
        }
    } else {
        // save file
        saveFunc(avatar_path);
    }
}

void CustomAddAvatarWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept(m_addAvatarFrame->geometry());
    m_isDragIn = true;
    update();


    QWidget::dragEnterEvent(event);
}

void CustomAddAvatarWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    m_isDragIn = false;
    update();

    QWidget::dragLeaveEvent(event);
}

void CustomAddAvatarWidget::dropEvent(QDropEvent *event)
{
    auto file = event->mimeData()->urls().first().toLocalFile();

    saveCustomAvatar(file);
    repaint();
}

void CustomAddAvatarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    QColor backColor = Qt::transparent;
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        if (m_isPress || m_isDragIn) {
            backColor = QColor(0, 129, 255, 0.1 * 255);
        } else if (m_isHover) {
            backColor = QColor(0, 129, 255, 0.05 * 255);
        } else {
            backColor = Qt::transparent;
        }
        pen.setColor(QColor(0, 0, 0, 0.2 * 255));
    } else {
        if (m_isPress || m_isDragIn) {
            backColor = QColor(255, 255, 255, 0.1 * 255);
        } else if (m_isHover) {
            backColor = QColor(255, 255, 255, 0.05 * 255);
        } else {
            backColor = Qt::transparent;
        }
        pen.setColor(QColor(255, 255, 255, 255 * 0.2));
    }

    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    QPainterPath path;
    path.addRoundedRect(m_addAvatarFrame->geometry(), WINDOW_ROUND_SIZE, WINDOW_ROUND_SIZE);
    painter.fillPath(path, backColor);
    painter.drawPath(path);
    QRect iconRect = 
        {m_addAvatarFrame->mapToParent(m_addAvatarIconSpacer->geometry().topLeft()), m_addAvatarIconSpacer->geometry().size()};
    m_addAvatarDciIcon.paint(&painter, iconRect, qApp->devicePixelRatio(), DDciIcon::Light, DDciIcon::Normal, 
        Qt::AlignCenter, DDciIconPalette::fromQPalette(palette()));
}

bool CustomAddAvatarWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_addAvatarFrame) {
        if (event->type() == QEvent::Enter) {
            m_isHover = true;
        } else if (event->type() == QEvent::Leave) {  
            m_isHover = false;
            m_isPress = false;
        } else if (event->type() == QEvent::MouseButtonPress) {
            m_isPress = true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_isPress = false;
            saveCustomAvatar(QString());
        } else {
            return false;
        }

        update();
        return true;
    }

    return false;
}

CustomAvatarView::CustomAvatarView(QWidget *parent)
    : QWidget(parent)
    , m_autoExitTimer(new QTimer(this))
    , m_cropBox(new AvatarCropBox(this))
{
    setFixedSize(BORDER_BOX_SIZE, BORDER_BOX_SIZE);

    m_autoExitTimer->setInterval(1000);
    m_autoExitTimer->setSingleShot(true);
    connect(m_autoExitTimer, &QTimer::timeout, this, [this]() {
        // 当用户退出图片修改后，1s内不再编辑，恢复背景
        m_cropBox->setBackgroundColor(palette().color(QPalette::Window));
        auto path = getCroppedImage();
        m_autoExitTimer->stop();

        if (!path.isEmpty()) {
            Q_EMIT requestSaveCustomAvatar(path);
        }
    });

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_cropBox);
    setLayout(layout);
}

QString CustomAvatarView::getCroppedImage()
{
    auto screen = qApp->primaryScreen();
    auto offset = (BORDER_BOX_SIZE - CROP_BOX_SIZE) / 2;
    QPoint pos = mapToGlobal(QPoint(offset, offset));
    QRect rect(pos.x(), pos.y(), CROP_BOX_SIZE, CROP_BOX_SIZE);

    QFileInfo customAvatarPath(m_path);
    const QString tempPath = QString("%1/%2").arg(QDir::tempPath()).arg(customAvatarPath.fileName());

    // 修改后的头像直接覆盖之前的头像
    bool ret =
            screen->grabWindow(0, rect.x(), rect.y(), rect.width(), rect.height()).save(tempPath);
    if (!ret) {
        qWarning() << "failed to save crop image";
        return QString();
    }

    return tempPath;
}

void CustomAvatarView::setAvatarPath(const QString &avatarPath)
{
    m_path = avatarPath;
    auto isValid = !avatarPath.isEmpty();
    m_image = isValid ? m_image : QImage();

    if (isValid) {
        m_image.load(m_path);
    }

    onPresetImage();
    Q_EMIT enableAvatarScaledItem(isValid);
    update();
}

CustomAvatarView::~CustomAvatarView()
{
    if (m_autoExitTimer) {
        m_autoExitTimer->stop();
        m_autoExitTimer->deleteLater();
        m_autoExitTimer = nullptr;
    }
}

void CustomAvatarView::paintEvent(QPaintEvent *event)
{
    // 绘制样式
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (m_image.isNull()) {
        painter.setBrush(QBrush(QColor("#e4e4e4")));
        painter.setPen(QColor("#e4e4e4"));
        painter.drawRoundedRect(QRect(35, 35, CROP_BOX_SIZE, CROP_BOX_SIZE),
                                WINDOW_ROUND_SIZE,
                                WINDOW_ROUND_SIZE);
        return QWidget::paintEvent(event);
    }

    // 平移
    painter.translate(this->width() / 2.0 + m_xPtInterval, this->height() / 2.0 + m_yPtInterval);

    // 缩放
    painter.scale(m_zoomValue, m_zoomValue);

    // 绘制图像
    QRect picRect(-AVATAR_ICON_SIZE / 2, -AVATAR_ICON_SIZE / 2, AVATAR_ICON_SIZE, AVATAR_ICON_SIZE);
    painter.drawImage(picRect, m_image);
}

void CustomAvatarView::mousePressEvent(QMouseEvent *event)
{
    if (m_image.isNull()) {
        return event->ignore();
    }

    m_OldPos = event->pos();
    startAvatarModify();
}

void CustomAvatarView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_Pressed)
        return QWidget::mouseMoveEvent(event);

    this->setCursor(Qt::SizeAllCursor);
    QPoint pos = event->pos();

    int xPtInterval = pos.x() - m_OldPos.x();
    int yPtInterval = pos.y() - m_OldPos.y();

    m_OldPos = pos;

    // 限制图片移动区域,超出移动范围不移动
    m_offset = (AVATAR_ICON_SIZE * m_zoomValue - CROP_BOX_SIZE) / 2;
    if ((m_xPtInterval >= m_offset && xPtInterval > 0)
        || (m_xPtInterval <= -m_offset && xPtInterval < 0)
        || (m_yPtInterval >= m_offset && yPtInterval > 0)
        || (m_yPtInterval <= -m_offset && yPtInterval < 0)) {
        return this->update();
    }

    m_xPtInterval += xPtInterval;
    m_yPtInterval += yPtInterval;

    this->update();
}

void CustomAvatarView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    endAvatarModify();
    this->setCursor(Qt::ArrowCursor);
}

void CustomAvatarView::startAvatarModify()
{
    m_Pressed = true;
    m_cropBox->setBackgroundColor(QColor(0, 0, 0, 100));

    if (m_autoExitTimer->isActive()) {
        m_autoExitTimer->stop();
    }
}

void CustomAvatarView::endAvatarModify()
{
    m_Pressed = false;
    if (!m_autoExitTimer->isActive()) {
        m_autoExitTimer->start();
    }
}

void CustomAvatarView::stopAutoExitTimer()
{
    m_autoExitTimer->stop();
}

void CustomAvatarView::onZoomInImage(void)
{
    m_zoomValue += 0.2;
    this->update();
}

void CustomAvatarView::setZoomValue(const int value)
{
    if (m_image.isNull()) {
        return;
    }

    if (value > m_currentScaledValue)
        onZoomInImage();
    else
        onZoomOutImage();

    m_currentScaledValue = value;

    this->update();
}

void CustomAvatarView::onZoomOutImage(void)
{
    // 限制图片缩放区域,超出缩放范围不缩放
    m_offset = (AVATAR_ICON_SIZE * m_zoomValue - CROP_BOX_SIZE) / 2;
    if (m_zoomValue <= 1.0 || m_offset == CROP_BOX_SIZE / 2) {
        return;
    }

    m_zoomValue -= 0.2;

    this->update();
}

// 还原图片大小
void CustomAvatarView::onPresetImage(void)
{
    m_cropBox->setBackgroundColor(palette().color(QPalette::Window));
    m_zoomValue = 1.0;
    m_xPtInterval = 0;
    m_yPtInterval = 0;
    this->update();
}

CustomAvatarWidget::CustomAvatarWidget(User *user, const int &role, QWidget *parent)
    : AvatarListFrame(user, role, parent)
    , m_avatarScaledItem(new DSlider(Qt::Horizontal, this))
    , m_avatarView(new CustomAvatarView(this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mainLayout->setContentsMargins(10, 2, 10, 2);

    m_avatarScaledItem->setEnabled(false);
    m_avatarScaledItem->setMinimum(SLIDER_MINIMUM_SIZE);
    m_avatarScaledItem->setMaximum(SLIDER_MAX_SIZE);
    m_avatarScaledItem->setPageStep(SLIDER_STEP);

    QHBoxLayout *iconLabelLayout = new QHBoxLayout();
    iconLabelLayout->addWidget(m_avatarView);
    QHBoxLayout *scaledItemLayout = new QHBoxLayout();
    m_avatarScaledItem->setFixedWidth(150);
    scaledItemLayout->addWidget(m_avatarScaledItem);
    mainLayout->addLayout(iconLabelLayout);
    mainLayout->addLayout(scaledItemLayout);

    QHBoxLayout *avatarLayout = new QHBoxLayout();
    avatarLayout->addWidget(getCurrentListView(), Qt::AlignCenter);
    mainLayout->addLayout(avatarLayout);

    connect(m_avatarScaledItem, &DSlider::valueChanged, m_avatarView, [this](int value) {
        m_avatarView->setZoomValue(value);
    });
    connect(m_avatarScaledItem, &DSlider::sliderPressed, m_avatarView, [this] {
        m_avatarView->startAvatarModify();
    });
    connect(m_avatarScaledItem, &DSlider::sliderReleased, m_avatarView, [this] {
        m_avatarView->endAvatarModify();
    });
    connect(m_avatarView,
            &CustomAvatarView::enableAvatarScaledItem,
            this,
            &CustomAvatarWidget::enableAvatarScaledItem);

    setLayout(mainLayout);
}

void CustomAvatarWidget::enableAvatarScaledItem(bool enabled)
{
    m_avatarScaledItem->setEnabled(enabled);
    m_avatarScaledItem->setValue(SLIDER_MINIMUM_SIZE);
}

void CustomAvatarWidget::stopAvatarModify()
{
    m_avatarView->stopAutoExitTimer();
}
