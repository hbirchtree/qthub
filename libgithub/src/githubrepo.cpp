#include <github/githubrepo.h>

GithubRepo::GithubRepo(QObject *parent) : QObject(parent)
{

}

void GithubRepo::addRelease(GithubRelease *rel)
{
    m_releases.push_back(rel);
    releaseAdded(rel);
}
