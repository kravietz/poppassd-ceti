#!/bin/bash

# This test script is expected to run in Multipass or Docker image
# under test environment as run by Travis CI for example - it depends
# on installing packages, adding users and sudo

set -exo pipefail

sudo useradd test1 || true
echo test1:test1 | sudo chpasswd

expect <<_EOT
set timeout -1
spawn sudo ./poppassd
match_max 100000
expect -exact "200 poppassd\r\r
"
send -- "user test1\r"
expect -exact "user test1\r
200 Your password please\r\r
"
send -- "pass test1\r"
expect -exact "pass test1\r
200 Your new password please\r\r
"
send -- "newpass test2\r"
expect -exact "newpass test2\r
200 Password changed\r\r
"
send -- "quit\r"
expect eof

spawn sudo ./poppassd
match_max 100000
expect -exact "200 poppassd\r\r
"
send -- "user test1\r"
expect -exact "user test1\r
200 Your password please\r\r
"
send -- "pass test1\r"
expect eof
_EOT

