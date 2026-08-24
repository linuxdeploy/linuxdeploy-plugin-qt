# linuxdeploy-plugin-qt

Plugin for linuxdeploy to bundle Qt dependencies of applications and libraries. It supports the Qt versions 5 and 6.


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


### Translations
#### Qt Translations
Translation of Qt libraries (usually accessible at `/usr/share/qt{5,6}/translations/`) is split into the following categories:

| Category                        | CLI enable                  | CLI disable                    | Env variable                     |
| ------------------------------- | --------------------------- | ------------------------------ | -------------------------------- |
| Individual library translations | `--individual-translations` | `--no-individual-translations` | `TRANSLATIONS_INDIVIDUAL=YES/NO` |
| Merged library translations     | `--merged-translations`     | `--no-merged-translations`     | `TRANSLATIONS_MERGED=YES/NO`     |

Individual library translations copy over individual `.qm` files into standard translation directory (`<AppDir>/share/translations`, retrievable by calling `QLibraryInfo::path(QLibraryInfo::TranslationsPath)` from within program). For example, if the program is using Core and Multimedia modules, `qtbase_cs.qm`, `qtmultimedia_cs.qm`, `qtbase_de.qm`, `qtmultimedia_de.qm`... will get copied over.

Merged library translations will produce a `qt_<lang>.qm` file into standard translation directory. This is consistent with for example how `windeployqt.exe` Qt official deployer deploys translations.

By default, all available translations matching the Qt libraries used are deployed. The list of deployed languages can be restricted by supplying a comma separated list of language codes (codes matching filenames in `/usr/share/qt{5,6}/translations/`) with `--qt-languages` or `$TRANSLATION_LANGUAGES`.

Note that `--qt-languages` and `$TRANSLATION_LANGUAGES` only affect Qt's own translations. Program provided translations are not affected.

