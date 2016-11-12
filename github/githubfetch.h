#ifndef GITHUBFETCH_H
#define GITHUBFETCH_H

#include <QObject>
#include <QPointer>
#include <QNetworkReply>
#include <QMap>

#include "githubuser.h"
#include "githubrepo.h"

class QNetworkAccessManager;

class GithubFetch : public QObject
{
    Q_OBJECT

    enum ReplyType
    {
        GitNone,
        GitRepo,
        GitUser,
        GitRelease,
        GitAllReleases,

        GitReleaseDelete,
    };

    QMap<QString, GithubUser*> m_users;
    QMap<QString, GithubRepo*> m_repos;

    QNetworkAccessManager* m_netman;
    QString m_apipoint;
    QString m_agentstring;
    QString m_token;

public:
    explicit GithubFetch(QObject *parent = 0);

private:
    void addToken(QNetworkRequest& req);

signals:
    void userUpdated(GithubUser* user);
    void repoUpdated(GithubRepo* repo);

private slots:
    void startNetworkRequest(const QString &url,
                             const QString &id,
                             ReplyType receive);
    void receiveUserData();

    void registerProgress(qint64 rec, qint64 tot);

public slots:
    void authenticate(QString const& token);

    void fetchUser(QString const& username);
    void fetchRepo(QString const& reponame);
    void fetchAllReleases(GithubRepo* repo);
    void fetchRelease(GithubRepo* repo, quint64 relId);

    void requestDelete(GithubRelease* release);
};

#endif // GITHUBFETCH_H
