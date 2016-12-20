#ifndef GITHUBCLI_LAMBDAS_H
#define GITHUBCLI_LAMBDAS_H

#include <github/github.h>
#include <functional>
#include <iostream>

struct ProcessingContext
{
    /* Objects */
    GithubFetch* github_daemon;

    /* Slots */
    std::function<void(GithubRepo*)> list_repo;
    std::function<void(GithubRepo*,GithubBranch*)> list_branch;
    std::function<void(GithubRepo*,GithubRelease*)> list_release;
    std::function<void(GithubRepo*,GithubTag*)> list_tag;
    std::function<void(GithubRelease*,GithubAsset*)> list_asset;

    std::function<void(GithubUser*)> show_user;
    std::function<void(GithubRepo*)> show_repo;
    std::function<void(GithubRepo*,GithubRelease*)> show_release;
    std::function<void(GithubRepo*,GithubBranch*)> show_branch;
    std::function<void(GithubRepo*,GithubTag*)> show_tag;
    std::function<void(GithubRepo*,GithubTag*)> show_issue;
    std::function<void(GithubRelease*,GithubAsset*)> show_asset;
    std::function<void(GithubRelease*,GithubAsset*)> show_pr;

    std::function<void(GithubUser*)> get_user_repos;
    std::function<void(GithubRepo*)> get_repo_releases;
    std::function<void(GithubRepo*)> get_repo_branches;
    std::function<void(GithubRepo*)> get_repo_tags;
    std::function<void(GithubRepo*)> get_repo_prs;
    std::function<void(GithubRepo*)> get_repo_issues;

    std::function<void(GithubRepo*,GithubRelease*)> delete_release;
    std::function<void(GithubRepo*,GithubTag*)> delete_tag;
    std::function<void(GithubRepo*,GithubRelease*)> delete_pr;
    std::function<void(GithubRepo*,GithubRelease*)> delete_issue;
    std::function<void(GithubRelease*,GithubAsset*)> delete_asset;

    std::function<void(GithubRepo*)> pull_repo;
    std::function<void(GithubRepo*,GithubBranch*)> pull_branch;
    std::function<void(GithubRepo*,GithubRelease*)> pull_release;
    std::function<void(GithubRepo*,GithubTag*)> pull_tag;
    std::function<void(GithubRelease*,GithubAsset*)> pull_asset;

    std::function<void(GithubRepo*,GithubRelease*)> push_release;
    std::function<void(GithubRepo*,GithubTag*)> push_tag;
    std::function<void(GithubRepo*,GithubRelease*)> push_pr;
    std::function<void(GithubRepo*,GithubRelease*)> push_issue;
    std::function<void(GithubRelease*,GithubAsset*)> push_asset;
};

