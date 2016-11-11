#ifndef GITHUBFETCH_H
#define GITHUBFETCH_H

#include <QObject>
#include <QPointer>
#include <QNetworkReply>

#include "github/githubuser.h"
#include "github/githubrepo.h"

class QNetworkAccessManager;

class GithubFetch : public QObject
{
    Q_OBJECT

    enum ReplyType
    {
        GitNone,
        GitRepo,
        GitUser,
    };

    QList<GithubUser*> m_users;
    QList<GithubRepo*> m_repos;

    QNetworkAccessManager* m_netman;

public:
    explicit GithubFetch(QObject *parent = 0);

signals:
    void userReceived(QPointer<GithubUser> user);
    void userReceived(QPointer<GithubRepo> user);

public slots:
    void fetchUser(QString const& username);
    void fetchRepo(QString const& reponame);

    void receiveUserData();
};

#endif // GITHUBFETCH_H
