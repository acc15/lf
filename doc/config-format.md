# Configuration file format

Configuration file format are plain text

Each line means separate synchronization process

## Grammar

```plantuml
@startebnf
title leafsync config grammar

file = [line, {eol, line}];

(* Blank lines and comments are ignored *)
line = ws, [(comment | sync), ws];
sync = sync_name, ":", index_path, "//", mirror, "//", mirror;

mirror = mode, ":", absolute_path;

(* Read/Write mirror mode *)
mode = "r" | "w";
comment = "#", {"<any char, except eol>"};

(* Zero or more whitespaces *)
ws = {"\t" | " "};

(* End of line - depends on OS *) 
eol = "\n" | "\r" | "\r\n"; 

@endebnf
```

## Format

    <name>:<path-to-index>//<left-mode>:<left-path>//<right-mode>:<right-path>

### Index

Index path are required to handle some cases during syncs.
Indexes stores current sync tree and synchronization state with each used mirror (whether it was already synchronized to it or not)

Index format is binary and described [here](index-format.md)

### Mode

Mode allow to specify whether target file is read-only or not. 
By using different modes its posible to specify different sync modes:

    r : w - Copies left tree to right (excluding updates of old files)
    w : r - Copies right tree to left (excluding updates of old files)
    w : w - Bidirectional synchronization (files/directories from left copied/deleted to/in right tree (and vice versa)
    r : r - invalid configuration - nothing to sync (both trees are read-only)

### Example

    home:/home/user/.config/leafsync/leafsync.index//w:/home/user//w:/mnt/sync
