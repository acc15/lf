# Configuration file format

Configuration file format are plain text

## Example

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

