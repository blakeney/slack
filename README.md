slack
=====

Simplified fork of the BSD hack game

# Differences from Standard hack
* No hunger
* No cursed items
* All items automatically identified
* Items removed related to hunger, curses, and identification
* No automatic item pickup (automatically look instead)

# Build Instructions
On FreeBSD with clang, execute the following in the 'src' directory:

```
env NO_WERROR=1 make
```

# Installation Instructions
No package is currently published.  To set up a system manually for play (needs to be done only once), use the following commands:
```
sudo mkdir /var/games/hackdir
sudo chmod 1777 /var/games/hackdir
sudo touch /var/games/hackdir/record /var/games/hackdir/perm
sudo chmod 666 /var/games/hackdir/record /var/games/hackdir/perm
```

Then run the `hack` binary that was built via `make`.

