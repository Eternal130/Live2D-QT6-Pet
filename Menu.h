#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include "ElaWindow.h"
#include "ElaScrollPage.h"
#include "ElaSlider.h"

#include <QMainWindow>

#include "GLCore.h"


class Menu : public ElaWindow{
    Q_OBJECT
public:
    void initWindow(QWidget *parent);

    void initContents();

    Menu(QWidget* parent = nullptr);
    ~Menu();

private:
    ElaScrollPage* _settingPage{nullptr};
    ElaSlider* _fpsSlider{nullptr};
    GLCore* _glCore{nullptr};

};


#endif //SETTINGSMENU_H
