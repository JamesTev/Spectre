## Spectre

Software for a low cost spectrometer using machine learning. This repo is divided into embedded and high level software (possibly a web interface, graphing dashboard etc).

### Basics 

Clone this repo and then perform all development in your branch. From the main develop branch, switch to develop-<name> by typing `git checkout develop-<name>` where name is your name (such as develop-james).

When you've got useful changes to incorporate, first merge develop into your personal develop branch by typing `git merge develop` while in your branch. Then, merge your branch into develop once you've resolved any merge conflicts (if applicable). To do this, change to develop branch using `git checkout develop` then `git merge develop-<name>`. We will merge to the main branch only to create new releases of our software.
