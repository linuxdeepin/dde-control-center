#include "addialog.h"

#include <QImageReader>
#include <QVBoxLayout>
#include <DThemeManager>
#include <QDebug>

using namespace dcc;
using namespace dcc::accounts;

ADDialog::ADDialog(QWidget *parent)
    : DDialog(parent)
    , m_server(new DLineEdit)
    , m_username(new DLineEdit)
    , m_password(new DPasswordEdit)
{
    initUI();
}

void ADDialog::setUserModel(UserModel *model)
{
    m_model = model;

#ifdef DCC_ENABLE_ADDOMAIN
    addButton(model->isJoinADDomain() ? tr("Exit domain") : tr("Join domain"), true);
#endif
}

void ADDialog::initUI()
{
    QPixmap pixmap;
    QImageReader reader;
    reader.setFileName(":/accounts/themes/dark/icons/default_avatar.png");
    if (reader.canRead()) {
        reader.setScaledSize(QSize(64, 64));
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(devicePixelRatio());
    }
    setIcon(QIcon(pixmap));

    m_server->setPlaceholderText(tr("Domain Server"));
    m_username->setPlaceholderText(tr("Admin account"));
    m_password->setPlaceholderText(tr("Password"));

    addContent(m_server);
    addContent(m_username);
    addContent(m_password);

    addButton(tr("Cancel"));

    setSpacing(10);

    connect(this, &ADDialog::buttonClicked, this, [=] (int index, const QString &text) {
        Q_UNUSED(text);

        switch (index) {
        case Submit:
            emit requestInfos(m_server->text(), m_username->text(), m_password->text());
            break;
        default:
            break;
        }
    });
}
