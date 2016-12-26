#include <github/github.h>

#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void GithubFetch::addConnections(QNetworkReply *rep)
{
    connect(rep, &QNetworkReply::finished,
            this, &GithubFetch::receiveUserData);
    connect(rep, &QNetworkReply::downloadProgress,
            this, &GithubFetch::registerProgress);
    m_activeTransfers++;
}

void GithubFetch::addToken(QNetworkRequest &req)
{
    req.setRawHeader("User-Agent", m_agentstring.toLocal8Bit());
    if(!m_token.isEmpty())
        req.setRawHeader("Authorization",
                         QString("token %1")
                         .arg(m_token)
                         .toLocal8Bit());
}

void GithubFetch::startNetworkRequest(const QString &url,
                                      QString const& id,
                                      GithubFetch::ReplyType receive,
                                      int page)
{
    QNetworkRequest req;
    req.setUrl(m_apipoint + url + QString("?page=%1").arg(page));
    addToken(req);

    QNetworkReply* rep = m_netman->get(req);
    rep->setProperty("type", receive);
    rep->setProperty("id", id);
    addConnections(rep);
}

void GithubFetch::deleteResource(const QString &rsrc, const QString &id, GithubFetch::ReplyType receive)
{
    QNetworkRequest req;
    req.setUrl(m_apipoint + rsrc);
    addToken(req);

    QNetworkReply* rep = m_netman->deleteResource(req);
    qDebug() << "Delete:" << req.url().toString();
    rep->setProperty("type", receive);
    rep->setProperty("id", id);
    addConnections(rep);
}

void GithubFetch::pushResource(QString const& apipoint,
                               const QString &rsrc, const QString &id,
                               GithubFetch::ReplyType receive,
                               QString const& type,
                               QByteArray const& data)
{
    QNetworkRequest req;
    req.setUrl(apipoint + rsrc);
    req.setHeader(QNetworkRequest::ContentTypeHeader, type);
    addToken(req);

    qDebug() << data;

    QNetworkReply* rep = m_netman->post(req, data);
    rep->setProperty("type", receive);
    rep->setProperty("id", id);
    addConnections(rep);
}

void GithubFetch::pullResource(const QString &rsrc, const QString &id,
                               GithubFetch::ReplyType receive,
                               const QString &target,
                               bool asset)
{
    QNetworkRequest req;
    req.setUrl(m_apipoint + rsrc);
    /* Allow binary data transfer, aka. file transfer */
    if(asset)
        req.setRawHeader("Accept", "application/octet-stream");
    addToken(req);

    QNetworkReply* rep = m_netman->get(req);
    rep->setProperty("type", receive);
    rep->setProperty("id", id);
    rep->setProperty("target", target);
    addConnections(rep);
}

