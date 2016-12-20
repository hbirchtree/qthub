#include <github/githubrepo.h>
#include <github/githubtag.h>
#include <github/githubrelease.h>

GithubRepo::GithubRepo(QObject *parent) : QObject(parent)
{

}

GithubTag *GithubRepo::tag(const QString &name) const
{
    for(GithubTag* t : m_tags)
        if(t->name() == name)
            return t;
    return nullptr;
}

GithubRelease *GithubRepo::release(quint64 id) const
{
    for(GithubRelease* r : m_releases)
        if(r->id() == id)
            return r;
    return nullptr;
}

void GithubRepo::addRelease(GithubRelease *rel)
{
    m_releases.push_back(rel);
    releaseAdded(rel);
}

void GithubRepo::addTag(GithubTag *tag)
{
    m_tags.push_back(tag);
    tagAdded(tag);
}

void GithubRepo::addBranch(GithubBranch *branch)
{
    m_branches.push_back(branch);
    branchAdded(branch);
}
