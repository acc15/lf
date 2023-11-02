![Linux build status](https://github.com/acc15/lf/actions/workflows/linux.yml/badge.svg "Linux build status")

# lf

`lf` - is tiny command-line utility which allow to partially synchronize two directories or files.

Its designed especially for partial tree sync. Only files/directories which was explicitly marked for synchronization are processed (not every file in directory)

Partial tree sync may be useful when you want, for example, synchronize whole user home direcory (or even whole "root" filesystem on linux/Mac OS) - but you dont need to sync **ALL** files - just files you explicitly enable for sync. 

## Comparison of existing software

There is already a lot of file sync software, but not all can make partial sync, some of them are really cool, but i find them inconvinent for command-line usage. 

The best tool which i found is `FreeFileSync` - it has a lot features (including partial sync)

### [Dropbox](https://www.dropbox.com/)

Dropbox can only sync whole directory recursively. 
It has selective sync feature however its about excluding right files/directories, but not left 

### [Yandex.Disk](https://disk.yandex.ru/)

Yandex.Disk has only `exclude-dir` options. Allows to recursively synchonize only single directory.

### [Unison](https://www.cis.upenn.edu/~bcpierce/unison/) 

Unison allows to specify two directories and also has Ignore options (but no include option). However its possible to create directory with symlinks to real files and `-follow` option and sync that directory. 

TBD

### [FreeFileSync](https://freefilesync.org/) 

Powerful FreeFileSync has [filters and exclusion](https://freefilesync.org/manual.php?topic=exclude-files) and allows partial sync with a lot of options - but its extremely inconvential to manage Filter Include/Exclude list from command-line and therefore requires additional scripting to add/remove filters from XML `.ffs_batch` configuration.

### [Syncthing](https://syncthing.net/)

Only whole directories, but supports Ignore list.

# Getting started

## Compilation

### Supported compiles and versions

* Linux: GCC 12 or CLang 14
* MacOS: CLang 14
* Windows: VS 2022 (_MSCVER >= 1930)

### Building

To build `release` binary just run 

on Linux/Mac OS

    release.sh

on Windows

    release.bat

## State file

State file contains current synchronization state.
Its a list of files which was synced to right directory. 
Required to correctly detect file addition/removal.

## Index file

List of files for synchronization is stored in binary file called index file. Index file contains file-tree with their assigned synchronization modes.

`lf` has several commands to manage index files:

- `lf add <paths>` - assigns synchronization mode to specified paths
- `lf rm <paths>` - completed removed file-tree from index

## Configuration file

Config file describes directories and location of index and state files.
Default config file location varies depending on platform.

* Linux - `~/.config/lf/lf.conf`
* Windows - `%LOCALAPPDATA%\lf\lf.conf`)
* Mac OS - `~/Library/Preferences/lf/lf.conf`

### Example

    # Path to left directory
    left=/home/user
    
    # Path to right directory
    right=/mnt/router/sync
    
    # Path to state file (relative to left directory), defaults to 'lf.state'
    # Also, any absolute path can be gived
    #state=lf.state

    # Path to index file (relative to right directory), defaults to 'lf.index'
    # Any absolute path can be gived, but it's required that any other host can access this file
    #index=lf.index

## Sync algorithm

Synchronization algorithm is very simple and straightforward - `lf`  just watches last modification time of files. It assumes that files are same when last modification time are equal (with 0.1 microsecond precision - default NTFS timestamp precision) 
