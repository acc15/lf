# lf

`lf` - is tiny program which allow to partially synchronize two directories or files.

Partial tree sync may be useful when you want, for example, synchronize user home direcory (or even whole "root" filesystem on linux/Mac OS) - but you dont need to sync **ALL** files - just files you explicitly enable for sync. 

## Comparison of existing software

The key difference between existing software (with bidirectional sync support) such as:

### [Dropbox](https://www.dropbox.com/)

Dropbox can only sync whole directory recursively. 
It has selective sync feature however its about excluding remote files/directories, but not local 

### [Yandex.Disk](https://disk.yandex.ru/)

Yandex.Disk has only `exclude-dir` options. Allows to recursively synchonize only single directory.

### [Unison](https://www.cis.upenn.edu/~bcpierce/unison/) 

Unison allows to specify two directories and also has Ignore options (but no include option). However its possible to create directory with symlinks to real files and `-follow` option and sync that directory. 

TBD

### [FreeFileSync](https://freefilesync.org/) 

Powerful FreeFileSync has [filters and exclusion](https://freefilesync.org/manual.php?topic=exclude-files) and allows partial sync with a lot of options - but its extremely inconvential to manage Filter Include/Exclude list from command-line and therefore requires additional scripts to add/remove filters in XML `.ffs_batch` configuration.

# Getting started

## Compilation

To build `release` binary just run 

    release.sh

on Linux/Mac OS, or

    release.bat

on Windows

## Configuration file

Config file describes directories and location of index and state files.

## State file

State file contains current synchronization state.
Its a list of files which was synced to remote directory. 
Required to correctly detect file add/remove.

## Index file

List of files for synchronization is stored in binary file called index file. Index file contains file-tree with their assigned synchronization modes.

`lf` has several commands to manage index files:

- `lf add <paths>` - assigns synchronization mode to specified paths
- `lf rm <paths>` - completed removed file-tree from index

## Sync algorithm

Synchronization algorithm is very simple and straightforward - `lf`  just watches last modification time of files. It assumes that files are same when last modification time are equal (with 0.1 microsecond precision - default NTFS timestamp precision) 