#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QString>

#include <future>
#include <functional>
#include <iostream>

#include <github/githubfetch.h>

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

struct ProcessingContext
{
    /* Objects */
    GithubFetch* github_daemon;

    /* Slots */
    std::function<void(GithubRepo*)> list_repo;
    std::function<void(GithubRepo*,GithubRelease*)> list_release;
    std::function<void(GithubRepo*,GithubTag*)> list_tag;

    std::function<void(GithubUser*)> show_user;
    std::function<void(GithubRepo*)> show_repo;
    std::function<void(GithubRepo*,GithubRelease*)> show_release;
    std::function<void(GithubRepo*,GithubTag*)> show_tag;

    std::function<void(GithubUser*)> get_user_repos;
    std::function<void(GithubRepo*)> get_repo_releases;
    std::function<void(GithubRepo*)> get_repo_tags;

    std::function<void(GithubRepo*,GithubRelease*)> delete_release;
};

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
    QEventLoop ev;

    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption(api_token_str,
                                        "Github API token",
                                        "<API key>"));
    parser.addOption(QCommandLineOption(filter_str,
                                        "Filter for resources [releases, tags, files]",
                                        "<^.*[0-9a-z]+.*$>"));
    parser.addOption(QCommandLineOption(separator_str,
                                        "Separator for output data",
                                        "<|>"));

    parser.addPositionalArgument(
                action_str,
                "Action to perform [list, show, delete, push]",
                "[action]");
    parser.addPositionalArgument(
                category_str,
                "Category to perform action on "
                "[user, repository, tag, release, release-file]",
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

    /* Listing functions */
    auto list_repo = [&](GithubRepo* r)
    {
        std::cout << r->name().toStdString() << std::endl;
    };
    auto list_release = [&](GithubRepo* r, GithubRelease* rl)
    {
        std::cout << rl->id() << " "
                  << r->name().toStdString() << ":" << rl->tagName().toStdString()
                  << std::endl;
    };
    auto list_tag = [&](GithubRepo* r, GithubTag* tag)
    {
        std::cout << r->name().toStdString() << ":" << tag->name().toStdString()
                  << " > " << tag->commit().toStdString()
                  << std::endl;
    };

    /* Show functions */
    auto show_user = [&](GithubUser* u)
    {
        std::cout
                << u->login().toStdString() << separator.toStdString()
                << u->name().toStdString() << separator.toStdString()
                << u->registered().toString().toStdString() << separator.toStdString()
                << std::endl;
    };
    auto show_repo = [&](GithubRepo* r)
    {
        std::cout
                << r->name().toStdString() << separator.toStdString()
                << r->description().toStdString() << separator.toStdString()
                << r->language().toStdString() << separator.toStdString()
                << r->created().toString().toStdString() << separator.toStdString()
                << std::endl;
    };
    auto show_release = [&](GithubRepo* repo, GithubRelease* r)
    {
        std::cout
                << r->id() << separator.toStdString()
                << r->name().toStdString() << separator.toStdString()
                << r->tagName().toStdString() << separator.toStdString()
                << std::endl;
    };
    auto show_tag = [&](GithubRepo* repo, GithubTag* tag)
    {
        std::cout
                << tag->name().toStdString() << separator.toStdString()
                << tag->commit().toStdString() << separator.toStdString()
                << tag->tarballUrl().toString().toStdString() << separator.toStdString()
                << std::endl;
    };

    /* Deleting resources */
    auto delete_release = [&](GithubRepo* repo, GithubRelease* rl)
    {
//        github_daemon.requestDelete(rl);
        list_release(repo, rl);
    };

    /* Recursion functions */
    auto get_repo_releases = [&](GithubRepo* repo)
    {
        github_daemon.fetchAllReleases(repo);
    };
    auto get_repo_tags = [&](GithubRepo* repo)
    {
        github_daemon.fetchAllTags(repo);
    };
    auto get_user_repos = [&](GithubUser* user)
    {
        github_daemon.fetchAllRepositories(user);
    };

    /* Function pointers and etc. to pass on to processing */
    ProcessingContext ctxt = {
        &github_daemon,

        list_repo,
        list_release,
        list_tag,

        show_user,
        show_repo,
        show_release,
        show_tag,

        get_user_repos,
        get_repo_releases,
        get_repo_tags,

        delete_release
    };

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
        qDebug().noquote() << QString("Downloading resource: %1 (%2/%3)")
                              .arg(dl).arg(curr).arg(tot);
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

    return ev.exec();
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

    if(action == "list")
    {
        if(category == "repository")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.list_repo);
            QObject::connect(c.github_daemon, &GithubFetch::userUpdated,
                             c.get_user_repos);
            c.github_daemon->fetchUser(item);
        }else if(category == "release")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::releaseUpdated,
                             [&](GithubRepo* r, GithubRelease* rl)
            {
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
                if(tag->name().contains(filter_rgx))
                    c.list_tag(r, tag);
            });
            c.github_daemon->fetchRepo(item);
        }else
        {
            parser.showHelp(1);
        }
    }else if(action == "show")
    {
        filter_rgx = QRegExp(item2);

        if(category == "repository")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.show_repo);
            c.github_daemon->fetchRepo(item);
        }else if(category == "release")
        {
            QObject::connect(c.github_daemon, &GithubFetch::repoUpdated,
                             c.get_repo_releases);
            QObject::connect(c.github_daemon, &GithubFetch::releaseUpdated,
                             [&](GithubRepo* r, GithubRelease* rel)
            {
                if(rel->tagName().contains(filter_rgx))
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
                if(tag->name().contains(filter_rgx))
                    c.show_tag(r, tag);
            });
            c.github_daemon->fetchRepo(item);
        }else if(category == "user")
        {
            QObject::connect(c.github_daemon, &GithubFetch::userUpdated,
                             c.show_user);
            c.github_daemon->fetchUser(item);
        }else
        {
            parser.showHelp(1);
        }
    }else if(action == "delete")
    {
        if(item2.isEmpty())
        {
            qDebug() << "Invalid filter, will not proceed";
            QCoreApplication::exit();
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
        }else
        {
            parser.showHelp(1);
        }
    }else if(action == "push")
    {
        QCoreApplication::exit();
    }else{
        parser.showHelp();
    }
}
