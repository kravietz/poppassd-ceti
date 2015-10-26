/*
 * poppassd.c
 *
 * A Eudora and NUPOP change password server.
 * 
 * Pawel Krawczyk <poppassd-bugs@nym.hush.com>
 *
 * Based on poppassd by John Norstad <j-norstad@nwu.edu>,
 * Roy Smith <roy@nyu.edu> and Daniel L. Leavitt <dll.mitre.org>.
 * Shadow file update code taken from shadow-960810 by John F. Haugh
 * II <jfh@rpp386.cactus.org> and Marek Michalkiewicz
 * <marekm@i17linuxb.ists.pwr.wroc.pl>
 *
 * See README for more information.
 */
 
/* Steve Dorner's description of the simple protocol:
 *
 * The server's responses should be like an FTP server's responses; 
 * 1xx for in progress, 2xx for success, 3xx for more information
 * needed, 4xx for temporary failure, and 5xx for permanent failure.  
 * Putting it all together, here's a sample conversation:
 *
 *   S: 200 hello\r\n
 *   E: user yourloginname\r\n
 *   S: 300 please send your password now\r\n
 *   E: pass yourcurrentpassword\r\n
 *   S: 200 My, that was tasty\r\n
 *   E: newpass yournewpassword\r\n
 *   S: 200 Happy to oblige\r\n
 *   E: quit\r\n
 *   S: 200 Bye-bye\r\n
 *   S: <closes connection>
 *   E: <closes connection>
 */
 
#define VERSION "1.8.6"
#define BAD_PASS_DELAY 	3   /* delay in seconds after bad 'Old password' */
#define POP_MIN_UID		100 /* minimum UID which is allowed to change
							   password via poppassd */

/* These need to be quoted because they are only used as
 * parts of format strings for sscanf; actual lengths are smaller
 * by 5 because the buffer needs to fit "user " and "pass " strings
 * as well
 */
#define MAX_LEN_USERNAME	"64"	/* maximum length of username buffer */
#define MAX_LEN_PASS		"128"	/* maximum length of password buffer */

#define SUCCESS 1
#define FAILURE 0
#define BUFSIZE 1000

#include <sys/types.h>
#include <fcntl.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <errno.h>
#include <stdarg.h>
#include <pwd.h>
#include <string.h>
#include <dirent.h>
#include <shadow.h>
#include <signal.h>
#include <sys/time.h>
#include <ulimit.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include "config.h"

#define LOCK_TRIES 30

/* need to be global for poppassd_conv */
char oldpass[BUFSIZE];
char newpass[BUFSIZE];
#define POP_OLDPASS 0
#define POP_NEWPASS 1
#define POP_SKIPASS 2
short int pop_state = POP_OLDPASS;
int pamret;

void WriteToClient (char *fmt, ...)
{
	va_list ap;
	
	va_start (ap, fmt);
	vfprintf (stdout, fmt, ap);
	fputs ("\r\n", stdout );
	fflush (stdout);
	va_end (ap);
}

void ReadFromClient (char *line)
{
	char *sp;

	bzero(line, BUFSIZE);
	if(fgets (line, BUFSIZE-1, stdin)) {
	    if ((sp = strchr(line, '\n')) != NULL) *sp = '\0'; 
	    if ((sp = strchr(line, '\r')) != NULL) *sp = '\0'; 
	
	    /* convert initial keyword on line to lower case. */
	
	    for (sp = line; isalpha(*sp); sp++) *sp = tolower(*sp);
	    line[BUFSIZE-1] = '\0';
    }
}

