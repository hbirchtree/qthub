#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "githubfetch.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    GithubFetch fetcher;

    fetcher.fetchUser("hbirchtree");
    fetcher.fetchRepo("hbirchtree/coffeecutie");

    return app.exec();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
