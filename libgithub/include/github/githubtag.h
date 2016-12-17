#ifndef GITHUBTAG_H
#define GITHUBTAG_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QUrl>

class GithubTag : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint64 id READ id WRITE setId)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString commit READ commit WRITE setCommit)
    Q_PROPERTY(QUrl tarballUrl READ tarballUrl WRITE setTarballUrl)

    quint64 m_id;
    QString m_name;
    QString m_commit;
    QUrl m_tarballUrl;

public:
    explicit GithubTag(QObject* parent = 0);

    quint64 id() const
    {
        return m_id;
    }
    QString name() const
    {
        return m_name;
    }

    QString commit() const
    {
        return m_commit;
    }

    QUrl tarballUrl() const
    {
        return m_tarballUrl;
    }

public slots:
    void setId(quint64 id)
    {
        m_id = id;
    }
    void setName(QString name)
    {
        m_name = name;
    }

    void setCommit(QString commit)
    {
        m_commit = commit;
    }

    void setTarballUrl(QUrl tarballUrl)
    {
        m_tarballUrl = tarballUrl;
    }
};

#endif