#### App Translations
App translation handling is program specific. For best results, make sure to configure the build system of the program to be deployed [as described in AppImage documentation](https://docs.appimage.org/packaging-guide/from-source/native-binaries.html#using-the-build-system-to-build-the-basic-appdir) when deploying from source.

It is best to test translations before distributing the AppImage. This can be done by

1. Making sure the locale to be tested is loaded on glibc Linux

   Here are some resources on the topic: [Arch Linux (Arch Wiki)](https://wiki.archlinux.org/title/Locale), [Debian](https://wiki.debian.org/Locale), [Alpine](https://wiki.alpinelinux.org/wiki/Locale), [Void Linux](https://docs.voidlinux.org/config/locales.html), [Gentoo](https://wiki.gentoo.org/wiki/Localization/Guide).
2. Override the `LC_MESSAGES` or `LANG` variable while executing the appimage from a terminal by either prepending `<VAR>=<LANG> ./myappimage.AppImage`:

   ```
   LC_MESSAGES=cs_CZ.UTF-8 ./myappimage-x86_64.AppImage
   ```

   or by issuing `export` before running the AppImage:

   ```
   export LC_MESSAGES=cs_CZ.UTF-8
   ./myappimage-x86_64.AppImage
   ```

Make sure that the tested program does indeed provide translations for the overridden locale.

linuxdeploy-plugin-qt provides a flag to add a symlink to program translations to `TranslationsPath` (to `<AppDir>/usr/translations`):

| Category                        | CLI enable                   | CLI disable                     | Env variable                      |
| ------------------------------- | ---------------------------- | ------------------------------- | --------------------------------- |
| Symlink app translations        | `--app-symlink-translations` | `--no-app-symlink-translations` | `TRANSLATIONS_SYMLINK_APP=YES/NO` |

#### Recommendations
linuxdeploy-plugin-qt enables individual library translations and symlink app translations and disabled merged library translations by default for backwards compatibility.

If the program was written with for example with `windeployqt.exe` in mind, merged library translations and symlink app translations should do the job.

You can try enabling and disabling these flags to see which are required for the program being packaged to load translations.

#### Recommendations to application developers
Load Qt translations with

```cpp
translator.load("qt_" + language, QLibraryInfo::path(QLibraryInfo::TranslationsPath));
```

or

```cpp
translator.load(QLocale::system(), "qt", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath));
```

This should work with linux distro packages, `windeployqt` deployed `.exe` files and with linuxdeploy-plugin-qt.

For program translations, the easiest way of distributing translations in regard to deploying it (with linuxdeploy-plugin-qt or other tools) is to bundle them into the executable as a [Qt resource](https://doc.qt.io/qt-6/resources.html). The rest of this section concerns the more complicated solution, which is installing compiled translations alongside the executable.

For program translations, you have the freedom of choosing translation directory, but be aware that the [recommended building process](https://docs.appimage.org/packaging-guide/from-source/native-binaries.html#using-the-build-system-to-build-the-basic-appdir) uses prefix of `/usr` and `DESTDIR` to install program files into AppDir.

If you try to load translations from the directory your build system thinks it installs them into at configure time, it will try to load translations from host, not from the appimage.

One solution is to load directories relative to `QLibraryInfo::path(QLibraryInfo::PrefixPath)` instead of `/usr` or build system prefix. See [standard paths](#standard-paths) for a list of standard paths recognized by Qt.

Another solution is to load from path relative to `QCoreApplication::applicationDirPath()`.

It is wise to try several directories for loading app translations. Some reasonable picks include:

```cpp
// Good for windeployqt and macdeployqt
QLibraryInfo::path(QLibraryInfo::TranslationsPath)
// windeployqt-esque
QCoreApplication::applicationDirPath() + "/translations"
// Not Windows friendly, good in combination with some other dirs
QLibraryInfo::path(QLibraryInfo::PrefixPath) + "/share/" + QCoreApplication::applicationName() + "/translations"
```

### Environment variables

Just like all linuxdeploy plugins, the Qt plugin's behavior can be configured some environment variables.

**General:**
- `$DEBUG=1`: enables verbose output, useful for debugging (equal to linuxdeploy's `-v0`)
- `$LD_LIBRARY_PATH=pathA:pathB`: Paths to check for library dependencies (see `man ld.so` for more information)

**Qt specific:**
- `$QMAKE=/path/to/my/qmake`: use another `qmake` binary to detect paths of plugins and other resources (usually doesn't need to be set manually, most Qt environments ship scripts changing `$PATH`)
- `$EXTRA_QT_MODULES=moduleA;moduleB`: Modules to deploy even if not found automatically by linuxdeploy-plugin-qt
  - Example: `EXTRA_QT_MODULES=svg;` if you want to use the module [QtSvg](https://doc.qt.io/qt-5/qtsvg-index.html)
  - To support Wayland, add `waylandcompositor` to this variable
- `$EXTRA_PLATFORM_PLUGINS=platformA;platformB`: Platforms to deploy in addition to `libqxcb.so`. Platform must be available from `QT_INSTALL_PLUGINS/platforms`.
  - To support Wayland, add `libqwayland-egl.so;libqwayland-generic.so`

**Translations:**
- `$TRANSLATIONS_INDIVIDUAL=YES/NO`
- `$TRANSLATIONS_MERGED=YES/NO`
- `$TRANSLATIONS_SYMLINK_APP=YES/NO`
- `$TRANSLATION_LANGUAGES=comma separated language list`

See [translations](#translations) for an explanation of the env variables.

QML related:
- `$QML_SOURCES_PATHS`: directory containing the application's QML files — useful/needed if QML files are "baked" into the binaries. linuxdeploy-plugin-qt will look for all imported QML modules and include them. `$QT_INSTALL_QML` is prepended to this list internally.
- `$QML_MODULES_PATHS`: extra directories containing imported QML files (normally doesn't need to be specified).

## Developer details
### Standard paths
Here are standard Qt lookup paths of appimage contents (same in Qt5 and Qt6):

| Path type                | Path                                       |
| -----------------------: | ------------------------------------------ |
| `PrefixPath`             | `/tmp/.mount_<random_id>/usr`              |
| `DocumentationPath`      | `/tmp/.mount_<random_id>/usr/doc`          |
| `HeadersPath`            | `/tmp/.mount_<random_id>/usr/include`      |
| `LibraryExecutablesPath` | `/tmp/.mount_<random_id>/usr/libexec`      |
| `BinariesPath`           | `/tmp/.mount_<random_id>/usr/bin`          |
| `PluginsPath`            | `/tmp/.mount_<random_id>/usr/plugins`      |
| `QmlImportsPath`         | `/tmp/.mount_<random_id>/usr/qml`          |
| `ArchDataPath`           | `/tmp/.mount_<random_id>/usr`              |
| `DataPath`               | `/tmp/.mount_<random_id>/usr`              |
| `TranslationsPath`       | `/tmp/.mount_<random_id>/usr/translations` |
| `ExamplesPath`           | `/tmp/.mount_<random_id>/usr/examples`     |
| `TestsPath`              | `/tmp/.mount_<random_id>/usr/tests`        |
| `SettingsPath`           | `/tmp/.mount_<random_id>/usr`              |
