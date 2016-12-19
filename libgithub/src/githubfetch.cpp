#include <github/githubfetch.h>
#include <github/githubrepo.h>
#include <github/githubuser.h>
#include <github/githubtag.h>
#include <github/githubrelease.h>
#include <github/githubreleasefile.h>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <functional>

void GithubFetch::updateUser(GithubUser* u, QJsonObject const& o)
{
    u->setId(o["id"].toVariant().toULongLong());

    u->setLogin(o["login"].toString());
    if(!o["name"].isNull())
        u->setName(o["name"].toString());
    if(!o["bio"].isNull())
        u->setBio(o["bio"].toString());
    if(!o["avatar_url"].isNull())
        u->setAvatar(o["avatar_url"].toString());
    if(!o["email"].isNull())
        u->setEmail(o["email"].toString());
    if(!o["created_at"].isNull())
        u->setRegistered(QDateTime::fromString(o["created_at"].toString()));

    u->setFollowers(o["followers"].toVariant().toULongLong());
    u->setFollowing(o["following"].toVariant().toULongLong());

    u->setNumRepos(o["public_repos"].toVariant().toULongLong());
}

void GithubFetch::updateRepo(GithubRepo* r, QJsonObject const& o)
{
    r->setId(o["id"].toVariant().toULongLong());

    r->setName(o["full_name"].toString());
    r->setDescription(o["description"].toString());

    r->setLanguage(o["language"].toString());
    r->setIssues(o["open_issues"].toVariant().toULongLong());
}

void GithubFetch::addRepositories(GithubUser* u, QJsonArray const& repos)
{
    for(int i=0;i<repos.size();i++)
    {
        QJsonObject const& m = repos[i].toObject();
        GithubRepo* r = new GithubRepo(u);

        updateRepo(r, m);

        u->addRepository(r);
        m_repos.insert(r->name(), r);
        repoUpdated(r);
    }
}

void GithubFetch::addReleases(GithubRepo* r, QJsonArray const& rels)
{
    for(int i=0;i<rels.size();i++)
    {
        auto const& m = rels[i].toObject();
        GithubRelease* rl = new GithubRelease(r);

        rl->setId(m["id"].toVariant().toULongLong());

        if(!m["tag_name"].isNull())
            rl->setTagName(m["tag_name"].toString());
        rl->setName(m["name"].toString());
        rl->setAuthor(m["author"].toObject()["login"].toString());

        rl->setDescription(m["body"].toString());
        rl->setDraft(m["draft"].toBool());
        rl->setPrerelease(m["prerelease"].toBool());
        rl->setPublished(m["published_at"].toVariant().toDateTime());

        addAssets(rl, m["assets"].toArray());

        r->addRelease(rl);

        releaseUpdated(r, rl);
    }
}

void GithubFetch::addTags(GithubRepo *u, const QJsonArray &tags)
{
    for(int i=0;i<tags.size();i++)
    {
        auto const& m = tags[i].toObject();
        GithubTag* tg = new GithubTag(u);

        tg->setName(m["name"].toString());
        tg->setCommit(m["commit"].toObject()["sha"].toString());
        tg->setTarballUrl(m["tarball_url"].toString());

        u->addTag(tg);
        tagUpdated(u, tg);
    }
}

void GithubFetch::addAssets(GithubRelease *r, const QJsonArray &assets)
{
    for(int i=0;i<assets.size();i++)
    {
        auto const& m = assets[i].toObject();
        GithubAsset* at = new GithubAsset(r);

        at->setId(m["id"].toVariant().toULongLong());
        at->setName(m["name"].toString());
        at->setUploader(m["uploader"].toObject()["login"].toString());
        at->setLabel(m["label"].toString());
        at->setType(m["content_type"].toString());
        at->setSize(m["size"].toVariant().toULongLong());
        at->setDownloads(m["download_count"].toVariant().toULongLong());
        at->setDownloadUrl(m["browser_download_url"].toVariant().toUrl());

        at->setUpdated(m["updated_at"].toVariant().toDateTime());

        r->addAsset(at);
        assetUpdated(r, at);
    }
}

GithubFetch::GithubFetch(QObject *parent) :
    QObject(parent),
    m_self(nullptr),
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

GithubFetch::GithubFetch(QString identifier, QObject *parent) :
    GithubFetch(parent)
{
    m_agentstring = identifier;

    qDebug() << "Identifying as:" << m_agentstring;
}

void GithubFetch::addToken(QNetworkRequest &req)
{
    req.setRawHeader("User-Agent", m_agentstring.toLocal8Bit());
    if(!m_token.isEmpty())
        req.setRawHeader("Authorization",
                         QString("token %1")
                         .arg(m_token)
                         .toLocal8Bit());
}

