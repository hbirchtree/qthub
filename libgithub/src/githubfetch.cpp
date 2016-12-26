#include <github/github.h>

#ifdef NDEBUG
#define QT_NO_QDEBUG_OUTPUT
#define QT_NO_DEBUG
#endif

#include <QtDebug>

#include <functional>

GithubFetch::GithubFetch(QObject *parent) :
    QObject(parent),
    m_activeTransfers(0),
    m_authenticated(false)
{
    m_netman = new QNetworkAccessManager();

    connect(m_netman, &QNetworkAccessManager::networkAccessibleChanged,
            [&](QNetworkAccessManager::NetworkAccessibility acc)
    {
        statusChanged(acc);
    });
    connect(this, &GithubFetch::selfUpdated,
            [&](GithubUser*)
    {
        this->authenticated();
    });
    connect(this, &GithubFetch::transferCompleted,
            [&]()
    {
        m_activeTransfers--;
    });

    m_apipoint = "https://api.github.com";
}

GithubFetch::GithubFetch(GithubStore* store,
                         QString identifier,
                         QObject *parent) :
    GithubFetch(parent)
{
    m_store = store;
    m_agentstring = identifier;
}

void GithubFetch::fetchUser(const QString &username)
{
    startNetworkRequest(QString("/users/%2")
                        .arg(username),
                        username, GitUser);
}

void GithubFetch::fetchAllRepositories(GithubUser *user)
{
    static const char* const public_repo = "/users/%1/repos";
    static const char* const private_repo = "/user/repos";

    QString path;
    if(m_store->self() == user)
        path = private_repo;
    else
        path = QString(public_repo).arg(user->login());

    startNetworkRequest(path, user->login(), GitAllRepos);
}

void GithubFetch::fetchRepo(const QString &reponame)
{
    startNetworkRequest(QString("/repos/%1").arg(reponame),
                        reponame,GitRepo);
}

void GithubFetch::fetchAllReleases(GithubRepo *repo)
{
    startNetworkRequest(QString("/repos/%1/releases")
                        .arg(repo->name()),
                        repo->name(),
                        GitAllReleases);
}

void GithubFetch::fetchAllTags(GithubRepo *repo)
{
    startNetworkRequest(QString("/repos/%1/tags")
                        .arg(repo->name()),
                        repo->name(),
                        GitAllTags);
}

void GithubFetch::fetchAllBranches(GithubRepo *repo)
{
    startNetworkRequest(QString("/repos/%1/branches")
                        .arg(repo->name()),
                        repo->name(),
                        GitAllBranches);
}

void GithubFetch::fetchRelease(GithubRepo *repo, quint64 relId)
{
    startNetworkRequest(
                QString("/repos/%1/releases/%2")
                .arg(repo->name()).arg(relId),
                repo->name(),
                GitRelease);
}

void GithubFetch::fetchBranchHead(GithubBranch *branch)
{
    startNetworkRequest(
                QString("/repos/%1/git/refs/heads/%2")
                .arg(branch->repository()->name()).arg(branch->name()),
                QString("%1:%2")
                .arg(branch->repository()->name()).arg(branch->name()),
                GitBranchHead);
}

void GithubFetch::fetchReadme(GithubRepo *repo)
{
    startNetworkRequest(
                QString("/repos/%1/readme")
                .arg(repo->name()),
                repo->name(),
                GitReadme);
}

void GithubFetch::fetchSelf()
{
    if(m_token.size() >= 1)
    {
        startNetworkRequest("/user", ":self", GitUser);
    }else{
        authenticatedNull();
        authenticationError();
    }

    agentStringIdentifier(m_agentstring);
}

void GithubFetch::initProcessing()
{
    /* Most processing starts here */
    fetchSelf();
}

void GithubFetch::killAll()
{
    m_netman->deleteLater();
}

void GithubFetch::registerProgress(qint64 rec, qint64 tot)
{
    QNetworkReply* req = dynamic_cast<QNetworkReply*>(sender());
    if(req)
        reportProgress(req->url().toString(), rec, tot);
}

void GithubFetch::authenticate(const QString &token)
{
    m_token = token;
}
