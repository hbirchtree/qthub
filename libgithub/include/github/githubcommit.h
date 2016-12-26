#ifndef GITHUBCOMMIT_H
#define GITHUBCOMMIT_H

#include "gitcommitter.h"

#include <QObject>
#include <QString>
#include <QDateTime>

class GithubCommit : public QObject
{
    Q_PROPERTY(QString sha READ sha WRITE setSha)
    Q_PROPERTY(QString message READ message WRITE setMessage)

    Q_PROPERTY(QString parent READ parent WRITE setParent)
    Q_PROPERTY(QString tree READ tree WRITE setTree)

    Q_PROPERTY(QDateTime authorDate READ authorDate WRITE setAuthorDate)
    Q_PROPERTY(QDateTime commitDate READ commitDate WRITE setCommitDate)

    Q_PROPERTY(GitCommitter* author READ author WRITE setAuthor)
    Q_PROPERTY(GitCommitter* committer READ committer WRITE setCommitter)

public:
    explicit GithubCommit(QObject* parent = 0);
    QString sha() const
    {
        return m_sha;
    }
    QString message() const
    {
        return m_message;
    }

    QString parent() const
    {
        return m_parent;
    }

    QString tree() const
    {
        return m_tree;
    }

    QDateTime authorDate() const
    {
        return m_authorDate;
    }

    QDateTime commitDate() const
    {
        return m_commitDate;
    }

    GitCommitter* author() const
    {
        return m_author;
    }

    GitCommitter* committer() const
    {
        return m_committer;
    }

public slots:
    void setSha(QString sha)
    {
        m_sha = sha;
    }
    void setMessage(QString message)
    {
        m_message = message;
    }

    void setParent(QString parent)
    {
        m_parent = parent;
    }

    void setTree(QString tree)
    {
        m_tree = tree;
    }

    void setAuthorDate(QDateTime authorDate)
    {
        m_authorDate = authorDate;
    }

    void setCommitDate(QDateTime commitDate)
    {
        m_commitDate = commitDate;
    }

    void setAuthor(GitCommitter* author)
    {
        m_author = author;
    }

    void setCommitter(GitCommitter* committer)
    {
        m_committer = committer;
    }

private:
    QString m_sha;
    QString m_message;
    QString m_parent;
    QString m_tree;
    QDateTime m_authorDate;
    QDateTime m_commitDate;

    GitCommitter* m_author;
    GitCommitter* m_committer;
};

#endif
