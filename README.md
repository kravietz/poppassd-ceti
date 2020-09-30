[![Coverity](https://scan.coverity.com/projects/5500/badge.svg)](https://scan.coverity.com/projects/5500)
[![LGTM](https://img.shields.io/lgtm/alerts/g/kravietz/poppassd-ceti.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/kravietz/poppassd-ceti/alerts/)
[![Build Status](https://travis-ci.org/kravietz/poppassd-ceti.svg?branch=master)](https://travis-ci.org/kravietz/poppassd-ceti)
[![GitHub license](https://img.shields.io/badge/license-GPLv2-blue.svg)](https://github.com/kravietz/poppassd-ceti/blob/master/LICENSE)
[![builds.sr.ht status](https://builds.sr.ht/~kravietz/poppassd-ceti.svg)](https://builds.sr.ht/~kravietz/poppassd-ceti?)

poppassd-ceti
=============

An Eudora and NUPOP change password server that allows user password change on PAM based systems via a simple POP3-like protocol.
This daemon is frequently used as a backend for web based password change interfaces.

Features
--------
* Uses [PAM (Pluggable Authentication Module)](https://en.wikipedia.org/wiki/Pluggable_authentication_module) so all system-wide controls (such as password quality restrictions) are still enforced
* Does not call any external programs with SUID permissions (as CGI wrappers for `passwd`  do)
* Simple, clean code with no known security issues since 2002

Security model
--------------
Poppassd operates over standard input and output exclusively. Network socket is expected to be handled by [systemd.socket](https://www.freedesktop.org/software/systemd/man/systemd.socket.html)
(historically `inetd` or `xinetd`).  Authentication and password change are handled exclusively by PAM.

In the intended usage model of remote network applications or local web applications connect to the `poppassd` port over TCP, which creates a clear trust boundary and avoids potentially dangerous
shell script operations using SUID or `expect` as seen in some other solutions to the same problem.

Systemd `poppassd.service` which actually runs the `poppassd` binary uses a reasonable set of [systemd hardening](https://krvtz.net/posts/reducing-your-attack-surface-with-systemd.html) features to further reduce the attack surface on modern systems.

Configuration
-------------
Poppassd uses PAM definitions from `/etc/pam.d/poppassd` for authentication of users and password change. Any local policies such as LDAP authentication, login times, password quality enforcement
should be configured there per [PAM System Administrator's Guide](http://www.linux-pam.org/Linux-PAM-html/Linux-PAM_SAG.html). Default PAM configuration does *not* have any provisions to
restrict number of login attempts per user, so this must be configured in PAM as well. The daemon itself however introduces a delay after each unsuccessful login attempt which to some
extent reduces effectiveness of password bruteforcing.

Use `systemctl edit --full poppassd.socket` to change default listening port (default `106/tcp`) or bind address (default `localhost`).

Protocol
--------
Poppassd implements a simple, text based protocol for user authentication and password change:

    200 poppassd
    USER username
    200 Your password please
    PASS old_password
    200 Your new password please
    NEWPASS new_password
    200 Password changed
    QUIT
    200 Bye

Server responses starting with `200` are successs, `500` are errors:

    200 poppassd
    USER username
    200 Your password please.
    PASS old_password
    500 Old password is incorrect

Integration with web applications requires that the application  connects over TCP `localhost:106` and speaks the above protocol using the data supplier by the user.

Installation
------------
Prerequisites:

* RPM based  (RedHat, CentOS etc):  `yum install pam pam-devel cmake`
* APT based  (Debian, Ubuntu etc): `apt-get install libpam0g libpam0g-dev cmake`

Installation from source:

    git clone https://github.com/kravietz/poppassd-ceti.git
    cd poppassd-ceti
    cmake .
    make
    sudo make install

Since version 1.8.9 the default deployment method is [systemd.socket](https://www.freedesktop.org/software/systemd/man/systemd.socket.html): `systemd` handles the port
`106/tcp` and starts `poppassd@.service` instance on new connection. Locations of installed files:

* `/usr/local/sbin/poppassd`
* `/etc/systemd/system/poppassd.socket`
* `/etc/systemd/system/poppassd.service`
* `/etc/pam.d/poppassd`

Testing
-------
Testing is as simple as `poppassd` works on standard input (as `root`):

    sudo /usr/local/sbin/poppassd
    200 poppassd
    USER kravietz
    200 Your password please
    PASS wie9on2cheB7oojeokai
    200 Your new password please
    NEWPASS eW4ieLieYieN6iefaith
    200 Password changed
    QUIT
    200 Bye 
    
If it does not work, check `journalctl -xe` or `/var/log/auth.log` on old systems. The most frequent problem are PAM configuration issues.

If it works locally but doesn't work over `localhost:106` you may need to use `systemctl edit --full poppassd.service` and disable some of the
`systemd` hardening settings as they might be too restrictive on your system.
 
Credits
-------
This program was initially based on poppassd by John Norstad <j-norstad@nwu.edu>, Roy Smith <roy@nyu.edu> and Daniel L. Leavitt <dll@mitre.org>. Shadow file update code taken from shadow-960810 by John F. Haugh II <jfh@rpp386.cactus.org> and Marek Michalkiewicz <marekm@i17linuxb.ists.pwr.wroc.pl>. A number of people (listed below) have contributed with suggestions and fixes.

Versions
--------
* Version 1.8.10 - Include more portable PAM configuration, build fixes for FreeBSD
* Version 1.8.9 - Improve error handling and reporting, add configuration files for `systemd` (@imilos), documentation updates 
* Version 1.8.8 - Now getstate() takes care of PAM message interpretation, better handling of PAM LDAP and Cracklib modules by Jinesh K J <jinesh_kj@rediffmail.com>
* Version 1.8.7 - Peter Colberg optimized the build configuration to make distribution builds easier
* Version 1.8.6 - František Hanzlík helped me refresh `poppassd` and move it to GitHub; the program is now compiled with autotools and includes several security improvements
* Version 1.8.4 - Steven Danz fixed one bug where PAM errors (like cracklib complaints) were actually not preventing the user from changing the password.  Now, if cracklib reports a weak password, it won't be accepted.  To return to the previous default behaviour, remove cracklib from poppassd PAM configuration.
* Version 1.8.3 - has changed the default PAM service name from "passwd" to "poppassd" (Radoslaw Antoniuk) and some more cleanups on password and username length (Mihail Vidiassov). Also added configuration hints from Brian Kircher.
* Version 1.8.2 - has some cleanups in maximum username and password length checking and more verbose logging. It also supports passwords  with space inside thanks to suggestion from Adam Conrad.
* Version 1.7 - uses PAM. Thanks to Mikolaj Rydzewski <mikir@kki.net.pl> for giving me a clue how to use PAM conversion function in his wwwpasswd.
* Version 1.5 - fixes bug which caused usernames containing characters like underscore "_" to be ignored. I've also added new compilation flag `-DALLOW_NULL_PASSWORDS` (needed for automated accounts creation).
* Version 1.0 - works on shadow files directly
