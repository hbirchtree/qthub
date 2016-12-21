#include <github/github.h>

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
                                     const QString &fname,
                                     const QString &label,
                                     const QString &type,
                                     const QByteArray &data)
{
    QString label_cpy = label;
    label_cpy.replace(" ", "%20");
    pushResource(rel->uploadUrl(),
                 QString("?name=%1&label=%2")
                 .arg(fname).arg(label_cpy),
                 QString("%1:%2/assets:%3")
                 .arg(rel->repository()->name())
                 .arg(rel->id()).arg(fname),
                 GitUpload, type, data);
}
