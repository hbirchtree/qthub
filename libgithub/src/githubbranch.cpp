#include <github/githubbranch.h>
#include <github/githubrepo.h>

GithubBranch::GithubBranch(QObject *parent)
    :QObject(parent)
{
}

GithubRepo *GithubBranch::repository() const
{
    return dynamic_cast<GithubRepo*>(parent());
}
