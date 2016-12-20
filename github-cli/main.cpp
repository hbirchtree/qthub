#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QString>

#include <future>
#include <iostream>

#include <github/github.h>

#include "lambdas.h"

/* Option strings */
const char* const api_token_str = "api-token";
const char* const filter_str = "filter";
const char* const separator_str = "separator";

const char* const action_str = "action";
const char* const category_str = "category";
const char* const item_str = "item";
const char* const item2_str = "item_sub";

/* Application identifier, change this if you fork this */
const char* const application_identifier = "HBirchtree-Qthub-CLI-App";

static void processInputs(QCommandLineParser& parser,
                          ProcessingContext& c,
                          QString const& action,
                          QString const& category,
                          QString const& item, const QString &item2,
                          QString const& filter);

int main(int argc, char *argv[])
{
    QString separator = "|";

    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption(api_token_str,
                                        "Github API token",
                                        "API key"));
    parser.addOption(QCommandLineOption(filter_str,
                                        "Filter for resources [releases, tags, files]",
                                        "^.*[0-9a-z]+.*$"));
    parser.addOption(QCommandLineOption(separator_str,
                                        "Separator for output data",
                                        "|"));

    parser.addPositionalArgument(
                action_str,
                "Action to perform [list, show, delete, push, pull, merge]\n"
                "Action matrix: \n"

                "list repository [username]\n"
                "list [tag|release"
//                "|pr"
                "] [repo]\n"
                "list asset [repo:release] [filter]\n"
                "list branch [repo] [filter]\n"

                "show [user|repository] [name]\n"
                "show [tag|release|branch"
//                "|pr"
                "] [repo] [filter]\n"
                "show asset [repo] [filter]\n"
//                "show pr [repo] [id]\n"

                "delete [release|tag] [repo] [name]\n"
                "delete asset [repo] [id]\n"
//                "delete pr [repo] [id]\n"

                "push release [repo] [name]\n"
                "push asset [repo] [name]\n"
//                "push pr [repo] [name]\n"

                "pull repository [repo]\n"
                "pull [tag|release|branch] [repo] [name]\n"
                "pull asset [repo] [id]\n"
                ,

                "[action]");
    parser.addPositionalArgument(
                category_str,
                "Category to perform action on "
                "[user|repository|tag|release|asset|pr|issue]",
                "[category]");
    parser.addPositionalArgument(
                item_str,
                "Item within category [id]",
                "[item]");
    parser.addPositionalArgument(
                item2_str,
                "Sub-item [id]",
                "[sub-item]");

    parser.process(app.arguments());

    if(parser.positionalArguments().size() < 3)
    {
        parser.showHelp();
    }

    GithubFetch github_daemon(application_identifier, &app);

    if(!parser.value(api_token_str).isEmpty())
        github_daemon.authenticate(parser.value(api_token_str));

    if(!parser.value(separator_str).isEmpty())
        separator = parser.value(separator_str);

    /* Function pointers and etc. to pass on to processing */
    ProcessingContext ctxt = {};
    PopulateProcessingContext(&ctxt, github_daemon, separator.toStdString());
    ctxt.github_daemon = &github_daemon;

    auto launch_processing = [&]()
    {
        const QString& action = parser.positionalArguments().at(0);
        const QString& category = parser.positionalArguments().at(1);
        const QString& item = parser.positionalArguments().at(2);
        QString item2 = QString();
        if(parser.positionalArguments().size() >= 4)
            item2 = parser.positionalArguments().at(3);
        const QString& filter = parser.value(filter_str);

        processInputs(parser, ctxt, action, category, item, item2, filter);
    };

    /* Error handling */
    QObject::connect(&github_daemon, &GithubFetch::contentNotFound,
                     [&](){
        qDebug() << "Failed to locate resource!";
    });
    QObject::connect(&github_daemon, &GithubFetch::authenticationError,
                     [&](){
        qDebug() << "Failed to authenticate!";
    });
    QObject::connect(&github_daemon, &GithubFetch::reportProgress,
                     [&](QString const& dl, int curr, int tot){
        qDebug().noquote() << QString("resource: %1 (%2/%3)")
                              .arg(dl).arg(curr).arg(tot);
    });
    QObject::connect(&github_daemon, &GithubFetch::downloadSuccess,
                     [&](QUrl const& url, QString const& file)
    {
        qDebug().noquote() << "Success:" << url.toString() << file;
    });
    QObject::connect(&github_daemon, &GithubFetch::downloadFailed,
                     [&](QUrl const& url, QString const& file)
    {
        qDebug().noquote() << "Failure:" << url.toString() << file;
    });

    /* Exit condition */
    QObject::connect(&github_daemon, &GithubFetch::transferCompleted,
                     [&]()
    {
        if(!github_daemon.activeTransfers())
            QCoreApplication::exit();
    });

    /* Synchronize on fetchSelf() */
    QObject::connect(&github_daemon, &GithubFetch::authenticated,
                     launch_processing);
    QObject::connect(&github_daemon, &GithubFetch::authenticationError,
                     launch_processing);

    /* Retrieve own user, we need to know */
    /* We do this to ensure that private repositories are visible
     *  if authorization is provided.
     */
    github_daemon.fetchSelf();

    return app.exec();
}

