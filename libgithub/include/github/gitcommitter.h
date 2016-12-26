#ifndef GITCOMMITTER_H
#define GITCOMMITTER_H

#include <QObject>
#include <QString>

class GitCommitter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString email READ email WRITE setEmail)

    QString m_name;
    QString m_email;

public:
    explicit GitCommitter(QObject* parent = 0) : QObject(parent) {}
    QString name() const
    {
        return m_name;
    }
    QString email() const
    {
        return m_email;
    }

public slots:
    void setName(QString name)
    {
        m_name = name;
    }
    void setEmail(QString email)
    {
        m_email = email;
    }
};

#endif
