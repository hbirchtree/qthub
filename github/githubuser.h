#ifndef GITHUBUSER_H
#define GITHUBUSER_H

#include <QObject>
#include <QString>
#include <QUrl>

class GithubUser : public QObject
{
    Q_OBJECT

    /* User information */
    Q_PROPERTY(quint64 id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString login READ login WRITE setLogin NOTIFY loginChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString bio READ bio WRITE setBio NOTIFY bioChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString registered READ registered WRITE setRegistered NOTIFY registeredChanged)

    Q_PROPERTY(QUrl avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)

    /* Business */
    Q_PROPERTY(quint64 numRepos READ numRepos WRITE setNumRepos NOTIFY numReposChanged)

    /* Community */
    Q_PROPERTY(quint64 followers READ followers WRITE setFollowers NOTIFY followersChanged)
    Q_PROPERTY(quint64 following READ following WRITE setFollowing NOTIFY followingChanged)

    quint64 m_id;
    QString m_login;
    QString m_name;
    QString m_bio;
    QString m_email;
    QString m_registered;
    QUrl m_avatar;

    quint64 m_numRepos;

    quint64 m_followers;
    quint64 m_following;

public:
    explicit GithubUser(QObject *parent = 0);

    quint64 id() const
    {
        return m_id;
    }

    QString login() const
    {
        return m_login;
    }

    QString name() const
    {
        return m_name;
    }

    QString bio() const
    {
        return m_bio;
    }

    QString email() const
    {
        return m_email;
    }

    QString registered() const
    {
        return m_registered;
    }

    QUrl avatar() const
    {
        return m_avatar;
    }

    quint64 numRepos() const
    {
        return m_numRepos;
    }

    quint64 followers() const
    {
        return m_followers;
    }

    quint64 following() const
    {
        return m_following;
    }

signals:

    void idChanged(quint64 id);

    void loginChanged(QString login);

    void nameChanged(QString name);

    void bioChanged(QString bio);

    void emailChanged(QString email);

    void registeredChanged(QString registered);

    void avatarChanged(QUrl avatar);

    void numReposChanged(quint64 numRepos);

    void followersChanged(quint64 followers);

    void followingChanged(quint64 following);

public slots:
    void setId(quint64 id)
    {
        if (m_id == id)
            return;

        m_id = id;
        emit idChanged(id);
    }
    void setLogin(QString login)
    {
        if (m_login == login)
            return;

        m_login = login;
        emit loginChanged(login);
    }
    void setName(QString name)
    {
        if (m_name == name)
            return;

        m_name = name;
        emit nameChanged(name);
    }
    void setBio(QString bio)
    {
        if (m_bio == bio)
            return;

        m_bio = bio;
        emit bioChanged(bio);
    }
    void setEmail(QString email)
    {
        if (m_email == email)
            return;

        m_email = email;
        emit emailChanged(email);
    }
    void setRegistered(QString registered)
    {
        if (m_registered == registered)
            return;

        m_registered = registered;
        emit registeredChanged(registered);
    }
    void setAvatar(QUrl avatar)
    {
        if (m_avatar == avatar)
            return;

        m_avatar = avatar;
        emit avatarChanged(avatar);
    }
    void setNumRepos(quint64 numRepos)
    {
        if (m_numRepos == numRepos)
            return;

        m_numRepos = numRepos;
        emit numReposChanged(numRepos);
    }
    void setFollowers(quint64 followers)
    {
        if (m_followers == followers)
            return;

        m_followers = followers;
        emit followersChanged(followers);
    }
    void setFollowing(quint64 following)
    {
        if (m_following == following)
            return;

        m_following = following;
        emit followingChanged(following);
    }
};

#endif // GITHUBUSER_H
