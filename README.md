# linuxdeploy-plugin-qt

Plugin for linuxdeploy to bundle Qt dependencies of applications and libraries.


## About

linuxdeploy is a tool to create and maintain AppDirs, which can be turned into application bundles like AppImages.

linuxdeploy-plugin-qt is a bundling plugin for linuxdeploy. If an application uses Qt, linuxdeploy-plugin-qt will bundle all the Qt plugins and resources such as QML files or translations.

As linuxdeploy plugins are standalone applications, this software can also be run standalone. However, usage as a plugin from linuxdeploy is highly encouraged.


## Usage

As all linuxdeploy plugins, linuxdeploy-plugin-qt is a standalone tool implementing the so-called [Plugin Specification](https://github.com/linuxdeploy/linuxdeploy/wiki/Plugin-system). Therefore, there's two ways of using it: in "plugin mode" (i.e., together with linuxdeploy), or "standalone mode" (i.e., calling it directly).

The most widely and also recommended method is to use it together with linuxdeploy.


### Plugin mode

Just download the plugin's official AppImage, and put it next to linuxdeploy's AppImage (alternatively, put it into one of the other [search locations](https://github.com/linuxdeploy/linuxdeploy/wiki/Plugin-system#plugin-discovery)). Make sure the AppImage is executable, otherwise it cannot be called by linuxdeploy.

To enable the plugin, just call linuxdeploy as follows:

```bash
$ ./linuxdeploy-x86_64.AppImage --appdir AppDir [...] --plugin qt [...]
```

That's it! All you have to add is `--plugin qt`, and the plugin will be called by linuxdeploy.

**Note:** If the application doesn't use Qt, linuxdeploy-plugin-qt will return an error. That's expected behavior, as it might help discover issues when a program is expected to use Qt but suddenly does not any more.


### Standalone mode

To use linuxdeploy-plugin-standalone, download the official AppImage, make it executable and run it like:

```bash
./linuxdeploy-plugin-qt-x86_64.AppImage --appdir AppDir
```

linuxdeploy-plugin-qt will look for Qt libraries in the library directory `usr/lib/` and deploy the Qt plugins and other resources for these. This means that if linuxdeploy or another tool haven't been run on the AppDir yet, i.e., no Qt libraries have been deployed yet, linuxdeploy-plugin-qt won't be able to recognize which plugins and resources have to be deployed, and will return an error.



### Environment variables

Just like all linuxdeploy plugins, the Qt plugin's behavior can be configured some environment variables.

**General:**
- `$DEBUG=1`: enables verbose output, useful for debugging (equal to linuxdeploy's `-v0`)
- `$LD_LIBRARY_PATH=pathA:pathB`: Paths to check for library dependencies (see `man ld.so` for more information)

**Qt specific:**
- `$QMAKE=/path/to/my/qmake`: use another `qmake` binary to detect paths of plugins and other resources (usually doesn't need to be set manually, most Qt environments ship scripts changing `$PATH`)
- `$EXTRA_QT_PLUGINS=pluginA;pluginB`: Plugins to deploy even if not found automatically by linuxdeploy-plugin-qt

QML related:
- `$QML_SOURCES_PATHS`: directory containing the application's QML files — useful/needed if QML files are "baked" into the binaries. `$QT_INSTALL_QML` is prepended to this list internally.
- `$QML_MODULES_PATHS`: extra directories containing imported QML files (normally doesn't need to be specified).