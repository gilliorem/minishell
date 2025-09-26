Sep 26, 12:51 pm
-
### GIT: Keep one clean version and 2 personal versions

#### Set up your branch
git checkout -b remi-branch

git add .
git commit -m "commit from remi-branch"
git push origin remi-branch

we keep the main branch clean compile-friendly until
we are both happy with changes.

#### Merging with main
git checkout main
git pull origin main  # make sure you're up to date
git merge remi-branch
git push origin main

