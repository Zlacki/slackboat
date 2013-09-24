slackboat
=========

An IRC bot with a simple dumbed down core parent process and a bunch of modules as child processes for features to be implemented.

Released under CC0 Public Domain license.

Core handles I/O and manages the children processes.
It should be compatible with most, or all UNIX-like systems.
The bot was made for #pharmaceuticals on Whatnet and Rizon.

Modules are essentially plugins as Firefox has, but they communicate with IPC via UNIX sockets.
There will be APIs for most commonly used and some less commonly used languages for writing modules.
Most of the modules you'll find packaged on here are written in Scheme, a simplistic Lisp dialect.

Feel free to fork and do whatever with, and it'd be great if you sent a merge request for your mods to me.

Thanks.

 - Zach Knight
