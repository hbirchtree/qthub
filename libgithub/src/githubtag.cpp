#include <github/githubtag.h>
#include <github/githubrepo.h>

GithubTag::GithubTag(QObject *parent):
    QObject(parent)
{

}

GithubRepo *GithubTag::repository() const
{
    return dynamic_cast<GithubRepo*>(parent());
}
