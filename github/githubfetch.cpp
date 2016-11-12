#include "githubfetch.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <functional>

void updateUser(GithubUser* u, QJsonObject const& o)
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

void updateRepo(GithubRepo* r, QJsonObject const& o)
{
    r->setId(o["id"].toVariant().toULongLong());

    r->setName(o["full_name"].toString());
    r->setDescription(o["description"].toString());

    r->setLanguage(o["language"].toString());
    r->setIssues(o["open_issues"].toVariant().toULongLong());
}

void addReleases(GithubRepo* r, QJsonArray const& rels)
{
    for(QVariant const& v : rels.toVariantList())
    {
        QMap<QString,QVariant> m = v.toMap();
        GithubRelease* rl = new GithubRelease(r);

        rl->setId(m.value("id").toULongLong());

        if(!m.value("tag_name").isNull())
            rl->setTagName(m.value("tag_name").toString());
        rl->setDraft(m.value("draft").toBool());

        r->addRelease(rl);
    }
}

GithubFetch::GithubFetch(QObject *parent) : QObject(parent)
{
    m_netman = new QNetworkAccessManager();

    m_apipoint = "https://api.github.com";
    m_agentstring = "HBirchtree-Qthub-App";

    qDebug() << "I will identify as: " << m_agentstring;
}

void GithubFetch::addToken(QNetworkRequest &req)
{
    req.setRawHeader("User-Agent", m_agentstring.toLocal8Bit());
    req.setRawHeader("Authorization",
                     QString("token %1")
                     .arg(m_token)
                     .toLocal8Bit());
}

void GithubFetch::startNetworkRequest(const QString &url,
                                      QString const& id,
                                      GithubFetch::ReplyType receive)
{
    QNetworkRequest req;
    req.setUrl(m_apipoint + url);
    addToken(req);

    QNetworkReply* rep = m_netman->get(req);
    rep->setProperty("type", receive);
    rep->setProperty("id", id);
    connect(rep, &QNetworkReply::finished,
            this, &GithubFetch::receiveUserData);
    connect(rep, &QNetworkReply::downloadProgress,
            this, &GithubFetch::registerProgress);
}

void GithubFetch::fetchUser(const QString &username)
{
    startNetworkRequest(QString("/users/%2")
                        .arg(username),
                        username, GitUser);
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

void GithubFetch::fetchRelease(GithubRepo *repo, quint64 relId)
{
    startNetworkRequest(
                QString("/repos/%1/releases/%2")
                .arg(repo->name()).arg(relId),
                repo->name(),
                GitRelease);
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
}

void GithubFetch::receiveUserData()
{
    QObject* o = sender();

    if(o)
    {
        QNetworkReply* rep = dynamic_cast<QNetworkReply*>(o);

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
            GithubUser* u = m_users.value(o->property("id").toString());

            if(!u)
            {
                u = new GithubUser(this);
                m_users.insert(o->property("id").toString(), u);
            }

            updateUser(u, doc.object());

            userUpdated(u);
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
        case GitRelease:
        {
            break;
        }
        case GitAllReleases:
        {
            GithubRepo* r = m_repos.value(o->property("id").toString());

            if(r)
                addReleases(r, doc.array());
            break;
        }
        case GitReleaseDelete:
        {
            qDebug() << "Deleted tag from " << rep->url() <<
                        " with status " << rep->rawHeader("Status");
            break;
        }
        }
    }
}

void GithubFetch::registerProgress(qint64 rec, qint64 tot)
{
    QNetworkRequest* req = dynamic_cast<QNetworkRequest*>(sender());
    if(req)
    {
        qDebug() << "Download " << req->url() << " : " << rec << "/" << tot;
    }
}

void GithubFetch::authenticate(const QString &token)
{
    m_token = token;
}
