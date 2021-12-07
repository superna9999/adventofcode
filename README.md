Advent Of Code
==============

Usage:

```
$ meson build -Dbuildtype=release
$ ninja -C build
$ build/subprojects/YEAR/subprojects/dayX/dayX subprojects/YEAR/subprojects/dayX/input.txt
```

or

```
$ cd subprojects/YEAR/subprojects/dayX/
$ meson build -Dbuildtype=release
$ ninja -C build
$ build/dayX input.txt
```

Run all tests:
```
$ for year in `cd subprojects; ls` ; do
	for day in `cd subprojects/$year/subprojects/ ; ls` ; do
		echo "===== $year/$day =====" ;
		for file in `cd subprojects/$year/subprojects/$day/ ; ls *.txt` ; do
			echo "---- $file ----" ;
			build/subprojects/2021/subprojects/$day/$day subprojects/2021/subprojects/$day/$file ;
		done ;
	done ;
done
```
