poppassd-ceti
=============

An Eudora and NUPOP change password server that allows user password change on PAM based systems via a simple POP3-like protocol. This daemon is frequently used as a backend for web based password change interfaces.

Features
--------
* Uses [PAM (Pluggable Authentication Module)](https://en.wikipedia.org/wiki/Pluggable_authentication_module) so all system-wide controls (such as password quality auditing) are still enforced
* Does not call any external programs with SUID permissions (as CGI wrappers for `passwd`  do)
* Simple, clean code with no known security issues since 2002

Security model
--------------
Poppassd should be only listening on local network interface (`localhost`). The program does not have any access control restrictions and these need to be implemented using operating system's native daemons, such as `inetd` and `tcpd`.

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

Integration with web applications requires that the application  connects to `localhost` on port `106/tcp` and speaks the above protocol using the data supplier by the user.

Installation
------------
Prerequisites:

* RPM based  (RedHat, CentOS etc):  `yum install pam pam-devel`
* APT based  (Debian, Ubuntu etc): `apt-get install libpam0g libpam0g-dev`

Installation from source:

    ./configure
    make
    sudo make install

By default, this will install:

* `poppassd` binary to `/usr/local/sbin`
* `poppassd` [PAM](https://en.wikipedia.org/wiki/Pluggable_authentication_module) configuration file to `/etc/pam.d`
* `poppassd` [Xinetd](http://www.xinetd.org/) configuration file to `/etc/xinetd.d`

Testing is simple as `poppassd` works on standard input:

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
    
If it does not work, check `/var/log/auth.log` in the first place.
 
Credits
-------
This program was initially based on poppassd by John Norstad <j-norstad@nwu.edu>, Roy Smith <roy@nyu.edu> and Daniel L. Leavitt <dll@mitre.org>. Shadow file update code taken from shadow-960810 by John F. Haugh II <jfh@rpp386.cactus.org> and Marek Michalkiewicz <marekm@i17linuxb.ists.pwr.wroc.pl>. A number of people (listed below) have contributed with suggestions and fixes.

Versions
--------
* Version 1.8.6 - František Hanzlík helped me refresh `poppassd` and move it to GitHub
* Version 1.8.4 - Steven Danz fixed one bug where PAM errors (like cracklib complaints) were actually not preventing the user from changing the password.  Now, if cracklib reports a weak password, it won't be accepted.  To return to the previous default behaviour, remove cracklib from poppassd PAM configuration.
* Version 1.8.3 - has changed the default PAM service name from "passwd" to "poppassd" (Radoslaw Antoniuk) and some more cleanups on password and username length (Mihail Vidiassov). Also added configuration hints from Brian Kircher.
* Version 1.8.2 - has some cleanups in maximum username and password length checking and more verbose logging. It also supports passwords  with space inside thanks to suggestion from Adam Conrad.
* Version 1.7 - uses PAM. Thanks to Mikolaj Rydzewski <mikir@kki.net.pl> for giving me a clue how to use PAM conversion function in his wwwpasswd.
* Version 1.5 - fixes bug which caused usernames containing characters like underscore "_" to be ignored. I've also added new compilation flag `-DALLOW_NULL_PASSWORDS` (needed for automated accounts creation).
* Version 1.0 - works on shadow files directly
