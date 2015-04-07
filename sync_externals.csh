#!/bin/tcsh

git remote add -f coda-oss_remote https://github.com/mdaus/coda-oss.git

# To set this up the very first time
# This does a subtree merge and puts it in the externals/coda-oss directory.  --squash avoids copying all the history
# You can replace 'master' with any branch of CODA-OSS you want
# git subtree add --prefix externals/coda-oss coda-oss_remote master --squash

# Now we just want to update
# Here I'm assuming you're running this on the master branch... otherwise the push command should change
git subtree pull --prefix externals/coda-oss coda-oss_remote master --squash
git push origin master
