#include <github/githubrelease.h>

GithubRelease::GithubRelease(QObject *parent) : QObject(parent)
{

}

void GithubRelease::addAsset(GithubAsset *asset)
{
    m_assets.push_back(asset);
}
