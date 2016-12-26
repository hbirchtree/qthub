#ifndef GITHUBFETCH_H
#define GITHUBFETCH_H

#include <QObject>
#include <QPointer>
#include <QNetworkReply>
#include <QMap>

class GithubUser;
class GithubRepo;
class GithubTag;
class GithubRelease;
class GithubAsset;
class GithubBranch;
class GithubCommit;
class GithubStore;

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
        GitRelease,

        GitReadme,
        GitBranchHead,

        GitAllRepos,
        GitAllReleases,
        GitAllTags,
        GitAllAssets,
        GitAllBranches,
        GitAllRefs,

        GitReleaseDelete,
        GitTagDelete,
        GitAssetDelete,

        GitReleaseCreate,
        GitTagCreate,

        GitDownload,
        GitUpload,
    };

    QNetworkAccessManager* m_netman;
    QString m_apipoint;
    QString m_agentstring;
    QString m_token;

    int m_activeTransfers;

    bool m_authenticated;

    GithubStore* m_store;

public:
    explicit GithubFetch(QObject *parent = 0);
    explicit GithubFetch(GithubStore* store, QString identifier, QObject *parent = 0);

    QNetworkAccessManager::NetworkAccessibility status()
    {
        return m_netman->networkAccessible();
    }

    bool activeTransfers()
    {
        return m_activeTransfers > 0;
    }

private:
    void addConnections(QNetworkReply* rep);
    void addToken(QNetworkRequest& req);

    void updateUser(GithubUser* u, QJsonObject const& o);
    void updateRepo(GithubRepo* r, QJsonObject const& o);

    void addRepositories(GithubUser* u, QJsonArray const& repos);
    void addReleases(GithubRepo* u, QJsonArray const& rels);
    void addTags(GithubRepo* u, QJsonArray const& tags);
    void addBranches(GithubRepo* u, QJsonArray const& branches);
    void addCommits(GithubRepo* u, QJsonArray const& commits, GithubBranch *b = nullptr);

    void addBranchHead(GithubBranch* b, QJsonObject const& head);

    void addAssets(GithubRelease* r, QJsonArray const& assets);

signals:
    void statusChanged(QNetworkAccessManager::NetworkAccessibility acc);
    void rateLimitUpdated(int remaining, int limit);
    void agentStringIdentifier(QString const& agent);

    /* Progression signals */
    void reportProgress(QString const& dl, qint64 rec, qint64 tot);
    void replyReceived(QString const& source, QString const& status);

    /* Errors */
    void authenticationError();
    void contentNotFound();
    void networkReplyError(int errorCode);
    void fileFailure(QString const& filename, QString const& errString);
    void parsingError(QString const& source, QString const& errString);

    /* Completion signals */
    void initComplete();
    void authenticated();
    void authenticatedNull();
    void transferCompleted();

    void selfUpdated(GithubUser* self);
    void userUpdated(GithubUser* user);
    void repoUpdated(GithubRepo* repo);
    void branchUpdated(GithubBranch* branch);
    void releaseUpdated(GithubRepo* repo, GithubRelease* release);
    void tagUpdated(GithubRepo* repo, GithubTag* tag);
    void assetUpdated(GithubRelease* release, GithubAsset* file);
    void commitUpdated(GithubBranch* branch, GithubCommit* commit);

    void readmeUpdated(GithubRepo* repo);

    /* File download signals */
    void downloadFailed(QUrl const& url, QString const& filename);
    void downloadSuccess(QUrl const& url, QString const& filename);

    void uploadFailed(QUrl const& url);
    void uploadSuccess(QUrl const& url);

private slots:
    /* Resource handling over HTTP */
    void startNetworkRequest(const QString &url,
                             const QString &id,
                             ReplyType receive,
                             int page = 1);
    void deleteResource(const QString &rsrc, const QString &id, ReplyType receive);
    void pushResource(const QString &apipoint, QString const& rsrc,
                      const QString &id, ReplyType receive, const QString &type,
                      QByteArray const& data);
    void pullResource(QString const& rsrc, const QString &id, ReplyType receive,
                      const QString &target, bool asset = false);

    /* Receiving all of the types of requests... Ouch. */
    void receiveUserData();

    /* Progression and stuff */
    void registerProgress(qint64 rec, qint64 tot);

    /* Special place for downloading files */
    void downloadFile(QUrl const& url, QString const& file);

public slots:
    void authenticate(QString const& token);

    void fetchUser(QString const& username);
    void fetchRepo(QString const& reponame);
    void fetchRelease(GithubRepo* repo, quint64 relId);

    void fetchBranchHead(GithubBranch* branch);
    void fetchReadme(GithubRepo* repo);

    void fetchAllRepositories(GithubUser* user);
    void fetchAllReleases(GithubRepo* repo);
    void fetchAllTags(GithubRepo* repo);
    void fetchAllBranches(GithubRepo* repo);

    void fetchSelf();

    void requestDelete(GithubRelease* release);
    void requestDelete(GithubTag* tag);
    void requestDelete(GithubAsset* tag);

    void requestDownload(GithubAsset* asset);
    void requestDownload(GithubRelease* rel);
    void requestDownload(GithubTag* tag);
    void requestDownload(GithubRepo* repo);
    void requestDownload(GithubRepo* repo, GithubBranch*branch);
    void requestDownload(GithubRepo* repo, QString const& ref);

    void requestUploadAsset(GithubRelease *rel,
                            GithubAsset* asset,
                            QByteArray const& data);
    void requestCreateRelease(GithubRelease* rel);
    void requestCreateTag(GithubTag* tag, GithubCommit* commit);
    void requestCreateBranch(GithubBranch* branch);

    void initProcessing();

    void killAll();
};

#endif // GITHUBFETCH_H
