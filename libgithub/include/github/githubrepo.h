#ifndef GITHUBREPO_H
#define GITHUBREPO_H

#include "githubrelease.h"
#include "githubtag.h"

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QPointer>

class GithubRepo : public QObject
{
    Q_OBJECT

    /* Details */
    Q_PROPERTY(quint64 id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)

    /* Meta-data */
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

    /* Statistics */
    Q_PROPERTY(quint64 issues READ issues WRITE setIssues NOTIFY issuesChanged)
    Q_PROPERTY(quint64 forks READ forks WRITE setForks NOTIFY forksChanged)
    Q_PROPERTY(quint64 subscribers READ subscribers WRITE setSubscribers NOTIFY subscribersChanged)

    Q_PROPERTY(QVector<GithubTag*> tags READ tags)
    Q_PROPERTY(QVector<GithubRelease*> releases READ releases)

    quint64 m_id;
    QString m_name;
    QString m_title;
    QString m_description;

    QDateTime m_created;

    QString m_language;

    quint64 m_issues;
    quint64 m_forks;
    quint64 m_subscribers;

    QVector<GithubRelease*> m_releases;
    QVector<GithubTag*> m_tags;

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

public slots:
    void addRelease(GithubRelease* rel);
    void addTag(GithubTag* tag);

signals:
    void releaseAdded(QPointer<GithubRelease> rel);
    void tagAdded(QPointer<GithubTag> tag);

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
};

#endif // GITHUBREPO_H