void GithubFetch::startNetworkRequest(const QString &url,
                                      QString const& id,
                                      GithubFetch::ReplyType receive,
                                      int page)
{
    QNetworkRequest req;
    req.setUrl(m_apipoint + url + QString("?page=%1").arg(page));
    addToken(req);

    QNetworkReply* rep = m_netman->get(req);
    rep->setProperty("type", receive);
    rep->setProperty("id", id);
    connect(rep, &QNetworkReply::finished,
            this, &GithubFetch::receiveUserData);
    connect(rep, &QNetworkReply::downloadProgress,
            this, &GithubFetch::registerProgress);
    m_activeTransfers++;
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
    if(m_self == user)
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

void GithubFetch::fetchRelease(GithubRepo *repo, quint64 relId)
{
    startNetworkRequest(
                QString("/repos/%1/releases/%2")
                .arg(repo->name()).arg(relId),
                repo->name(),
                GitRelease);
}

void GithubFetch::fetchSelf()
{
    if(m_token.size() >= 1)
    {
        startNetworkRequest("/user", ":self", GitUser);
    }else{
        qWarning() << "Cannot retrieve self, no API token";
        authenticationError();
    }
}

void GithubFetch::requestDelete(GithubRelease *release)
{
    GithubRepo* repo = dynamic_cast<GithubRepo*>(release->parent());
    if(!repo)
        return;

    QNetworkRequest req;
    req.setUrl(m_apipoint + QString("/repos/%1/releases/%2")
               .arg(repo->name()).arg(release->id()));
    addToken(req);

    QNetworkReply* rep = m_netman->deleteResource(req);
    rep->setProperty("type", GitReleaseDelete);
    connect(rep, &QNetworkReply::finished,
            this, &GithubFetch::receiveUserData);
    m_activeTransfers++;
}

void GithubFetch::killAll()
{
    m_netman->deleteLater();
}

void GithubFetch::receiveUserData()
{

    QObject* o = sender();

    if(o)
    {
        QNetworkReply* rep = dynamic_cast<QNetworkReply*>(o);

        /* Check for error */
        switch(rep->error())
        {
        case QNetworkReply::NoError:
            break;
        case QNetworkReply::ContentNotFoundError:
            transferCompleted();
            contentNotFound();
            return;
        case QNetworkReply::InternalServerError:
            if(rep->rawHeader("Status") == "401 Unauthorized")
                authenticationError();
            transferCompleted();
            return;
        case QNetworkReply::ServiceUnavailableError:
            authenticationError();
            transferCompleted();
            return;
        default:
            networkReplyError(rep->error());
            transferCompleted();
            return;
        }

        /* Get the data */
        QByteArray rep_data = rep->readAll();

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(rep_data, &err);
        if(err.error != QJsonParseError::NoError)
        {
            qDebug() << err.errorString();
            return;
        }

        switch(o->property("type").toInt())
        {
        case GitUser:
        {
            auto dobj = doc.object();
            QString obj_id = dobj["login"].toString();
            GithubUser* u = m_users.value(obj_id);

            if(!u)
            {
                u = new GithubUser(this);
                m_users.insert(obj_id, u);
            }

            updateUser(u, doc.object());

            if(o->property("id").toString() == ":self")
            {
                selfUpdated(u);
                m_self = u;
            }
            else
                userUpdated(u);

            break;
        }
        case GitAllRepos:
        {
            GithubUser* u = m_users.value(o->property("id").toString());

            if(u)
                addRepositories(u, doc.array());
            break;
        }
        case GitRepo:
        {
            GithubRepo* r = m_repos.value(o->property("id").toString());

            if(!r)
            {
                r = new GithubRepo(this);
                m_repos.insert(o->property("id").toString(), r);
            }

            updateRepo(r, doc.object());

            repoUpdated(r);
            break;
        }
        case GitAllTags:
        {
            GithubRepo* r = m_repos.value(o->property("id").toString());

            if(r)
                addTags(r, doc.array());

            break;
        }
        case GitAllReleases:
        {
            GithubRepo* r = m_repos.value(o->property("id").toString());


            if(r)
                addReleases(r, doc.array());

            break;
        }
        case GitRelease:
        {
            GithubRepo* r = m_repos.value(o->property("id").toString());

            QJsonArray arr = {doc.object()};
            if(r)
                addReleases(r, arr);

            break;
        }
        case GitReleaseDelete:
        {
            qDebug() << "Deleted tag from " << rep->url() <<
                        " with status " << rep->rawHeader("Status");
            break;
        }
        }

        qDebug().nospace().noquote()
                << "Rate limit: "
                << rep->rawHeader("X-RateLimit-Remaining") << "/"
                << rep->rawHeader("X-RateLimit-Limit");

        /* If data is paginated, get the next pages */
        QString linkHeader = rep->rawHeader("Link");

        if(!linkHeader.isEmpty())
        {
            static QRegExp exp("page=(\\d+)>; rel=\"last\"");
            static QRegExp exp2("page=(\\d+)>; rel=\"next\"");

            if(exp.indexIn(linkHeader) != -1 && exp2.indexIn(linkHeader) != -1)
            {
                if(exp.capturedTexts().size() > 1 && exp2.capturedTexts().size() > 1)
                {
                    int next = exp2.cap(1).toInt();
                    int last = exp.cap(1).toInt();

                    QNetworkRequest req = rep->request();

                    if(next == 2)
                        for(int i=2;i<=last;i++)
                        {
                            startNetworkRequest(
                                        req.url().path().split("?").at(0),
                                        rep->property("id").toString(),
                                        static_cast<ReplyType>(rep->property("type").toInt()),
                                        i);
                        }
                }
            }
        }

        transferCompleted();
    }

}

void GithubFetch::registerProgress(qint64 rec, qint64 tot)
{
    QNetworkReply* req = dynamic_cast<QNetworkReply*>(sender());
    if(req)
        reportProgress(req->url().toString(), rec, tot);
}

void GithubFetch::authenticate(const QString &token)
{
    qDebug("Enabling OAuth authentication");
    m_token = token;
}
