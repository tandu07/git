#!/bin/sh

test_description='bisect can handle submodules'

. ./test-lib.sh
. "$TEST_DIRECTORY"/lib-submodule-update.sh

git_bisect () {
	git status -su >expect &&
	ls -1pR * >>expect &&
	tar cf "$TRASH_DIRECTORY/tmp.tar" * &&
	GOOD=$(git rev-parse --verify HEAD) &&
	$OVERWRITING_FAIL git checkout "$1" &&
	if test -z "$OVERWRITING_FAIL"
	then
		echo "foo" >bar &&
		git add bar &&
		git commit -m "bisect bad" &&
		BAD=$(git rev-parse --verify HEAD) &&
		git reset --hard HEAD^^ &&
		git submodule update &&
		git bisect start &&
		git bisect good $GOOD &&
		rm -rf * &&
		tar xf "$TRASH_DIRECTORY/tmp.tar" &&
		git status -su >actual &&
		ls -1pR * >>actual &&
		test_cmp expect actual &&
		git bisect bad $BAD
	fi
}

test_submodule_switch_func "git_bisect"

test_done
