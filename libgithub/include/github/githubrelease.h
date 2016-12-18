#ifndef GITHUBRELEASE_H
#define GITHUBRELEASE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVector>

#include "githubreleasefile.h"

class GithubRelease : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint64 id READ id WRITE setId NOTIFY idChanged)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString tagName READ tagName WRITE setTagName NOTIFY tagNameChanged)

    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QDateTime published READ published WRITE setPublished NOTIFY publishedChanged)

    Q_PROPERTY(bool prerelease READ prerelease WRITE setPrerelease NOTIFY prereleaseChanged)
    Q_PROPERTY(bool draft READ draft WRITE setDraft NOTIFY draftChanged)

    Q_PROPERTY(QVector<GithubAsset*> assets READ assets)

    quint64 m_id;
    QString m_name;
    QString m_tagName;
    QDateTime m_created;
    QDateTime m_published;
    bool m_prerelease;
    bool m_draft;

    QVector<GithubAsset*> m_assets;

public:
    explicit GithubRelease(QObject *parent = 0);

    QVector<GithubAsset*> const& assets() const
    {
        return m_assets;
    }

    quint64 id() const
    {
        return m_id;
    }

    QString name() const
    {
        return m_name;
    }

    QString tagName() const
    {
        return m_tagName;
    }

    QDateTime created() const
    {
        return m_created;
    }

    QDateTime published() const
    {
        return m_published;
    }

    bool prerelease() const
    {
        return m_prerelease;
    }

    bool draft() const
    {
        return m_draft;
    }

signals:
    void idChanged(quint64 id);
    void nameChanged(QString name);
    void tagNameChanged(QString tagName);
    void createdChanged(QDateTime created);
    void publishedChanged(QDateTime published);
    void prereleaseChanged(bool prerelease);
    void draftChanged(bool draft);

    void assetAdded(GithubAsset* asset);

public slots:
    void addAsset(GithubAsset* asset);

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
    void setTagName(QString tagName)
    {
        if (m_tagName == tagName)
            return;

        m_tagName = tagName;
        emit tagNameChanged(tagName);
    }
    void setCreated(QDateTime created)
    {
        if (m_created == created)
            return;

        m_created = created;
        emit createdChanged(created);
    }
    void setPublished(QDateTime published)
    {
        if (m_published == published)
            return;

        m_published = published;
        emit publishedChanged(published);
    }
    void setPrerelease(bool prerelease)
    {
        if (m_prerelease == prerelease)
            return;

        m_prerelease = prerelease;
        emit prereleaseChanged(prerelease);
    }
    void setDraft(bool draft)
    {
        if (m_draft == draft)
            return;

        m_draft = draft;
        emit draftChanged(draft);
    }
};

#endif // GITHUBRELEASE_H
