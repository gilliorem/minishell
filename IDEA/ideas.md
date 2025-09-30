Sep 30, 7:30PM

Dude this project is quite vast - I've been reading for the last 4 days and there is still lots to go

I know some ppl like to see action and want to just start somehwere but this needs a more targeted approach - I'd give at least a week more to understand this fully before we get coding in full swing

Whatsay?


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

