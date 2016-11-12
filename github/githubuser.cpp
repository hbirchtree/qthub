#include "githubuser.h"
#include "githubrepo.h"

GithubUser::GithubUser(QObject *parent) : QObject(parent)
{

}

void GithubUser::addRepository(GithubRepo *repo)
{
    m_repos.push_front(repo);
}
