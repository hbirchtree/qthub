#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "github/githubfetch.h"

#include <QtQml/QtQml>
#include <QtQml/QJSValue>
#include <QtQml/QJSEngine>

const char* github_token = nullptr;
GithubFetch *m_github_daemon;

QObject* GenGithubDaemon(QQmlEngine* engine, QJSEngine* sengine)
{
    Q_UNUSED(engine);

    GithubFetch* obj = new GithubFetch();

    if(github_token)
        obj->authenticate(github_token);

    return obj;
}

int main(int argc, char *argv[])
{
    if(argc >= 2)
        github_token = argv[1];

    qmlRegisterType<GithubFetch>("Qthub", 1, 0, "QthubDaemon");
    qmlRegisterType<GithubRepo>("Qthub", 1, 0, "QthubRepo");
    qmlRegisterType<GithubUser>("Qthub", 1, 0, "QthubUser");
    qmlRegisterType<GithubRelease>("Qthub", 1, 0, "QthubRelease");

    qmlRegisterSingletonType<GithubFetch>("Qthub.daemon", 1, 0, "HubDaemon", GenGithubDaemon);

    QGuiApplication app(argc, argv);

//    fetcher.connect(&fetcher, &GithubFetch::repoUpdated,
//                    [&](QPointer<GithubRepo> repo)
//    {
//        qDebug() << "Updated repo: " << repo->name();

//        fetcher.fetchAllReleases(repo);
//    });

//    fetcher.fetchUser("hbirchtree");
//    fetcher.fetchRepo("hbirchtree/coffeecutie");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
