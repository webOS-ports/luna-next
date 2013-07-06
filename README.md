Summary
=======

This is the repository of a prototype for a next generation webOS UI.

How to Build on Linux
=====================

Currently this is only support for an Ubuntu system either 12.10 or 13.04.

Install the following ppa's

* ppa:canonical-qt5-edgers/qt5-proper
* ppa:ubuntu-sdk-team

Install the following packages

```
sudo apt-get install qtbase5-private-dev libwayland-dev libxkbcommon-dev \
libgl1-mesa-dev libegl1-mesa-dev libgles2-mesa-dev libxcomposite-dev \
libxrender-dev libudev-dev qt5-default pkg-config pkg-kde-tools \
libfontconfig1-dev libfreetype6-dev libglib2.0-dev libqt5v8-5-dev
```

Get qtwayland

```
git clone git://gitorious.org/qt/qtwayland.git
cd qtwayland
```

checkout require SRCREV

`git checkout -b work 5cb159395eccb1d96fb73a78e499eef30aacb46d`

build it with

```
echo "MODULE_VERSION = 0.0.0" >> .qmake.conf
qmake CONFIG+=wayland-compositor && make
```

finally you have to install it to your environment with

`sudo make install`

Now you're ready to build luna-next with

`qmake && make`

Finally you can launch it with

`./luna-next -platform xcb`

After the compositor is up you can launch a simple client by

`qmlscene -platform wayland-egl tests/simple-scene.qml`
