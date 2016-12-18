#ifndef GITHUBTAG_H
#define GITHUBTAG_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QUrl>

class GithubTag : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString commit READ commit WRITE setCommit)
    Q_PROPERTY(QUrl tarballUrl READ tarballUrl WRITE setTarballUrl)

    QString m_name;
    QString m_commit;
    QUrl m_tarballUrl;

public:
    explicit GithubTag(QObject* parent = 0);

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
