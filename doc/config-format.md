# Configuration file format

Configuration file format are plain text

## Example

    # Mirror (sync) name
    [home]
    
    # Path to local directory
    local=/home/user
    
    # Path to remote directory
    remote=/mnt/router/sync
    
    # Path to state file (relative to local directory), defaults to 'lf.state'
    # Also, any absolute path can be gived
    #state=lf.state

    # Path to index file (relative to remote directory), defaults to 'lf.index'
    # Any absolute path can be gived, but it's required that any other host can access this file
    #index=lf.index

    # Config file may contain as many syncs as required
    # [other]
    # local=/local
    # remote=/remote
