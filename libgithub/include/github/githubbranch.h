#ifndef GITHUBBRANCH_H
#define GITHUBBRANCH_H

#include <QObject>
#include <QString>

class GithubRepo;

class GithubBranch : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString commit READ commit WRITE setCommit NOTIFY commitChanged)

    QString m_name;
    QString m_commit;

public:
    explicit GithubBranch(QObject* parent = 0);

    GithubRepo* repository() const;

    QString name() const
    {
        return m_name;
    }
    QString commit() const
    {
        return m_commit;
    }

public slots:
    void setName(QString name)
    {
        m_name = name;
    }
    void setCommit(QString commit)
    {
        if (m_commit == commit)
            return;

        m_commit = commit;
        emit commitChanged(commit);
    }
signals:
    void commitChanged(QString commit);
};

#endif
