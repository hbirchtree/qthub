#include <github/github.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void GithubFetch::updateUser(GithubUser* u, QJsonObject const& o)
{
    u->setId(o["id"].toVariant().toULongLong());

    u->setLogin(o["login"].toString());
    if(!o["name"].isNull())
        u->setName(o["name"].toString());
    if(!o["bio"].isNull())
        u->setBio(o["bio"].toString());
    if(!o["avatar_url"].isNull())
        u->setAvatar(o["avatar_url"].toString());
    if(!o["email"].isNull())
        u->setEmail(o["email"].toString());
    if(!o["created_at"].isNull())
        u->setRegistered(QDateTime::fromString(o["created_at"].toString()));

    u->setFollowers(o["followers"].toVariant().toULongLong());
    u->setFollowing(o["following"].toVariant().toULongLong());

    u->setNumRepos(o["public_repos"].toVariant().toULongLong());
}

void GithubFetch::updateRepo(GithubRepo* r, QJsonObject const& o)
{
    r->setId(o["id"].toVariant().toULongLong());

    r->setCreated(o["created_at"].toVariant().toDateTime());
    r->setName(o["full_name"].toString());
    r->setDescription(o["description"].toString());

    r->setLanguage(o["language"].toString());

    r->setIssues(o["open_issues"].toVariant().toULongLong());
    r->setForks(o["forks_count"].toVariant().toULongLong());
    r->setSubscribers(o["subscribers_count"].toVariant().toULongLong());
    r->setWatchers(o["watchers_count"].toVariant().toULongLong());

    r->setBranch(o["default_branch"].toString());

    r->setOwner(o["owner"].toObject()["login"].toString());
    r->setSourceId(o["source"].toObject()[""].toVariant().toULongLong());
}

void GithubFetch::addRepositories(GithubUser* u, QJsonArray const& repos)
{
    for(int i=0;i<repos.size();i++)
    {
        QJsonObject const& m = repos[i].toObject();
        GithubRepo* r = new GithubRepo(u);

        updateRepo(r, m);

        u->addRepository(r);
        m_repos.insert(r->name(), r);
        repoUpdated(r);
    }
}

void GithubFetch::addReleases(GithubRepo* r, QJsonArray const& rels)
{
    for(int i=0;i<rels.size();i++)
    {
        auto const& m = rels[i].toObject();
        GithubRelease* rl = new GithubRelease(r);

        rl->setId(m["id"].toVariant().toULongLong());

        if(!m["tag_name"].isNull())
            rl->setTagName(m["tag_name"].toString());
        rl->setBranch(m["target_commitish"].toString());
        rl->setName(m["name"].toString());
        rl->setAuthor(m["author"].toObject()["login"].toString());

        rl->setDescription(m["body"].toString());
        rl->setDraft(m["draft"].toBool());
        rl->setPrerelease(m["prerelease"].toBool());

        rl->setPublished(m["published_at"].toVariant().toDateTime());
        rl->setCreated(m["created_at"].toVariant().toDateTime());

        rl->setUploadUrl(m["upload_url"].toString().split("{")[0]);

        addAssets(rl, m["assets"].toArray());

        r->addRelease(rl);

        releaseUpdated(r, rl);
    }
}

void GithubFetch::addTags(GithubRepo *u, const QJsonArray &tags)
{
    for(int i=0;i<tags.size();i++)
    {
        auto const& m = tags[i].toObject();
        GithubTag* tg = new GithubTag(u);

        tg->setName(m["name"].toString());
        tg->setCommit(m["commit"].toObject()["sha"].toString());
        tg->setTarballUrl(m["tarball_url"].toString());

        u->addTag(tg);
        tagUpdated(u, tg);
    }
}

void GithubFetch::addBranches(GithubRepo *u, const QJsonArray &branches)
{
    for(int i=0;i<branches.size();i++)
    {
        auto const& m = branches[i].toObject();
        GithubBranch* b = new GithubBranch(u);

        b->setName(m["name"].toString());
        b->setCommit(m["commit"].toObject()["sha"].toString());

        u->addBranch(b);
        branchUpdated(u, b);
    }
}

void GithubFetch::addAssets(GithubRelease *r, const QJsonArray &assets)
{
    for(int i=0;i<assets.size();i++)
    {
        auto const& m = assets[i].toObject();
        GithubAsset* at = new GithubAsset(r);

        at->setId(m["id"].toVariant().toULongLong());
        at->setName(m["name"].toString());
        at->setUploader(m["uploader"].toObject()["login"].toString());
        at->setLabel(m["label"].toString());
        at->setType(m["content_type"].toString());
        at->setSize(m["size"].toVariant().toULongLong());
        at->setDownloads(m["download_count"].toVariant().toULongLong());
        at->setDownloadUrl(m["browser_download_url"].toVariant().toUrl());

        at->setUpdated(m["updated_at"].toVariant().toDateTime());

        r->addAsset(at);
        assetUpdated(r, at);
    }
}
