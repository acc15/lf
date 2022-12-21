# Command

Command file is needed to persist "add" commands to avoid race conditions while some synchronizations may be in progress. 

Commands usually processed when mirror sync process is started and finished.

After paths are added to index, command file may be safely truncated/deleted.

If specified path isn't in path of any configured mirror, then such paths will be silently ignored.

Command file - its just a queue of `type:paths`, which sync process should index.
 
## Grammar

```plantuml
@startebnf
title leafsync command grammar

file = [line, {eol, line}];

(* Blank lines and comments are ignored *)
line = ws, [action, entry_type, ":", absolute_path, ws];

(* Add or remove to/from index *)
action = "+" | "-";

(* File, directory or recursive directory *)
entry_type = "f" | "d" | "r";

(* Zero or more whitespaces *)
ws = {"\t" | " "};

(* End of line - depens on OS *) 
eol = "\n" | "\r" | "\r\n"; 

@endebnf
```

## Format

Format is pretty simple: 

one line - one path

## Examples

    +f:/home/user/sync/file1.txt
    +d:/home/user/sync/dir1/dir2
    +r:/home/user/sync/dir1/recursive_dir
    -f:/home/user/sync/dir2/file2.txt

