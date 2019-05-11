#! /bin/bash

CUR_DIR=$PWD
DATE_NOW=`date --rfc-2822`
REPO_URL="git@github.com:lixingcong/lixingcong.github.io.git"
REPO_BRANCH="master"

cd public
rm -rf .git
git init
git add *
git commit -m "update: $DATE_NOW"

git remote add origin $REPO_URL
git push -u origin $REPO_BRANCH --force

cd $CUR_DIR
