#ifndef GITHUBSTORE_H
#define GITHUBSTORE_H

#include <github/github.h>

#include <QObject>
#include <QMap>
#include <QString>

class GithubStore : public QObject
{
private:
    GithubUser* m_self;

    QMap<QString, GithubUser*> m_users;
    QMap<QString, GithubRepo*> m_repos;

public:
    GithubStore(QObject* parent = 0)
        : QObject(parent), m_self(nullptr)
    {}

    QMap<QString, GithubUser*>& users()
    {
        return m_users;
    }
    QMap<QString, GithubRepo*>& repos()
    {
        return m_repos;
    }

    GithubUser*& self()
    {
        return m_self;
    }
    void setSelf(GithubUser* user)
    {
        m_self = user;
    }
};

#endif
