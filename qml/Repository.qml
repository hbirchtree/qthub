import QtQuick 2.4

RepositoryForm {
    function setData(repo)
    {
        reponame.text = repo.name
        language.text = repo.language
        description.text = repo.description
        repotitle.text = repo.title
    }
}
