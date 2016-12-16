#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QString>

#include <future>
#include <iostream>

#include <github/githubfetch.h>

/* Option strings */
const char* const api_token_str = "api-token";
const char* const action_str = "action";
const char* const category_str = "category";
const char* const item_str = "item";

/* Application identifier, change this if you fork this */
const char* const application_identifier = "HBirchtree-Qthub-CLI-App";

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption(api_token_str, "Github API token",
                                        "[API key]"));

    parser.addPositionalArgument(
                action_str,
                "Action to perform [list, delete, push]",
                "[action]");
    parser.addPositionalArgument(
                category_str,
                "Category to perform action on [user, repository, tag, release, release-file]",
                "[category]");
    parser.addPositionalArgument(
                item_str,
                "Item within category [id]",
                "[item]");

    parser.process(app.arguments());

    GithubFetch github_daemon(application_identifier, &app);

    if(!parser.value(api_token_str).isEmpty())
    {
        github_daemon.authenticate(parser.value(api_token_str));
    }

    if(parser.positionalArguments().size() != 3)
    {
        parser.showHelp();
        QCoreApplication::exit();
    }

    /* Listing functions */
    auto list_repo = [&](GithubRepo* r)
    {
        std::cout << r->name().toStdString() << std::endl;
        if(!github_daemon.activeTransfers())
            QCoreApplication::exit();
    };
    auto list_release = [&](GithubRepo*, GithubRelease* rl)
    {
        std::cout << rl->id() << " " << rl->tagName().toStdString() << std::endl;
        if(!github_daemon.activeTransfers())
            QCoreApplication::exit();
    };

    /* Recursion functions */
    auto get_repo_releases = [&](GithubRepo* repo)
    {
        github_daemon.fetchAllReleases(repo);
    };
    auto get_user_repos = [&](GithubUser* user)
    {
        github_daemon.fetchAllRepositories(user);
    };

    QObject::connect(&github_daemon, &GithubFetch::contentNotFound,
                     [&](){
        qDebug() << "Failed to locate resource!";
        QCoreApplication::exit();
    });
    QObject::connect(&github_daemon, &GithubFetch::authenticationError,
                     [&](){
        qDebug() << "Failed to authenticate!";
        QCoreApplication::exit();
    });
//    QObject::connect(&github_daemon, &GithubFetch::reportProgress,
//                     [&](QString const& dl, int curr, int tot){
//        qDebug().noquote() << QString("Downloading resource: %1 (%2/%3)")
//                              .arg(dl).arg(curr).arg(tot);
//    });

    const QString& action = parser.positionalArguments().at(0);
    const QString& category = parser.positionalArguments().at(1);
    const QString& item = parser.positionalArguments().at(2);

    if(action == "list")
    {
        if(category == "repository")
        {
            QObject::connect(&github_daemon, &GithubFetch::repoUpdated,
                             list_repo);
            QObject::connect(&github_daemon, &GithubFetch::userUpdated,
                             get_user_repos);
            github_daemon.fetchUser(item);
        }else if(category == "release")
        {
            QObject::connect(&github_daemon, &GithubFetch::repoUpdated,
                             get_repo_releases);
            QObject::connect(&github_daemon, &GithubFetch::releaseUpdated,
                             list_release);
            github_daemon.fetchRepo(item);
        }
    }else if(action == "delete")
    {

    }else if(action == "push")
    {

    }else{
        parser.showHelp();
    }

    QEventLoop ev;

    return ev.exec();
}
