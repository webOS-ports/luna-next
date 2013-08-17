Luna Next
=========

Summary
-------
The new webOS UI of the webOS ports project.

Description
-----------
This is the repository of a prototype for a next generation webOS UI. It's meant to be a
complete replacement for LunaSysMgr/WebAppMgr and is completely based on well known open
source technologies like Qt 5 and Wayland.

CAUTION: At the moment it's not meant to be usable in any kind!

## Dependencies

In order to build and run luna-next you will need a complete
Wayland and Qt5 development environment.

The Wayland site has some information to set this up:

    http://wayland.freedesktop.org/building.html

More information about building Qt 5 can be found here:

    http://qt-project.org/wiki/Building-Qt-5-from-Git

## Build

Now you're ready to build luna-next with

```
mkdir build
(cd build ; cmake .. ; make)
```

Finally you can run luna-next with

`build/luna-next -platform xcb`

After the compositor is up you can launch a simple webapp client by

`test_webapp -a /usr/palm/applications/org.webosports.app.firstuse/index.html`

## Contributing

If you want to contribute you can just start with cloning the repository and make your
contributions. We're using a pull-request based development and utilizing github for the
management of those. All developers must provide their contributions as pull-request and
github and at least one of the core developers needs to approve the pull-request before it
can be merged.

Please refer to http://www.webos-ports.org/wiki/Communications for information about how to
contact the developers of this project.
