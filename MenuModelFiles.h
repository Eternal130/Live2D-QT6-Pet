#ifndef MENUMODELFILES_H
#define MENUMODELFILES_H

#include "ElaScrollPage.h"

// 前向声明
class ElaTableView;
class TableViewModel;
class ConfirmDialog;
class ElaPushButton;
class GLCore;

/**
 * @brief 模型管理页面类，用于管理Live2D模型
 */
class MenuModelFiles : public ElaScrollPage {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param glCore OpenGL核心对象指针
     * @param parent 父控件
     */
    Q_INVOKABLE explicit MenuModelFiles(GLCore *glCore = nullptr, QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MenuModelFiles() override;

private slots:
    /**
     * @brief 导入新模型，从文件系统选择模型文件夹并导入
     */
    void importModel();

    /**
     * @brief 删除选中的模型
     */
    void deleteSelectedModel();

private:
    /**
     * @brief 初始化主题变更连接
     */
    void initThemeConnection();

    /**
     * @brief 设置顶部控件
     */
    void setupTopWidget();

    /**
     * @brief 设置中央控件和布局
     */
    void setupCentralWidget();

    /**
     * @brief 设置底部控件
     */
    void setupBottomWidget();

    /**
     * @brief 扫描并显示模型文件
     * @return 返回找到的模型数量
     */
    int scanAndDisplayModels();

    /**
     * @brief 递归复制目录
     * @param source 源目录
     * @param destination 目标目录
     * @return 是否复制成功
     */
    bool copyDir(const QString &source, const QString &destination);

    GLCore *m_glCore{nullptr};          ///< OpenGL核心对象指针
    TableViewModel* m_modelView{nullptr}; ///< 模型视图对象指针
    ElaTableView* m_tableView{nullptr};   ///< 表格视图对象指针
    ConfirmDialog* m_confirmDialog{nullptr}; ///< 确认对话框对象指针
};

#endif // MENUMODELFILES_H