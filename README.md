Advent Of Code
==============

Usage:

```
$ cd YEAR/
$ meson build -Dbuildtype=release
$ ninja -C build
$ build/subprojects/dayX/dayX subprojects/dayX/input.txt
```

or

```
$ cd YEAR/subprojects/dayX/
$ meson build -Dbuildtype=release
$ ninja -C build
$ build/dayX input.txt
```
