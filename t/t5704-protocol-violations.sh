#!/bin/sh

test_description='Test responses to violations of the network protocol. In most
of these cases it will generally be acceptable for one side to break off
communications if the other side says something unexpected. We are mostly
making sure that we do not segfault or otherwise behave badly.'
. ./test-lib.sh

# If we don't print the object format, we'll fail for a spurious reason: the
# mismatched object format.
print_object_format () {
	local algo=$(test_oid algo) &&
	if test "$algo" != "sha1"
	then
		packetize "object-format=$algo"
	fi
}

test_expect_success 'extra delim packet in v2 ls-refs args' '
	{
		packetize command=ls-refs &&
		print_object_format &&
		printf 0001 &&
		# protocol expects 0000 flush here
		printf 0001
	} >input &&
	test_must_fail env GIT_PROTOCOL=version=2 \
		git upload-pack . <input 2>err &&
	test_i18ngrep "expected flush after ls-refs arguments" err
'

test_expect_success 'extra delim packet in v2 fetch args' '
	{
		packetize command=fetch &&
		print_object_format &&
		printf 0001 &&
		# protocol expects 0000 flush here
		printf 0001
	} >input &&
	test_must_fail env GIT_PROTOCOL=version=2 \
		git upload-pack . <input 2>err &&
	test_i18ngrep "expected flush after fetch arguments" err
'

test_done
