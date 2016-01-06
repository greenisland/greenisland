Green Island
============

[![GitHub release](https://img.shields.io/github/release/greenisland/greenisland.svg)](https://github.com/greenisland/greenisland)
[![GitHub issues](https://img.shields.io/github/issues/greenisland/greenisland.svg)](https://github.com/greenisland/greenisland/issues)
[![IRC Network](https://img.shields.io/badge/irc-freenode-blue.svg "IRC Freenode")](https://webchat.freenode.net/?channels=hawaii-desktop)

QtQuick-based Wayland compositor in library form.

The name comes from [Kure Atoll, Hawaii](http://en.wikipedia.org/wiki/Green_Island,_Hawaii).

The API extends QtCompositor with additional features needed by any real world
Wayland compositor.

Green Island offers multiple screen support and it also implements specific
protocols such as xdg-shell, gtk-shell and those for Plasma 5.

Additional Wayland protocols for shells built with Green Island are provided,
such as greenisland_applications and greenisland_windows for task managers.

Also include a launcher that executes the compositor on different hardware,
a screencaster protocol and command line application, plus a
minimal Wayland compositor and shell written with QML.

It is primarily developed for the [Hawaii desktop environment](https://github.com/hawaii-desktop),
however it can be used by any desktop environment that wish to implement
its compositor by using QML or for shells deeply integrated with the compositor
in the same process.

## Dependencies

Compiler requirements:

* [gcc >= 4.8](https://gcc.gnu.org/gcc-4.8/) or
* [Clang](http://clang.llvm.org/)

Qt >= 5.5 with at least the following modules is required:

* [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
* [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
* [qtwayland](http://code.qt.io/cgit/qt/qtwayland.git)

The following modules and their dependencies are required:

* [ECM >= 1.4.0](http://quickgit.kde.org/?p=extra-cmake-modules.git)
* [udev](http://www.freedesktop.org/software/systemd/libudev/)
* [libinput >= 0.12](http://www.freedesktop.org/wiki/Software/libinput/)

Optional dependencies:

* [Weston >= 1.5.0](http://wayland.freedesktop.org) to nest Green Island into
  Weston with the fullscreen shell protocol, useful only if your device is not
  yet supported by Green Island but works with Weston.

If you enable DRM/KMS device integration you also need:

* [libdrm](https://wiki.freedesktop.org/dri/)
* [gbm](http://www.mesa3d.org)

If you enable Broadcom VideoCore device integration you also need:

* [bcmhost](https://github.com/raspberrypi/firmware)

If you enable Vivante device integration you also need:

* [mx6q](https://community.freescale.com/docs/DOC-95560)

If you enable XWayland support you also need:

* [xcb-util-cursor](http://cgit.freedesktop.org/xcb/util-cursor)

### qtwayland

Make sure qtwayland is built with the QtCompositor API, please read
upstream [documentation](http://code.qt.io/cgit/qt/qtwayland.git/tree/README?h=5.5).

## Build

Building Green Island is a piece of cake.

Assuming you are in the source directory, just create a build directory
and run cmake:

```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii ..
```

To do a debug build the last command can be:

```sh
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii -DCMAKE_BUILD_TYPE=Debug ..
```

To do a release build instead it can be:

```sh
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii -DCMAKE_BUILD_TYPE=Release ..
```

If not passed, the `CMAKE_INSTALL_PREFIX` parameter defaults to /usr/local.
You have to specify a path that fits your needs, /opt/hawaii is just an example.

Package maintainers would pass `-DCMAKE_INSTALL_PREFIX=/usr`.

The `CMAKE_BUILD_TYPE` parameter allows the following values:

* **Debug:** debug build
* **Release:** release build
* **RelWithDebInfo:** release build with debugging information

### Configuration arguments

* **ENABLE_EGLDEVICEINTEGRATION_KMS**

  Enabled by default. Pass `-DENABLE_EGLDEVICEINTEGRATION_KMS:BOOL=OFF`
  to cmake if you don't want to build the DRM/KMS device integration.

  Please note that this way you won't be able to run Green Island
  on DRM/KMS devices.

* **ENABLE_EGLDEVICEINTEGRATION_BRCM**

  Enabled by default. Pass `-DENABLE_EGLDEVICEINTEGRATION_BRCM:BOOL=OFF`
  to cmake in order to disable the Broadcom VideoCore device integration.

  You will need Broadcom VideoCore proprietary libraries.

  Notable examples of devices powered by a Broadcom GPU are
  Raspberry Pi and Raspberry Pi 2.

* **ENABLE_EGLDEVICEINTEGRATION_MALI**

  Disabled by default. Pass `-DENABLE_EGLDEVICEINTEGRATION_MALI:BOOL=ON`
  to cmake in order to enable Mali device integration.

  You will need Mali proprietary libraries.

* **ENABLE_EGLDEVICEINTEGRATION_VIV**

  Disabled by default. Pass `-DENABLE_EGLDEVICEINTEGRATION_VIV:BOOL=ON`
  to cmake in order to enable Vivante device integration.

  You will need Freescale proprietary Vivante GPU libraries.

* **ENABLE_XWAYLAND**

  Enabled by default. Pass `-DENABLE_XWAYLAND:BOOL=OFF` to cmake if
  you don't want to build the XWayland plugin.

  If disabled, compatibility with old X11-only applications will
  not work.

* **ENABLE_ONLY_EGLDEVICEINTEGRATION**

  Disabled by default. Pass `-DENABLE_ONLY_EGLDEVICEINTEGRATION:BOOL=ON`
  to cmake in order to build and install device integration plugins.

  This option is meant for packagers. The best way to package Green Island
  for multiple devices is to ship device integration plugins with separate
  packages.

## Installation

It's really easy, it's just a matter of typing:

```sh
make install
```

from the build directory.

# Notes

## Plugins

Green Island can be extend with plugins.
Specify a colon separated list of plugins with the ``GREENISLAND_PLUGINS``
environment variable.

List of plugins:

* **plasma:** Wayland protocols for Plasma
* **xwayland:** XWayland compatibility

xwayland plugin status is currently alpha, not everything
is guaranteed to work.

## Logging categories

Qt 5.2 introduced logging categories and Hawaii takes advantage of
them to make debugging easier.

Please refer to the [Qt](http://doc.qt.io/qt-5/qloggingcategory.html) documentation
to learn how to enable them.

### Available categories

* Compositor:
  * **greenisland.compositor:** Compositor
  * **greenisland.screenbackend.native:** Native screen backend
  * **greenisland.screenbackend.fake:** Fake screen backend
  * **greenisland.screenconfiguration:** Screen configuration loader

* Platform:
  * **greenisland.qpa.deviceintegration:** Device integration
  * **greenisland.qpa.input:** Input management on device integration plugins
  * **greenisland.qpa.logind:** Logind integration
  * **greenisland.qpa.udev:** udev integration

* Client-side:
  * **greenisland.wlregistry:** Registry
  * **greenisland.wlshmpool:** Shared memory pool

* Protocols:
  * **greenisland.protocols.xdgshell:** xdg_shell
  * **greenisland.protocols.xdgshell.trace:** xdg_shell trace
  * **greenisland.protocols.wlshell:** wl_shell
  * **greenisland.protocols.wlshell.trace:** wl_shell trace
  * **greenisland.protocols.wlsubcompositor.trace:** wl_subcompositor and wl_subsurface trace
  * **greenisland.protocols.gtkshell:** gtk_shell
  * **greenisland.protocols.gtkshell.trace:** gtk_shell trace
  * **greenisland.protocols.greenisland.recorder:** greenisland_recorder
  * **greenisland.protocols.greenisland.screenshooter:** greenisland_screenshooter
  * **greenisland.protocols.fullscreenshell.client:** wl_fullscreen_shell client side

* Plugins:
  * **greenisland.plugins.plasma.shell:** org_kde_plasma_shell protocol
  * **greenisland.plugins.plasma.effects:** org_kde_plasma_effects protocol
  * **greenisland.xwayland:** xwayland
  * **greenisland.xwayland.trace:** xwayland protocol trace
