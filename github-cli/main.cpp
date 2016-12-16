#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QString>

#include <github/githubfetch.h>

const char* const application_identifier = "HBirchtree-Qthub-App";

const char* github_token = nullptr;
GithubFetch *m_github_daemon;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption("api-token", "Github API token",
                                        "[API key]", "0x0"));

    parser.process(app.arguments());

    auto api_key_store = parser.value("api-token").toStdString();
    github_token = api_key_store.c_str();

    qDebug() << parser.optionNames();

    m_github_daemon = new GithubFetch(application_identifier, &app);

    QObject::connect(m_github_daemon, &GithubFetch::contentNotFound,
                     [&](){
        qDebug() << "Failed to locate resource!";
    });

    m_github_daemon->fetchRepo("hbirchtree/coffeecutie");

    QEventLoop ev;

    return ev.exec();
}
