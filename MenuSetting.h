#ifndef MENUSETTING_H
#define MENUSETTING_H
#include <ElaSlider.h>

#include "ElaScrollPage.h"
#include "GLCore.h"


class MenuSetting : public ElaScrollPage{
    Q_OBJECT
public:
    Q_INVOKABLE explicit MenuSetting(GLCore* glCore = nullptr, QWidget* parent = nullptr);

    ~MenuSetting();

private:
    ElaSlider* _fpsSlider;
    GLCore* _glCore{nullptr};

};



#endif //MENUSETTING_H
