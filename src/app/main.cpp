// 文件说明：应用程序入口，负责解析软件类型并委托初始化器完成装配。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include <cstdlib>
#include <QApplication>
#include <QSurfaceFormat>

#include <QVTKOpenGLNativeWidget.h>

#include "../communication/datasource/StateSample.h"
#include "../contracts/UiAction.h"
#include "../contracts/LogicNotification.h"
#include "../app/software/RedisSoftwareResolver.h"
#include "../app/software/SoftwareInitializerFactory.h"
#include "../shell/MainWindow.h"

int main(int argc, char* argv[]) {
    // 为 VTK 与 Qt 的 OpenGL 集成设置默认渲染格式
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);
    app.setApplicationName("medCore");
    app.setApplicationVersion("1.0.0");

    // 注册跨线程信号槽所需的自定义元类型
    qRegisterMetaType<UiAction>("UiAction");
    qRegisterMetaType<LogicNotification>("LogicNotification");
    qRegisterMetaType<StateSample>("StateSample");

    // 1. 解析软件类型；若 Redis 不可用则回退到默认配置
    RedisSoftwareResolver resolver;
    QString softwareType = resolver.resolve("medcore.config");

    auto softwareInitializer = SoftwareInitializerFactory::create(softwareType);
    if (!softwareInitializer)
        softwareInitializer = SoftwareInitializerFactory::create(QString());

    const SoftwareBootstrapResult bootstrap = softwareInitializer->build(app);
    if (!bootstrap.isValid())
        return EXIT_FAILURE;

    bootstrap.mainWindow->show();

    return app.exec();
}
