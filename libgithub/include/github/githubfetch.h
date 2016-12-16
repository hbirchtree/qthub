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

    Q_PROPERTY(QNetworkAccessManager::NetworkAccessibility status READ status NOTIFY statusChanged)

    enum ReplyType
    {
        GitNone,
        GitRepo,
        GitUser,
        GitAllRepos,
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
    explicit GithubFetch(QString identifier, QObject *parent = 0);

    QNetworkAccessManager::NetworkAccessibility status()
    {
        return m_netman->networkAccessible();
    }

private:
    void addToken(QNetworkRequest& req);

    void updateUser(GithubUser* u, QJsonObject const& o);
    void updateRepo(GithubRepo* r, QJsonObject const& o);

    void addRepositories(GithubUser* u, QJsonArray const& repos);
    void addReleases(GithubRepo* u, QJsonArray const& rels);

signals:
    void userUpdated(GithubUser* user);
    void repoUpdated(GithubRepo* repo);

    void authenticationError();
    void contentNotFound();

    void networkReplyError(int errorCode);

    void statusChanged(QNetworkAccessManager::NetworkAccessibility acc);

private slots:
    void startNetworkRequest(const QString &url,
                             const QString &id,
                             ReplyType receive);
    void receiveUserData();

    void registerProgress(qint64 rec, qint64 tot);

public slots:
    void authenticate(QString const& token);

    void fetchUser(QString const& username);
    void fetchAllRepositories(GithubUser* user);
    void fetchRepo(QString const& reponame);
    void fetchAllReleases(GithubRepo* repo);
    void fetchRelease(GithubRepo* repo, quint64 relId);

    void requestDelete(GithubRelease* release);

    void killAll();
};

#endif // GITHUBFETCH_H
