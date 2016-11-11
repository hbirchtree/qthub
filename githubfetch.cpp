#include "githubfetch.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDebug>

#include <QJsonDocument>

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
}

void GithubFetch::fetchRepo(const QString &reponame)
{

}

void GithubFetch::receiveUserData()
{
    QObject* o = sender();

    if(o)
    {
        QNetworkReply* rep = dynamic_cast<QNetworkReply*>(o);

        qDebug() << rep->readAll();
    }
}
