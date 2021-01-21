#! /bin/bash

DIRNAME=hexo-empty-dir

function linkFileHere {
	[[ -f $1 || -d $1 ]] && mv $1 $1.bak;
	ln -s ../$1 ./$1;
}

if [ ! -d $DIRNAME ]; then
	hexo init $DIRNAME
	
	pushd $DIRNAME
	
	npm install
	linkFileHere source
	linkFileHere themes
	linkFileHere _config.yml
	linkFileHere _config.next.yml
	
	popd
fi

pushd $DIRNAME
hexo g && hexo s
popd
