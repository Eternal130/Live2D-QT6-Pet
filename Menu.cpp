#include "Menu.h"

#include "GLCore.h"

#include "ElaText.h"
#include "MenuModelFiles.h"
#include "MenuSetting.h"

//添加了一个菜单，目前只有一个设置菜单，其他的后面再加
Menu::Menu(QWidget *parent)
    : ElaWindow(parent){
    initWindow(parent);
}

void Menu::initWindow(QWidget *parent)
{
    // setIsEnableMica(true);
    // setIsCentralStackedWidgetTransparent(true);
    _glCore = dynamic_cast<GLCore *>(parent);
    resize(1200, 740);
    // ElaLog::getInstance()->initMessageLog(true);
    // eTheme->setThemeMode(ElaThemeType::Dark);
    // setIsNavigationBarEnable(false);
    // setNavigationBarDisplayMode(ElaNavigationType::Compact);
    // setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint, false);
    // setUserInfoCardPixmap(QPixmap(":/Resource/Image/Cirno.jpg"));
    // setUserInfoCardTitle("Desktop Pet");
    // setUserInfoCardSubTitle("Eternal130@outlook.com");
    setUserInfoCardVisible(false);
    setWindowTitle("菜单");
    initContents();
    // setIsStayTop(true);
    // setUserInfoCardVisible(false);
}
void Menu::initContents() {
    _settingPage = new MenuSetting(_glCore,this);
    _modelFilesPage = new MenuModelFiles(_glCore, this);
    addPageNode("Setting", _settingPage, ElaIconType::GearComplex);
    addPageNode("Model Files", _modelFilesPage, ElaIconType::Folder);
}

Menu::~Menu(){}