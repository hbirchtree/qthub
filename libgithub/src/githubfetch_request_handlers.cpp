#include <github/github.h>

#include <QJsonDocument>
#include <QJsonObject>

void GithubFetch::requestDelete(GithubRelease *release)
{
    GithubRepo* repo = release->repository();
    if(!repo)
        return;

     deleteResource(QString("/repos/%1/releases/%2")
                    .arg(repo->name()).arg(release->id()),
                    QString("%1:%2")
                    .arg(repo->name()).arg(release->id()),
                    GitReleaseDelete);
}

void GithubFetch::requestDelete(GithubTag *tag)
{
    GithubRepo* repo = tag->repository();
    if(!repo)
        return;

    deleteResource(QString("/repos/%1/git/refs/tags/%2")
                   .arg(repo->name()).arg(tag->name()),
                   QString("%1:%2")
                   .arg(repo->name()).arg(tag->name()),
                   GitTagDelete);
}

void GithubFetch::requestDelete(GithubAsset *asset)
{
    GithubRelease* release = asset->release();
    if(!release)
        return;
    GithubRepo* repo = release->repository();
    if(!repo)
        return;

    deleteResource(QString("/repos/%1/releases/assets/%2")
                   .arg(repo->name()).arg(asset->id()),
                   QString("%1:%2")
                   .arg(repo->name()).arg(asset->id()),
                   GitAssetDelete);
}

void GithubFetch::requestDownload(GithubAsset *asset)
{
    GithubRelease* release = asset->release();
    if(!release)
        return;
    GithubRepo* repo = release->repository();
    if(!repo)
        return;

    pullResource(QString("/repos/%1/releases/assets/%2")
                 .arg(repo->name()).arg(asset->id()),
                 QString("%1:assets/%2")
                 .arg(repo->name()).arg(asset->id()),
                 GitDownload,
                 asset->name(),
                 true);
}

void GithubFetch::requestDownload(GithubRelease *rel)
{
    GithubRepo* repo = rel->repository();
    if(!repo)
        return;

    requestDownload(repo, rel->tagName());
}

void GithubFetch::requestDownload(GithubTag *tag)
{
    GithubRepo* repo = tag->repository();
    if(!repo)
        return;
    requestDownload(repo, tag->name());
}

void GithubFetch::requestDownload(GithubRepo *repo)
{
    requestDownload(repo, QString("heads/%1").arg(repo->branch()));
}

void GithubFetch::requestDownload(GithubRepo *repo, GithubBranch *branch)
{
    requestDownload(repo, QString("heads/%1").arg(branch->name()));
}

void GithubFetch::requestDownload(GithubRepo *repo, const QString &ref)
{
    QString repo_name = repo->name();
    QString ref_name = ref;
    repo_name.replace("/", "-");
    ref_name.replace("/", "-");

    pullResource(QString("/repos/%1/tarball/%2")
                 .arg(repo->name()).arg(ref),
                 QString("%1:source/%2")
                 .arg(repo->name()).arg(ref),
                 GitDownload,
                 QString("source_%1_%2.tar.gz")
                 .arg(repo_name).arg(ref_name));
}

void GithubFetch::requestUploadAsset(GithubRelease* rel,
                                     GithubAsset *asset,
                                     const QByteArray &data)
{
    QString label_cpy = asset->label();
    label_cpy.replace(" ", "%20");
    pushResource(rel->uploadUrl(),
                 QString("?name=%1&label=%2")
                 .arg(asset->name()).arg(label_cpy),
                 QString("%1:%2/assets:%3")
                 .arg(rel->repository()->name())
                 .arg(rel->id()).arg(asset->name()),
                 GitUpload, asset->type(), data);
}

void GithubFetch::requestCreateRelease(GithubRelease *rel)
{
    QJsonDocument doc;
    QJsonObject object;
    object["tag_name"] = rel->tagName();
    object["target_commitish"] = rel->branch();
    object["name"] = rel->name();
    object["body"] = rel->description();
    object["draft"] = rel->draft();
    object["prerelease"] = rel->prerelease();
    doc.setObject(object);

    pushResource(m_apipoint,
                 QString("/repos/%1/releases")
                 .arg(rel->repository()->name()),
                 QString("%1:%2")
                 .arg(rel->repository()->name()).arg(rel->tagName()),
                 GitAllReleases, "application/json",
                 doc.toJson());
}

void GithubFetch::requestCreateTag(GithubTag *tag, GithubCommit* commit)
{
    QJsonDocument doc;
    QJsonObject object;
    object["tag"] = tag->name();
    object["type"] = "commit";
    object["object"] = commit->sha();
    object["message"] = (tag->message().size() ? tag->message() : " - Auto-generated tag");
    {
        QJsonObject author;
        author["name"] = m_store->self()->name();
        author["email"] = m_store->self()->email();
        {
            QDateTime current = QDateTime::currentDateTime();
            current.setTimeSpec(Qt::OffsetFromUTC);
            author["date"] = current.toString(Qt::ISODate);
        }
        object["tagger"] = author;
    }

    doc.setObject(object);

    pushResource(m_apipoint,
                 QString("/repos/%1/git/tags")
                 .arg(tag->repository()->name()),
                 QString("%1:%2")
                 .arg(tag->repository()->name()).arg(tag->name()),
                 GitAllTags, "application/json",
                 doc.toJson());

    object = QJsonObject();
    object["ref"] = QString("refs/tags/%1").arg(tag->name());
    object["sha"] = commit->sha();

    doc.setObject(object);

    pushResource(m_apipoint,
                 QString("/repos/%1/git/refs")
                 .arg(tag->repository()->name()),
                 QString("%1:%2")
                 .arg(tag->repository()->name()),
                 GitAllRefs, "application/json",
                 doc.toJson());
}

void GithubFetch::requestCreateBranch(GithubBranch *branch)
{
    Q_UNUSED(branch);
}