void processInputs(QCommandLineParser& parser,
                   ProcessingContext& c,
                   QString const& action,
                   QString const& category,
                   QString const& item,
                   QString const& item2,
                   QString const& filter)
{
    static QRegExp filter_rgx(filter);
    static QRegExp filter_asset = QRegExp(item2);

    /* All of these methods are future-dependent
     * Signals and slots allow networking to happen and queueing becomes simple.
     * There is no queueing. All of this is random, no order is determined.
     * Thank C++11 for lambdas.
     */

    if(action == "list")
    {
        if(category == "repository")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.list_repo);
            QObject::connect(c.github_daemon, &GithubFetch::userUpdated,
                             c.get_user_repos);
            c.github_daemon->fetchUser(item);
        }else if(category == "branch")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_branches);
            QObject::connect(c.github_daemon, &GithubFetch::branchUpdated,
                             [&](GithubRepo* r, GithubBranch* branch)
            {
                if(branch->name().contains(filter_rgx))
                    c.list_branch(r, branch);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "release")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::releaseUpdated,
                             [&](GithubRepo* r, GithubRelease* rl)
            {
                /* Filter releases based on their tags */
                if(rl->tagName().contains(filter_rgx))
                    c.list_release(r, rl);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "tag")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_tags);
            QObject::connect(c.github_daemon, &GithubFetch::tagUpdated,
                             [&](GithubRepo* r, GithubTag* tag)
            {
                /* Filter tags based on filter provided, regex */
                if(tag->name().contains(filter_rgx))
                    c.list_tag(r, tag);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "asset")
        {
            QStringList args = item.split(":");
            if(args.size() >= 2)
                filter_rgx = QRegExp(args[1]);
            else
                parser.showHelp(1);

            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             [&](GithubRepo* repo)
            {
                /* We want tags to get commit IDs */
                c.github_daemon->fetchAllTags(repo);
                /* Pass it on to get releases */
                c.get_repo_releases(repo);
            });
            QObject::connect(c.github_daemon, &GithubFetch::assetUpdated,
                             [&](GithubRelease* rel, GithubAsset* asset)
            {
                /* Releases also acquire asset details! */
                if(rel->tagName().contains(filter_rgx))
                    if(asset->name().contains(filter_asset))
                        c.list_asset(rel, asset);
            });
            c.github_daemon->fetchRepo(args[0]);
        }else if(category == "pr")
        {
            qDebug("Implementation needed");
            QCoreApplication::exit();
        }else
        {
            parser.showHelp(1);
        }
    }else if(action == "show")
    {
        if(category == "repository")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.show_repo);
            c.github_daemon->fetchRepo(item);
        }else if(category == "branch")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_branches);
            QObject::connect(c.github_daemon, &GithubFetch::branchUpdated,
                             [&](GithubRepo* r, GithubBranch* branch)
            {
                if(branch->name().contains(filter_asset))
                    c.show_branch(r, branch);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "release")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::releaseUpdated,
                             [&](GithubRepo* r, GithubRelease* rel)
            {
                if(rel->tagName().contains(filter_asset))
                    c.show_release(r, rel);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "tag")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_tags);
            QObject::connect(c.github_daemon, &GithubFetch::tagUpdated,
                             [&](GithubRepo* r, GithubTag* tag)
            {
                if(tag->name().contains(filter_asset))
                    c.show_tag(r, tag);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "user")
        {
            QObject::connect(c.github_daemon, &GithubFetch::userUpdated,
                             c.show_user);
            c.github_daemon->fetchUser(item);
        }else if(category == "asset")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::assetUpdated,
                             [&](GithubRelease* rel, GithubAsset* asset)
            {
                if(QString("%1").arg(asset->id()) == filter_asset.pattern()
                        || asset->name().contains(filter_asset))
                    c.show_asset(rel, asset);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "pr")
        {
            qDebug("Implementation needed");
            QCoreApplication::exit();
        }else
            parser.showHelp(1);
    }else if(action == "delete")
    {
        if(item2.isEmpty())
        {
            qDebug() << "Invalid filter, will not proceed";
            QCoreApplication::exit(1);
        }

        filter_rgx = QRegExp(item2);

        if(category == "release")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::releaseUpdated,
                             [&](GithubRepo* repo, GithubRelease* rel)
            {
                if(rel->tagName().contains(filter_rgx))
                    c.delete_release(repo, rel);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "tag")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_tags);
            QObject::connect(c.github_daemon, &GithubFetch::tagUpdated,
                             [&](GithubRepo* repo, GithubTag* tag)
            {
                if(tag->name().contains(filter_rgx))
                    c.delete_tag(repo, tag);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "asset")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::assetUpdated,
                             [&](GithubRelease* rel, GithubAsset* asset)
            {
                if(QString("%1").arg(asset->id()) == item2
                        || asset->name().contains(filter_rgx))
                    c.delete_asset(rel, asset);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "pr")
        {
            qDebug("Implementation needed");
            QCoreApplication::exit();
        }else
            parser.showHelp(1);
    }else if(action == "push")
    {
        if(item2.isEmpty())
        {
            qDebug() << "Invalid filter, will not proceed";
            QCoreApplication::exit(1);
        }

        filter_rgx = QRegExp(item2);

        if(category == "release")
        {
            qDebug("Implementation needed");
            QCoreApplication::exit();
        }else if(category == "asset")
        {
            qDebug("Implementation needed");
            QCoreApplication::exit();
        }else if(category == "pr")
        {
            qDebug("Implementation needed");
            QCoreApplication::exit();
        }else
            parser.showHelp(1);
    }else if(action == "pull")
    {
        if(category == "asset")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::assetUpdated,
                             [&](GithubRelease* rel, GithubAsset* asset)
            {
                if(QString("%1").arg(asset->id()) == filter_asset.pattern()
                        || asset->name().contains(filter_asset))
                {
                    c.pull_asset(rel, asset);
                }
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "release")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::releaseUpdated,
                             [&](GithubRepo* repo, GithubRelease* rel)
            {
                if(rel->tagName().contains(filter_asset))
                    c.pull_release(repo, rel);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "branch")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_branches);
            QObject::connect(c.github_daemon, &GithubFetch::branchUpdated,
                             [&](GithubRepo* repo, GithubBranch* branch)
            {
                if(branch->name().contains(filter_asset))
                    c.pull_branch(repo, branch);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "tag")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_tags);
            QObject::connect(c.github_daemon, &GithubFetch::tagUpdated,
                             [&](GithubRepo* repo, GithubTag* rel)
            {
                if(rel->name().contains(filter_asset))
                    c.pull_tag(repo, rel);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "repository")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             [&](GithubRepo* repo)
            {
                c.pull_repo(repo);
            });
            c.github_daemon->fetchRepo(item);
        }else
            parser.showHelp(1);
    }else{
        qDebug() << "Invalid action:" << action;
        parser.showHelp(1);
    }
}
