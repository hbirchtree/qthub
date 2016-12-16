#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QString>

#include <github/githubfetch.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/QtQml>
#include <QtQml/QJSValue>
#include <QtQml/QJSEngine>

const char* const application_identifier = "HBirchtree-Qthub-App";

const char* github_token = nullptr;
QQmlApplicationEngine* m_engine;

QObject* GenGithubDaemon(QQmlEngine*, QJSEngine*)
{
    GithubFetch* obj = new GithubFetch(application_identifier);

    if(github_token)
        obj->authenticate(github_token);

    m_engine->connect(m_engine, &QQmlApplicationEngine::destroyed,
                      obj, &GithubFetch::killAll);

    return obj;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption("api-token", "Github API token",
                                        "[API key]"));

    parser.process(app.arguments());

    auto api_key_store = parser.value("api-token").toStdString();
    if(api_key_store.size() > 0)
        github_token = api_key_store.c_str();

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
