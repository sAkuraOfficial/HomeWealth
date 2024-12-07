#include <QtWidgets/QApplication>
#include <core/core.h>
#include <ui/HomeWealth/HomeWealth.h>
#include <ui/Login/ClientLogin.h>
#include<QStyleFactory>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setStyle(QStyleFactory::create("fusion"));
     //a.setStyle(QStyleFactory::create("WindowsVista"));
    
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);    // 使用高 DPI 位图
    Core *core = new Core();
    ClientLogin *client_login = new ClientLogin(core);
    HomeWealth *w = new HomeWealth(core);
    client_login->show();
    QObject::connect(client_login, &ClientLogin::LoginSuccess, w, &HomeWealth::onLoginSuccess);
    QObject::connect(w, &HomeWealth::backToLogin, client_login, &ClientLogin::show);
    return a.exec();
}