int poppassd_conv(num_msg, msg, resp, appdata_ptr)
    int         num_msg;
    const struct pam_message **msg;
    struct pam_response **resp;
    void        *appdata_ptr;
{
	int i;

	if(num_msg <= 0) 
		return(PAM_CONV_ERR);

	struct pam_response *r = malloc(sizeof(struct pam_response) * num_msg);

	if(r == NULL)
		return(PAM_CONV_ERR);

	for(i=0; i<num_msg; i++) {
		if(msg[i]->msg_style == PAM_ERROR_MSG) {
		       WriteToClient ("500 PAM error: %s", msg[i]->msg);
		       syslog(LOG_ERR, "PAM error: %s", msg[i]->msg);
		       /*
		        * If there is an error, we don't want to be sending in
		        * anything more, so set pop_state to invalid
		        */
		       pop_state = POP_SKIPASS;
		}

	r[i].resp_retcode = 0;
	if(msg[i]->msg_style == PAM_PROMPT_ECHO_OFF ||
			msg[i]->msg_style == PAM_PROMPT_ECHO_ON)
	{
		switch(pop_state) {
			case POP_OLDPASS: r[i].resp = strdup(oldpass);  
					  break;
			case POP_NEWPASS: r[i].resp = strdup(newpass); 
					  break;
			case POP_SKIPASS: r[i].resp = NULL;
					  break;
			default: syslog(LOG_ERR, "PAM error: too many switches (state=%d)", pop_state);
		}
	} else 
	{
		r[i].resp = strdup("");
	}
	}
	
	*resp = r;
	return PAM_SUCCESS;
}
	
const struct pam_conv pam_conv = {
	poppassd_conv,
	NULL
};

int main (int argc, char *argv[])
{
     char line[BUFSIZE];
     char user[BUFSIZE];
     struct passwd *pw, *getpwnam();
     pam_handle_t *pamh=NULL;
     
     *user = *oldpass = *newpass = 0;

     openlog("poppassd", LOG_PID, LOG_LOCAL4);

     WriteToClient ("200 poppassd");
     ReadFromClient (line);
     if( strlen(line) > atoi(MAX_LEN_USERNAME) ) {
	  WriteToClient ("500 Username too long (max %d)", atoi(MAX_LEN_USERNAME));
	  exit(1);
     }
     sscanf (line, "user %" MAX_LEN_USERNAME "s", user) ;
     if (strlen (user) == 0 )
     {
	  WriteToClient ("500 Username required");
	  exit(1);
     }
     if(pam_start("poppassd", user, &pam_conv, &pamh) != PAM_SUCCESS) 
     {
	     WriteToClient("500 Invalid username");
	     exit(1);
     }

     WriteToClient ("200 Your password please");
     ReadFromClient (line);
     if( strlen(line) > atoi(MAX_LEN_PASS) )
     {
	     WriteToClient("500 Password length exceeded (max %d)",
			     atoi(MAX_LEN_PASS) );
	     exit(1);
     }
     sscanf (line, "pass %" MAX_LEN_PASS "c", oldpass);
     if(strlen(oldpass) == 0) 
     {
	     WriteToClient("500 Password required");
	     exit(1);
     }
     pamret = pam_authenticate(pamh, 0);
     if(pamret != PAM_SUCCESS)
     {
          /* pause to make brute force attacks harder */
          sleep(BAD_PASS_DELAY);
	      WriteToClient ("500 Old password is incorrect");
          syslog(LOG_ERR, "old password is incorrect for user %s", user);
	      exit(1);
     }

     pw = getpwnam(user);

     if(pw == NULL || pw->pw_uid < POP_MIN_UID) {
         WriteToClient("500 Old password is incorrect");
         syslog(LOG_ERR, "failed attempt to change password for %s", user);
         exit(1);
     }

     pop_state = POP_NEWPASS;

     WriteToClient ("200 Your new password please");
     ReadFromClient (line);
     sscanf (line, "newpass %" MAX_LEN_PASS "c", newpass);
     
     /* new pass required */
     if (strlen (newpass) == 0)
     {
	  WriteToClient ("500 New password required");
	  exit(1);
     }

     pamret = pam_chauthtok(pamh, PAM_SILENT);
     if(pamret != PAM_SUCCESS) {
         WriteToClient("500 Server pam_chauthtok error %i, password not changed", pamret);
	     exit(1);
     } else {
     		syslog(LOG_ERR, "changed POP3 password for %s", user);
     		WriteToClient("200 Password changed");
     		ReadFromClient (line);
     		if (strncmp(line, "quit", 4) != 0) {
      			WriteToClient("500 Quit requested");
     			exit (1);
		}
     }
	  
     pam_end(pamh, 0);
     WriteToClient("200 Bye");
     closelog();
     exit(0);
     }

