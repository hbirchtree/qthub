#include <github/githubreleasefile.h>
#include <github/githubrelease.h>

GithubAsset::GithubAsset(QObject *parent)
    :QObject(parent)
{
}

GithubRelease *GithubAsset::release() const
{
    return dynamic_cast<GithubRelease*>(parent());
}
