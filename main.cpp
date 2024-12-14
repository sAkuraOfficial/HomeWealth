#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QtWidgets/QApplication>
#include <core/core.h>
#include <ui/HomeWealth/HomeWealth.h>
#include <ui/Login/ClientLogin.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // a.setStyle(QStyleFactory::create("fusion"));
    //  a.setStyle(QStyleFactory::create("WindowsVista"));
    // a.setStyle(QStyleFactory::create("windows11"));
    // 打印所有支持的样式
    qDebug() << QStyleFactory::keys();

    // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);    // 使用高 DPI 位图
    // QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); // 启用高 DPI 支持
    
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3); // 使用 OpenGL 3.3+
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    Core *core = new Core();
    ClientLogin *client_login = new ClientLogin(core);
    HomeWealth *w = new HomeWealth(core);
    client_login->show();
    QObject::connect(client_login, &ClientLogin::LoginSuccess, w, &HomeWealth::onLoginSuccess);
    QObject::connect(w, &HomeWealth::backToLogin, client_login, &ClientLogin::show);
    return a.exec();
}
