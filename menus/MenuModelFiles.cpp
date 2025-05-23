#include "MenuModelFiles.h"

#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>

#include "../widgets/ConfirmDialog.h"
#include "../widgets/MessageBox.h"
#include "ElaTheme.h"
#include "ElaText.h"
#include "ElaTableView.h"
#include "ElaPushButton.h"
#include "../GLCore.h"
#include "../widgets/TableViewModel.h"

namespace {
    /**
     * @brief 计算目录大小(递归)
     * @param path 目录路径
     * @return 目录大小(字节)
     */
    qint64 calculateDirSize(const QDir &path) {
        qint64 size = 0;

        // 计算所有文件大小
        for (const QFileInfo &fileInfo: path.entryInfoList(QDir::Files)) {
            size += fileInfo.size();
        }

        // 递归计算子目录
        for (const QString &subDir: path.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QDir subDirPath(path.filePath(subDir));
            size += calculateDirSize(subDirPath);
        }

        return size;
    }
}

MenuModelFiles::MenuModelFiles(GLCore *glCore, QWidget *parent)
    : ElaScrollPage(parent), m_glCore(glCore) {
    setWindowTitle("模型管理");

    initThemeConnection();
    setupTopWidget();
    setupCentralWidget();
    setupBottomWidget();
}

MenuModelFiles::~MenuModelFiles() {
    // 析构函数，不需要额外操作
}

void MenuModelFiles::initThemeConnection() {
    connect(eTheme, &ElaTheme::themeModeChanged, this, [this]() {
        if (!parentWidget()) {
            update();
        }
    });
}

void MenuModelFiles::setupTopWidget() {
    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(10);

    // 添加标题
    ElaText *subtitleText = new ElaText("程序启动时自动从Resources扫描加载的模型列表", this);
    subtitleText->setTextPixelSize(12);
    topLayout->addWidget(subtitleText);

    // 添加到顶部布局
    setCustomWidget(topWidget);
}

void MenuModelFiles::setupCentralWidget() {
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("模型管理");

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 15, 10, 10);

    // 创建表格视图
    m_tableView = new ElaTableView(this);
    m_modelView = new TableViewModel(this);

    // 设置表格属性
    QFont tableHeaderFont = m_tableView->horizontalHeader()->font();
    tableHeaderFont.setPixelSize(16);
    m_tableView->horizontalHeader()->setFont(tableHeaderFont);
    m_tableView->setModel(m_modelView);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setIconSize(QSize(38, 38));
    m_tableView->verticalHeader()->setHidden(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->horizontalHeader()->setMinimumSectionSize(60);
    m_tableView->verticalHeader()->setMinimumSectionSize(46);
    m_tableView->setFixedHeight(450);

    // 自动调整列宽
    connect(m_tableView, &ElaTableView::tableViewShow, this, [this]() {
        m_tableView->setColumnWidth(0, 600);
        m_tableView->setColumnWidth(1, 200);
    });

    QHBoxLayout *tableViewLayout = new QHBoxLayout();
    tableViewLayout->setContentsMargins(0, 0, 10, 0);
    tableViewLayout->addWidget(m_tableView);

    // 扫描并添加模型
    scanAndDisplayModels();
    mainLayout->addLayout(tableViewLayout);

    // 设置为中央控件
    addCentralWidget(centralWidget, true, true, 0);
}

void MenuModelFiles::setupBottomWidget() {
    QWidget *bottomWidget = new QWidget(this);
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(0, -10, 15, 15);
    bottomLayout->setSpacing(10);
    bottomLayout->addStretch();

    // 创建按钮
    ElaPushButton *importButton = new ElaPushButton("导入模型", this);
    ElaPushButton *deleteButton = new ElaPushButton("删除模型", this);

    // 连接信号
    connect(importButton, &ElaPushButton::clicked, this, &MenuModelFiles::importModel);
    connect(deleteButton, &ElaPushButton::clicked, this, [this]() {
        // 获取当前选中的行
        QModelIndex currentIndex = m_tableView->currentIndex();
        if (!currentIndex.isValid()) {
            MessageBox *messageBox = new MessageBox("请先选择要删除的模型");
            messageBox->show();
            messageBox->setIsStayTop(true);
            return;
        }

        // 获取模型名称
        QString modelName = m_modelView->data(m_modelView->index(currentIndex.row(), 0), Qt::DisplayRole).toString();

        // 创建确认对话框
        if (!m_confirmDialog) {
            m_confirmDialog = new ConfirmDialog();
        }

        m_confirmDialog->setConfirmButtonText("删除");
        m_confirmDialog->setCancelButtonText("取消");
        m_confirmDialog->setFixedSize(400, 200);
        m_confirmDialog->moveToCenter();

        QString message = QString("确定要删除模型 \"%1\" 吗？\n此操作不可恢复！").arg(modelName);
        m_confirmDialog->setMessageText(message);
        m_confirmDialog->show();
        m_confirmDialog->setIsStayTop(true);
    });

    // 配置确认对话框
    if (!m_confirmDialog) {
        m_confirmDialog = new ConfirmDialog();
        m_confirmDialog->hide();
    }

    connect(m_confirmDialog->m_confirm_button(), &ElaPushButton::clicked, this, [this]() {
        this->deleteSelectedModel();
        m_confirmDialog->hide();
    });

    connect(m_confirmDialog->m_cancel_button(), &ElaPushButton::clicked, this, [this]() {
        m_confirmDialog->hide();
    });

    // 添加按钮到布局
    bottomLayout->addWidget(importButton);
    bottomLayout->addWidget(deleteButton);

    QVBoxLayout *mainLayout = findChild<QVBoxLayout *>();
    if (mainLayout) {
        mainLayout->addWidget(bottomWidget);
    }
}

