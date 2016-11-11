#include "githubfetch.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDebug>

#include <QJsonDocument>
#include <QJsonObject>

#include <functional>

void updateUser(GithubUser* u, QJsonObject const& o)
{
    u->setId(o["id"].toInt());

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
        u->setRegistered(o["created_at"].toString());

    u->setFollowers(o["followers"].toInt());
    u->setFollowing(o["following"].toInt());

    u->setNumRepos(o["public_repos"].toInt());
}

void updateRepo(GithubRepo* r, QJsonObject const& o)
{
    r->setId(o["id"].toInt());

    r->setName(o["name"].toString());
    r->setDescription(o["description"].toString());
}

GithubFetch::GithubFetch(QObject *parent) : QObject(parent)
{
    m_netman = new QNetworkAccessManager();
}

void GithubFetch::fetchUser(const QString &username)
{
    QNetworkRequest req;
    req.setUrl(QString("https://api.github.com/users/%1").arg(username));
    QNetworkReply* rep = m_netman->get(req);
    connect(rep, &QNetworkReply::finished,
            this, &GithubFetch::receiveUserData);
    rep->setProperty("type", GitUser);
}

void GithubFetch::fetchRepo(const QString &reponame)
{
    QNetworkRequest req;
    req.setUrl(QString("https://api.github.com/repos/%1").arg(reponame));
    QNetworkReply* rep = m_netman->get(req);
    connect(rep, &QNetworkReply::finished,
            this, &GithubFetch::receiveUserData);
    rep->setProperty("type", GitRepo);
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

        if(o->property("type") == GitUser)
        {
            m_users.push_front(new GithubUser(this));

            GithubUser* u = m_users.front();

            updateUser(u, doc.object());

            qDebug() << u->name() << u->login();
        }else if(o->property("type") == GitRepo)
        {
            m_repos.push_back(new GithubRepo(this));

            GithubRepo* r = m_repos.front();

            updateRepo(r, doc.object());

            qDebug() << r->name() << r->description();
        }
    }
}
