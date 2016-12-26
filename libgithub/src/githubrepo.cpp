#include <github/githubrepo.h>
#include <github/githubtag.h>
#include <github/githubrelease.h>
#include <github/githubbranch.h>
#include <github/githubcommit.h>

GithubRepo::GithubRepo(QObject *parent) : QObject(parent)
{

}

GithubTag *GithubRepo::tag(const QString &name) const
{
    return m_tags[name];
}

GithubRelease *GithubRepo::release(quint64 id) const
{
    return m_releases[id];
}

GithubBranch *GithubRepo::branch(const QString &name) const
{
    return m_branches[name];
}

GithubCommit *GithubRepo::commit(const QString &sha) const
{
    return m_commits[sha];
}

void GithubRepo::addRelease(GithubRelease *rel)
{
    m_releases[rel->id()] = rel;
    releaseAdded(rel);
}

void GithubRepo::addTag(GithubTag *tag)
{
    m_tags[tag->name()] = tag;
    tagAdded(tag);
}

void GithubRepo::addBranch(GithubBranch *branch)
{
    m_branches[branch->name()] = branch;
    branchAdded(branch);
}

void GithubRepo::addCommit(GithubCommit *commit)
{
    m_commits[commit->sha()] = commit;
    commitAdded(commit);
}
