#ifndef GITHUBREPO_H
#define GITHUBREPO_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QPointer>

class GithubRelease;
class GithubTag;

class GithubRepo : public QObject
{
    Q_OBJECT

    /* Details */
    Q_PROPERTY(quint64 id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

    Q_PROPERTY(QString owner READ owner WRITE setOwner)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)

    Q_PROPERTY(quint64 sourceId READ sourceId WRITE setSourceId)

    Q_PROPERTY(bool fork READ fork WRITE setFork)
    Q_PROPERTY(bool privt READ privt WRITE setPrivt NOTIFY privateChanged)

    /* Meta-data */
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

    /* Statistics */
    Q_PROPERTY(quint64 issues READ issues WRITE setIssues NOTIFY issuesChanged)
    Q_PROPERTY(quint64 forks READ forks WRITE setForks NOTIFY forksChanged)
    Q_PROPERTY(quint64 subscribers READ subscribers WRITE setSubscribers NOTIFY subscribersChanged)
    Q_PROPERTY(quint64 watchers READ watchers WRITE setWatchers NOTIFY watchersChanged)

    Q_PROPERTY(QString branch READ branch WRITE setBranch NOTIFY branchChanged)

    Q_PROPERTY(QVector<GithubTag*> tags READ tags)
    Q_PROPERTY(QVector<GithubRelease*> releases READ releases)

    quint64 m_id;
    QString m_name;
    QString m_title;
    QString m_description;

    QString m_owner;
    QDateTime m_created;

    QString m_language;
    QString m_branch;
    quint64 m_sourceId;

    quint64 m_issues;
    quint64 m_forks;
    quint64 m_subscribers;
    quint64 m_watchers;

    QVector<GithubRelease*> m_releases;
    QVector<GithubTag*> m_tags;

    union {
        struct
        {
            bool m_fork;
            bool m_private;
        };
        quint8 m_bitfield_store;
    };

public:
    explicit GithubRepo(QObject *parent = 0);

    QVector<GithubTag*> const& tags() const
    {
        return m_tags;
    }
    QVector<GithubRelease*> const& releases() const
    {
        return m_releases;
    }

    GithubTag* tag(QString const& name) const;
    GithubRelease *release(quint64 id) const;

public slots:
    void addRelease(GithubRelease* rel);
    void addTag(GithubTag* tag);

signals:
    void releaseAdded(GithubRelease* rel);
    void tagAdded(GithubTag* tag);

public:
    quint64 id() const
    {
        return m_id;
    }

    QString name() const
    {
        return m_name;
    }

    QString title() const
    {
        return m_title;
    }

    QString description() const
    {
        return m_description;
    }

    QString language() const
    {
        return m_language;
    }

    quint64 issues() const
    {
        return m_issues;
    }

    quint64 forks() const
    {
        return m_forks;
    }

    quint64 subscribers() const
    {
        return m_subscribers;
    }

    QDateTime created() const
    {
        return m_created;
    }

    quint64 watchers() const
    {
        return m_watchers;
    }

    QString branch() const
    {
        return m_branch;
    }

    QString owner() const
    {
        return m_owner;
    }

    quint64 sourceId() const
    {
        return m_sourceId;
    }

    bool fork()
    {
        return m_fork;
    }

    bool privt()
    {
        return m_private;
    }

signals:
    void idChanged(quint64 id);
    void nameChanged(QString name);
    void titleChanged(QString title);
    void descriptionChanged(QString description);
    void languageChanged(QString language);
    void issuesChanged(quint64 issues);
    void forksChanged(quint64 forks);
    void subscribersChanged(quint64 subscribers);
    void createdChanged(QDateTime created);
    void privateChanged(bool privt);
    void watchersChanged(quint64 watchers);
    void branchChanged(QString branch);

public slots:

    void setId(quint64 id)
    {
        if (m_id == id)
            return;

        m_id = id;
        emit idChanged(id);
    }
    void setName(QString name)
    {
        if (m_name == name)
            return;

        m_name = name;
        emit nameChanged(name);
    }
    void setTitle(QString title)
    {
        if (m_title == title)
            return;

        m_title = title;
        emit titleChanged(title);
    }
    void setDescription(QString description)
    {
        if (m_description == description)
            return;

        m_description = description;
        emit descriptionChanged(description);
    }
    void setLanguage(QString language)
    {
        if (m_language == language)
            return;

        m_language = language;
        emit languageChanged(language);
    }
    void setIssues(quint64 issues)
    {
        if (m_issues == issues)
            return;

        m_issues = issues;
        emit issuesChanged(issues);
    }
    void setForks(quint64 forks)
    {
        if (m_forks == forks)
            return;

        m_forks = forks;
        emit forksChanged(forks);
    }
    void setSubscribers(quint64 subscribers)
    {
        if (m_subscribers == subscribers)
            return;

        m_subscribers = subscribers;
        emit subscribersChanged(subscribers);
    }
    void setCreated(QDateTime created)
    {
        if (m_created == created)
            return;

        m_created = created;
        emit createdChanged(created);
    }
    void setWatchers(quint64 watchers)
    {
        if (m_watchers == watchers)
            return;

        m_watchers = watchers;
        emit watchersChanged(watchers);
    }
    void setBranch(QString branch)
    {
        if (m_branch == branch)
            return;

        m_branch = branch;
        emit branchChanged(branch);
    }
    void setOwner(QString owner)
    {
        m_owner = owner;
    }
    void setSourceId(quint64 sourceId)
    {
        m_sourceId = sourceId;
    }

    void setFork(bool fork)
    {
        m_fork = fork;
    }
    void setPrivt(bool privt)
    {
        if(m_private == privt)
            return;

        m_private = privt;
        emit privateChanged(privt);
    }
};

#endif // GITHUBREPO_H
