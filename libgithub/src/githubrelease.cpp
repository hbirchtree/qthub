#include <github/githubrelease.h>
#include <github/githubrepo.h>

GithubRelease::GithubRelease(QObject *parent) : QObject(parent)
{
}

GithubRepo *GithubRelease::repository() const
{
    return dynamic_cast<GithubRepo*>(parent());
}

void GithubRelease::addAsset(GithubAsset *asset)
{
    m_assets.push_back(asset);
}