int MenuModelFiles::scanAndDisplayModels() {
    // 获取Resources目录
    QDir resourcesDir("Resources");
    if (!resourcesDir.exists()) {
        QStringList data;
        data << "Resources目录不存在!" << "请检查路径";
        m_modelView->addModel(data);
        return 0;
    }

    // 获取所有子目录
    QStringList subdirs = resourcesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (subdirs.isEmpty()) {
        QStringList data;
        data << "没有找到模型文件!" << "请检查路径";
        m_modelView->addModel(data);
        return 0;
    }

    int modelCount = 0;
    // 检查每个子目录中是否有model3.json文件
    for (const QString &subdir: subdirs) {
        QDir modelDir(resourcesDir.filePath(subdir));
        QStringList jsonFiles = modelDir.entryList(QStringList() << "*.model3.json", QDir::Files);

        // 如果找到model3.json文件，则添加到列表
        if (!jsonFiles.isEmpty()) {
            QStringList data;
            qint64 size = calculateDirSize(modelDir);
            QString modelSize = QString::number(size / 1024.0 / 1024.0, 'f', 2) + " MB";
            data << subdir << modelSize;
            m_modelView->addModel(data);
            modelCount++;
        }
    }

    return modelCount;
}

void MenuModelFiles::deleteSelectedModel() {
    // 获取当前选中的行
    QModelIndex currentIndex = m_tableView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }

    // 获取模型名称
    QString modelName = m_modelView->data(m_modelView->index(currentIndex.row(), 0), Qt::DisplayRole).toString();

    // 删除模型文件夹
    QString modelPath = "Resources/" + modelName;
    QDir modelDir(modelPath);

    if (modelDir.removeRecursively()) {
        // 删除成功，从表格中移除
        m_modelView->removeModel(modelName);

        // 更新GLCore中的模型列表
        if (m_glCore) {
            m_glCore->scanAndLoadModels();
        }

        MessageBox *messageBox = new MessageBox(QString("模型 \"%1\" 已删除").arg(modelName));
        messageBox->show();
        messageBox->setIsStayTop(true);
    } else {
        MessageBox *messageBox = new MessageBox(QString("无法删除模型 \"%1\"，请检查权限或文件是否存在").arg(modelName));
        messageBox->show();
        messageBox->setIsStayTop(true);
    }
}

void MenuModelFiles::importModel() {
    // 使用系统原生文件对话框选择文件夹
    QString selectedDir = QFileDialog::getExistingDirectory(
        this,
        "选择模型文件夹",
        QCoreApplication::applicationDirPath(), // 起始目录
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (selectedDir.isEmpty()) {
        return; // 用户取消选择
    }

    // 检查选择的文件夹是否包含model3.json文件
    QDir modelDir(selectedDir);
    QStringList jsonFiles = modelDir.entryList(QStringList() << "*.model3.json", QDir::Files);

    if (jsonFiles.isEmpty()) {
        MessageBox *messageBox = new MessageBox("所选文件夹不是有效的模型文件夹，请确保文件夹中包含xxx.model3.json文件");
        messageBox->show();
        messageBox->setIsStayTop(true);
        return;
    }

    // 获取文件夹名称并检查是否已存在
    QFileInfo dirInfo(selectedDir);
    QString folderName = dirInfo.fileName();
    QDir resourcesDir("Resources");

    if (!resourcesDir.exists()) {
        resourcesDir.mkpath(".");
    }

    QString destPath = resourcesDir.filePath(folderName);
    QDir destDir(destPath);

    if (destDir.exists()) {
        MessageBox *messageBox = new MessageBox(QString("模型 \"%1\" 已存在").arg(folderName));
        messageBox->show();
        messageBox->setIsStayTop(true);
        return;
    }

    // 复制文件夹到Resources目录
    if (copyDir(selectedDir, destPath)) {
        // 计算模型大小
        qint64 size = calculateDirSize(destDir);
        QString modelSize = QString::number(size / 1024.0 / 1024.0, 'f', 2) + " MB";

        // 添加到表格模型
        QStringList modelData;
        modelData << folderName << modelSize;
        m_modelView->addModel(modelData);

        // 更新GLCore中的模型列表
        if (m_glCore) {
            m_glCore->scanAndLoadModels();
        }

        MessageBox *messageBox = new MessageBox(QString("模型 \"%1\" 导入成功").arg(folderName));
        messageBox->show();
        messageBox->setIsStayTop(true);
    } else {
        MessageBox *messageBox = new MessageBox(QString("导入模型 \"%1\" 失败").arg(folderName));
        messageBox->show();
        messageBox->setIsStayTop(true);
    }
}

bool MenuModelFiles::copyDir(const QString &source, const QString &destination) {
    QDir sourceDir(source);
    if (!sourceDir.exists()) {
        return false;
    }

    QDir destDir(destination);
    if (!destDir.exists()) {
        destDir.mkpath(".");
    }

    // 复制文件
    QStringList files = sourceDir.entryList(QDir::Files);
    for (const QString &file: files) {
        QString srcName = sourceDir.filePath(file);
        QString destName = destDir.filePath(file);
        if (!QFile::copy(srcName, destName)) {
            qWarning() << "复制文件失败:" << srcName << "到" << destName;
            return false;
        }
    }

    // 递归复制子目录
    QStringList dirs = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (const QString &dir: dirs) {
        QString srcName = sourceDir.filePath(dir);
        QString destName = destDir.filePath(dir);
        if (!copyDir(srcName, destName)) {
            return false;
        }
    }

    return true;
}
