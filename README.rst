====================================
 File system notification forwarder
====================================

A simple tool for forwarding file system notifications from one host to another, possibly remapping
the path in the process. Useful when running a reloading build system on one machine and editing
files on another, or over shared folders to a VM.

Supports Linux/inotify and Mac OS X/fsevents on the host side, and Linux, Mac OS X, and BSD on the
client side.


Downloading
===========

Either use one of the pre-built binary releases for your operating system:

* Mac OS X 10.10 or newer, 64 bit: notify-forwarder_osx_x64_
* Linux 64 bit: notify-forwarder_linux_x64_
* FreeBSD 64 bit: notify-forwarder_freebsd_x64_

Or download the source and compile it yourself:

.. code-block:: sh

   $ git clone git@github.com:mhallin/notify-forwarder.git
   $ make
   # Now you have the binary at _build/notify-forwarder


Running
=======

To map the current directory to "/home/user/project" on a remote machine, run the following on the
host side:

.. code-block:: sh

   $ notify-forwarder watch -c <remote-ip> . /home/user/project
   # This maps the current directory "." to "/home/user/project". Add more pairs of paths
   # to watch more folders with different mappings.

And on the client side:

.. code-block:: sh

   $ notify-forwarder receive

The "host side" here being the file system that will send events, i.e. where you run the text
editor. The "client side" is where the auto reloading build system runs.



.. _notify-forwarder_osx_x64: https://github.com/mhallin/notify-forwarder/releases/download/release%2Fv0.1.0/notify-forwarder_osx_x64
.. _notify-forwarder_linux_x64: https://github.com/mhallin/notify-forwarder/releases/download/release%2Fv0.1.0/notify-forwarder_linux_x64
.. _notify-forwarder_freebsd_x64: https://github.com/mhallin/notify-forwarder/releases/download/release%2Fv0.1.0/notify-forwarder_freebsd_x64
