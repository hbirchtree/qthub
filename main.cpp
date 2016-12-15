#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "github/githubfetch.h"

#include <QtQml/QtQml>
#include <QtQml/QJSValue>
#include <QtQml/QJSEngine>

const char* github_token = nullptr;
GithubFetch *m_github_daemon;
QQmlApplicationEngine* m_engine;

QObject* GenGithubDaemon(QQmlEngine*, QJSEngine*)
{
    GithubFetch* obj = new GithubFetch();

    if(github_token)
        obj->authenticate(github_token);

    m_engine->connect(m_engine, &QQmlApplicationEngine::destroyed,
                      obj, &GithubFetch::killAll);

    return obj;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    if(argc >= 2)
        github_token = argv[1];

    qmlRegisterType<GithubFetch>("Qthub", 1, 0, "QthubDaemon");
    qmlRegisterType<GithubRepo>("Qthub", 1, 0, "QthubRepo");
    qmlRegisterType<GithubUser>("Qthub", 1, 0, "QthubUser");
    qmlRegisterType<GithubRelease>("Qthub", 1, 0, "QthubRelease");

    qmlRegisterSingletonType<GithubFetch>("Qthub.daemon", 1, 0,
                                          "HubDaemon", GenGithubDaemon);

    QQmlApplicationEngine engine;
    m_engine = &engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
