slackboat
=========

An IRC bot with a simple dumbed down core parent process and a bunch of modules as child processes for features to be implemented.

Released under ISC license.  The single restriction of the ISC license is that you leave my copyright notice visible to end users on your project if you distribute it.  I realize you can easily ignore my rights to this work, but I request you respect my rights.

Core handles I/O and launches stand-alone applications that communicate commands to the core via stdout plaintext.
It should be compatible with most UNIX-like systems.

The way features are going to be implemented is through stand-alone applications that print commands for the core to interpret, like an API via stdout.
This implies an instant limitation: it is unidirectional IPC.  I can not communicate to the modules during runtime.
To get around this, I will be using command-line arguments and document for all how to implement modules/plugins based on that API.

Most of the modules you'll find packaged on here will be written in Scheme, a simplistic Lisp dialect.

Feel free to fork and do whatever with, and it'd be great if you sent a merge request for your modifications to me, under any license that is non-restrictive(e.g no GPL, no proprietary.  Public domain and BSD-like licenses work.), and I will obviously respect your rights over the contribution and if necessary multi-license the project, but if you use ISC license I will simply add your details to the list of owners of the copy rights.

Thanks.

 - Zach Knight
