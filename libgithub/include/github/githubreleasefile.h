#ifndef GITHUBRELEASEFILE_H
#define GITHUBRELEASEFILE_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QDateTime>

class GithubRelease;

class GithubAsset : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint64 id READ id WRITE setId)

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QString type READ type WRITE setType)

    Q_PROPERTY(quint64 size READ size WRITE setSize)
    Q_PROPERTY(quint64 downloads READ downloads WRITE setDownloads NOTIFY downloadsChanged)

    Q_PROPERTY(QString uploader READ uploader WRITE setUploader)

    Q_PROPERTY(QDateTime updated READ updated WRITE setUpdated NOTIFY updatedChanged)

    Q_PROPERTY(QUrl downloadUrl READ downloadUrl WRITE setDownloadUrl)

    quint64 m_id;
    QString m_name;
    QString m_label;
    QString m_type;
    quint64 m_size;
    quint64 m_downloads;
    QString m_uploader;
    QDateTime m_updated;
    QUrl m_downloadUrl;

public:
    explicit GithubAsset(QObject* parent = 0);

    GithubRelease* release() const;

    quint64 id() const
    {
        return m_id;
    }
    QString name() const
    {
        return m_name;
    }
    QString label() const
    {
        return m_label;
    }
    QString type() const
    {
        return m_type;
    }
    quint64 size() const
    {
        return m_size;
    }
    quint64 downloads() const
    {
        return m_downloads;
    }
    QString uploader() const
    {
        return m_uploader;
    }
    QDateTime updated() const
    {
        return m_updated;
    }
    QUrl downloadUrl() const
    {
        return m_downloadUrl;
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
    void setLabel(QString label)
    {
        if (m_label == label)
            return;

        m_label = label;
        emit labelChanged(label);
    }
    void setType(QString type)
    {
        m_type = type;
    }

    void setSize(quint64 size)
    {
        m_size = size;
    }

    void setDownloads(quint64 downloads)
    {
        if (m_downloads == downloads)
            return;

        m_downloads = downloads;
        emit downloadsChanged(downloads);
    }
    void setUploader(QString uploader)
    {
        m_uploader = uploader;
    }
    void setUpdated(QDateTime updated)
    {
        if (m_updated == updated)
            return;

        m_updated = updated;
        emit updatedChanged(updated);
    }
    void setDownloadUrl(QUrl downloadUrl)
    {
        m_downloadUrl = downloadUrl;
    }

signals:
    void labelChanged(QString label);
    void downloadsChanged(quint64 downloads);
    void updatedChanged(QDateTime updated);
};

#endif
