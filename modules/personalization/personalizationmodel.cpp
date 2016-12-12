#include "personalizationmodel.h"
#include "model/thememodel.h"
#include "model/fontmodel.h"
#include "model/fontsizemodel.h"
using namespace dcc;
using namespace dcc::personalization;
PersonalizationModel::PersonalizationModel(QObject *parent)
    : QObject(parent)
{
    m_windowModel    = new ThemeModel(this);
    m_iconModel      = new ThemeModel(this);
    m_mouseModel     = new ThemeModel(this);
    m_standFontModel = new FontModel(this);
    m_monoFontModel  = new FontModel(this);
    m_fontSizeModel  = new FontSizeModel(this);
}

PersonalizationModel::~PersonalizationModel()
{

}