void PopulateProcessingContext(ProcessingContext* ctxt, GithubFetch& github_daemon, std::string sep)
{
    /* Listing functions */
    auto list_repo = [&](GithubRepo* r)
    {
        std::cout << r->name().toStdString() << std::endl;
    };
    auto list_release = [&](GithubRepo* r, GithubRelease* rl)
    {
        std::cout << rl->id() << sep
                  << r->name().toStdString() << sep
                  << rl->tagName().toStdString()
                  << std::endl;
    };
    auto list_tag = [&](GithubRepo* r, GithubTag* tag)
    {
        std::cout << r->name().toStdString() << sep
                  << tag->name().toStdString() << sep
                  << tag->commit().toStdString()
                  << std::endl;
    };
    auto list_asset = [&](GithubRelease* rel, GithubAsset* asset)
    {
        std::cout
                << rel->repository()->name().toStdString() << sep
                << rel->tagName().toStdString() << sep
                << asset->id() << sep
                << rel->name().toStdString() << sep
                << asset->name().toStdString() << sep
                << std::endl;
    };
    auto list_branch = [&](GithubRepo*, GithubBranch* branch)
    {
        std::cout
                << branch->name().toStdString() << sep
                << branch->commit().toStdString()
                << std::endl;
    };

    /* Show functions */
    auto show_user = [&](GithubUser* u)
    {
        std::cout
                << u->login().toStdString() << sep
                << u->name().toStdString() << sep
                << u->registered().toString().toStdString()
                << std::endl;
    };
    auto show_repo = [&](GithubRepo* r)
    {
        std::cout
                << r->name().toStdString() << sep
                << r->description().toStdString() << sep
                << r->language().toStdString() << sep
                << r->created().toString().toStdString()
                << std::endl;
    };
    auto show_release = [&](GithubRepo*, GithubRelease* r)
    {
        std::cout
                << r->id() << sep
                << r->name().toStdString() << sep
                << r->tagName().toStdString()
                << std::endl;
    };
    auto show_tag = [&](GithubRepo*, GithubTag* tag)
    {
        std::cout
                << tag->name().toStdString() << sep
                << tag->commit().toStdString() << sep
                << tag->tarballUrl().toString().toStdString()
                << std::endl;
    };
    auto show_asset = [&](GithubRelease* r, GithubAsset* a)
    {
        std::cout
                << r->repository()->name().toStdString() << sep
                << r->tagName().toStdString() << sep
                << a->id() << sep
                << a->name().toStdString() << sep
                << a->size() << sep
                << a->label().toStdString() << sep
                << a->downloadUrl().toString().toStdString()
                << std::endl;
    };
    auto show_branch = [&](GithubRepo* repo, GithubBranch* branch)
    {
        std::cout
                << repo->name().toStdString() << sep
                << branch->name().toStdString() << sep
                << branch->commit().toStdString()
                << std::endl;
    };

    /* Deleting resources */
    auto delete_release = [&](GithubRepo*, GithubRelease* rl)
    {
        std::cout
                << rl->id() << sep
                << rl->tagName().toStdString() << sep
                << std::endl;
        github_daemon.requestDelete(rl);
    };
    auto delete_tag = [&](GithubRepo*, GithubTag* tag)
    {
        std::cout
                << tag->name().toStdString() << sep
                << tag->commit().toStdString()
                << std::endl;
        github_daemon.requestDelete(tag);
    };
    auto delete_asset = [&](GithubRelease*, GithubAsset* asset)
    {
        std::cout
                << asset->id() << sep
                << asset->name().toStdString()
                << std::endl;
    };

    /* Deepening functions */
    auto get_repo_releases = [&](GithubRepo* repo)
    {
        github_daemon.fetchAllReleases(repo);
    };
    auto get_repo_branches = [&](GithubRepo* repo)
    {
        github_daemon.fetchAllBranches(repo);
    };
    auto get_repo_tags = [&](GithubRepo* repo)
    {
        github_daemon.fetchAllTags(repo);
    };
    auto get_user_repos = [&](GithubUser* user)
    {
        github_daemon.fetchAllRepositories(user);
    };

    /* Pull functions */
    auto pull_asset = [&](GithubRelease*, GithubAsset* asset)
    {
        std::cout
                << asset->id() << sep
                << asset->name().toStdString() << sep
                << std::endl;
        github_daemon.requestDownload(asset);
    };
    auto pull_branch = [&](GithubRepo* repo, GithubBranch* ref)
    {
        std::cout
                << ref->name().toStdString()
                << std::endl;
        github_daemon.requestDownload(repo, ref);
    };
    auto pull_tag = [&](GithubRepo*, GithubTag* tag)
    {
        std::cout
                << tag->name().toStdString() << sep
                << tag->commit().toStdString()
                << std::endl;
        github_daemon.requestDownload(tag);
    };
    auto pull_release = [&](GithubRepo*, GithubRelease* rel)
    {
        std::cout
                << rel->tagName().toStdString() << sep
                << rel->branch().toStdString() << sep
                << rel->id()
                << std::endl;
        github_daemon.requestDownload(rel);
    };
    auto pull_repo = [&](GithubRepo* repo)
    {
        std::cout
                << repo->name().toStdString() << sep
                << repo->branch().toStdString()
                << std::endl;
        github_daemon.requestDownload(repo);
    };

    /* Push functions */

    ctxt->list_repo = list_repo;
    ctxt->list_release = list_release;
    ctxt->list_tag = list_tag;
    ctxt->list_asset = list_asset;
    ctxt->list_branch = list_branch;

    ctxt->get_user_repos = get_user_repos;
    ctxt->get_repo_releases = get_repo_releases;
    ctxt->get_repo_branches = get_repo_branches;
    ctxt->get_repo_tags = get_repo_tags;

    ctxt->show_user = show_user;
    ctxt->show_repo = show_repo;
    ctxt->show_release = show_release;
    ctxt->show_tag = show_tag;
    ctxt->show_asset = show_asset;
    ctxt->show_branch = show_branch;

    ctxt->delete_release = delete_release;
    ctxt->delete_tag = delete_tag;
    ctxt->delete_asset = delete_asset;

    ctxt->pull_repo = pull_repo;
    ctxt->pull_branch = pull_branch;
    ctxt->pull_asset = pull_asset;
    ctxt->pull_release = pull_release;
    ctxt->pull_tag = pull_tag;
}

#endif