void GithubFetch::receiveUserData()
{

    QObject* o = sender();

    if(o)
    {
        QNetworkReply* rep = dynamic_cast<QNetworkReply*>(o);

        replyReceived(rep->url().toString(), rep->rawHeader("Status"));

        /* Check for error */
        qDebug().noquote().nospace()
                << "Response: " << rep->url().toString() << rep->rawHeader("Status");
        switch(rep->error())
        {
        case QNetworkReply::NoError:
            break;
        case 204:
            transferCompleted();
            contentNotFound();
            return;
        case QNetworkReply::InternalServerError:
            if(rep->rawHeader("Status") == "401 Unauthorized")
                authenticationError();
            transferCompleted();
            return;
        case QNetworkReply::ServiceUnavailableError:
            authenticationError();
            transferCompleted();
            return;
        default:
            networkReplyError(rep->error());
            transferCompleted();
            return;
        }

        /* Get the data */
        QByteArray rep_data = rep->readAll();

        if(rep_data.size() <= 0)
        {
            if(rep->rawHeader("Status") == "302 Found")
            {
                downloadFile(QUrl(rep->rawHeader("Location")),
                             rep->property("target").toString());
            }
            transferCompleted();
            return;
        }

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(rep_data, &err);
        if(err.error != QJsonParseError::NoError
                && rep->property("type").toInt() != GitDownload
                && rep->property("type").toInt() != GitUpload)
        {
            parsingError(rep->url().toString(), err.errorString());
            transferCompleted();
            return;
        }

        switch(o->property("type").toInt())
        {
        case GitUser:
        {
            auto dobj = doc.object();
            QString obj_id = dobj["login"].toString();
            GithubUser* u = m_store->users().value(obj_id);
            if(!u)
            {
                u = new GithubUser(this);
                m_store->users().insert(obj_id, u);
            }
            updateUser(u, doc.object());
            if(o->property("id").toString() == ":self")
            {
                selfUpdated(u);
                m_store->setSelf(u);
            }
            else
                userUpdated(u);
            break;
        }
        case GitRepo:
        {
            GithubRepo* r = m_store->repos().value(o->property("id").toString());
            if(!r)
            {
                r = new GithubRepo(this);
                m_store->repos().insert(o->property("id").toString(), r);
            }
            updateRepo(r, doc.object());
            repoUpdated(r);
            break;
        }
        case GitRelease:
        {
            GithubRepo* r = m_store->repos().value(o->property("id").toString());
            QJsonArray arr = QJsonArray();
            arr.append(doc.object());
            if(r)
                addReleases(r, arr);
            break;
        }
        case GitAllRepos:
        {
            GithubUser* u = m_store->users().value(o->property("id").toString());
            if(u)
                addRepositories(u, doc.array());
            break;
        }
        case GitAllTags:
        {
            GithubRepo* r = m_store->repos().value(o->property("id").toString());
            if(r && doc.isArray())
                addTags(r, doc.array());
            else
            {
                qDebug() << doc;
                uploadSuccess(rep->url());
            }
            break;
        }
        case GitAllReleases:
        {
            GithubRepo* r = m_store->repos().value(o->property("id").toString());
            if(r)
                addReleases(r, doc.array());
            break;
        }
        case GitAllBranches:
        {
            GithubRepo* r = m_store->repos().value(o->property("id").toString());
            if(r)
                addBranches(r, doc.array());
            break;
        }
        case GitBranchHead:
        {
            QStringList args = o->property("id").toString().split(":");
            GithubRepo* r = m_store->repos().value(args[0]);
            GithubBranch* b = r->branch(args[1]);
            if(r)
                addBranchHead(b, doc.object());
            break;
        }
        case GitDownload:
        {
            QFile output(rep->property("target").toString());
            if(!output.open(QFile::WriteOnly))
            {
                fileFailure(output.fileName(), output.errorString());
                downloadFailed(rep->url(), output.fileName());
            }else{
                if(output.write(rep_data) == -1)
                    downloadFailed(rep->url(), output.fileName());
                else
                    downloadSuccess(rep->url(), output.fileName());
            }
            break;
        }
        case GitUpload:
        {
            break;
        }
        }

        if(rep->hasRawHeader("X-RateLimit-Remaining"))
        {
            QString rem = rep->rawHeader("X-RateLimit-Remaining");
            QString lim = rep->rawHeader("X-RateLimit-Limit");
            rateLimitUpdated(rem.toInt(), lim.toInt());
        }

        /* If data is paginated, get the next pages */
        QString linkHeader = rep->rawHeader("Link");

        if(!linkHeader.isEmpty())
        {
            static QRegExp exp("page=(\\d+)>; rel=\"last\"");
            static QRegExp exp2("page=(\\d+)>; rel=\"next\"");

            if(exp.indexIn(linkHeader) != -1 && exp2.indexIn(linkHeader) != -1)
            {
                if(exp.capturedTexts().size() > 1 && exp2.capturedTexts().size() > 1)
                {
                    int next = exp2.cap(1).toInt();
                    int last = exp.cap(1).toInt();

                    QNetworkRequest req = rep->request();

                    if(next == 2)
                        for(int i=2;i<=last;i++)
                        {
                            startNetworkRequest(
                                        req.url().path().split("?").at(0),
                                        rep->property("id").toString(),
                                        static_cast<ReplyType>(rep->property("type").toInt()),
                                        i);
                        }
                }
            }
        }

        transferCompleted();
    }

}

void GithubFetch::downloadFile(const QUrl &url, const QString &file)
{
    QNetworkRequest req;
    req.setUrl(url);

    QNetworkReply* rep = m_netman->get(req);
    rep->setProperty("target", file);
    rep->setProperty("type", GitDownload);
    addConnections(rep);
}
