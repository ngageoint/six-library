#!/bin/csh -f

git remote add -f coda-oss_remote git@github.com:mdaus/coda-oss.git

# To set this up the very first time
# This does a subtree merge and puts it in the externals/coda-oss directory.  --squash avoids copying all the history
# You can replace 'master' with any branch of CODA-OSS you want
# git subtree add --prefix externals/coda-oss coda-oss_remote master --squash

# Then, let's get rid of some externals we don't care about
# git is smart, so we only have to do this once and then future subtree pulls will never
# pull these in
# git rm -r externals/coda-oss/modules/drivers/fftw externals/coda-oss/modules/drivers/openssl externals/coda-oss/modules/drivers/pcre externals/coda-oss/modules/drivers/sql externals/coda-oss/modules/drivers/uuid externals/coda-oss/modules/drivers/zlib

# Now we just want to update
# Here I'm assuming you're running this on the master branch... otherwise the push command should change
git subtree pull --prefix externals/coda-oss coda-oss_remote cpp17 --squash

# If when you do this command you git a merge conflict because a file that has been removed here has been updated in CODA-OSS, you just need to do a 'git rm <pathname>' to resolve the merge conflict.  Then a 'git commit'.
# TODO: Make this script smart enough to do this.

#git push origin master
