# Used mirrors table

Table contains list of paths to which current directory was synchronized.
Names are null-terminated string
Empty names are disallowed - so empty names - designates end of mirrors table

## Example

	/mnt/sync\0
	/home/other\0
	\0 - mirrors table ended

# File tree with sync states

## Entry

	(name) | \0 | flags | (mirror sync flags - size depends on used mirrors) | (children entries) | \0

### Flags

Flags contains next group bits:

     87654   32   1
	[00000] [00] [0] 

* 1 bit - directory flag (0 - file, 1 - dir)
* 2-3 bits - enables sync of entry (0 - not syncing, 1 - shallow dir/file sync, 2 - deep/recursive dir sync)
* other bits - unused

### Mirror sync flags

Byte count is determined by used mirrors table and must be always equal to 

	(count of mirrors + 7) / 8

Each bit designates whether current entry was synchronized to mirror or not.
This information is required to determine how to resolve case when 
left contains file "a", but remote doesn't - as there is 2 solutions possible:

* add file "a" to right
* delete file "a" in left

### Example

	\b00000001\b00000000 <-- root flags and state
	 a\0\b00000001\b00000000 <-- Directory "a"
	  b.json\0\b00000010\b00000001\0 <-- Syncable file "b.json" - which already synced to first used mirror (`/mnt/sync`)
	 \0 <-- Designated "a" listing
	\0 <-- designates end of root listing, all trailing \0 also can be replaced with just <EOF>
