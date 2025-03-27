#include "MenuSetting.h"

#include "ElaScrollPageArea.h"
#include "ElaSlider.h"
#include "ElaText.h"
#include <QHBoxLayout>
#include <QTimer>

#include "ConfigManager.h"
#include "GLCore.h"
#include "ElaTheme.h"

class ElaScrollPageArea;
// 设置菜单，目前能调的只有fps
MenuSetting::MenuSetting(GLCore* glCore, QWidget *parent)
    : ElaScrollPage(parent), _glCore(glCore)
{
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=]() {
        if (!parent)
        {
            update();
        }
    });
    // 下面一大块都是fps设置
    ElaScrollPageArea* settingArea = new ElaScrollPageArea(this);
    QHBoxLayout* settingLayout = new QHBoxLayout(settingArea);
    ElaText* settingText = new ElaText(QString("FPS (10-144)  : %1").arg(GLCore::fps), this);
    settingText->setWordWrap(false);
    settingText->setTextPixelSize(15);

    _fpsSlider = new ElaSlider(this);
    _fpsSlider->setMinimum(10);
    _fpsSlider->setMaximum(144);
    _fpsSlider->setValue(GLCore::fps);
    settingLayout->addWidget(settingText);
    // settingLayout->addStretch();
    settingLayout->addWidget(_fpsSlider);
    connect(_fpsSlider, &ElaSlider::valueChanged, this, [=](int value) {
        GLCore::fps = value;
        settingText->setText(QString("FPS (10-144)  : %1").arg(GLCore::fps));
        if (_glCore && _glCore->renderTimer) {
            _glCore->renderTimer->setInterval((1.0 / value) * 1000);
        }
    });
    connect(_fpsSlider, &ElaSlider::sliderReleased, this, [] {
        ConfigManager::getInstance().setFps(GLCore::fps);
    });
    // 下面6行必须有，保证每个设置能正常显示
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Setting");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(settingArea);
    addCentralWidget(centralWidget, true, true, 0);
}
MenuSetting::~MenuSetting()
{

}