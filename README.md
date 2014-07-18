slack
=====

Simplified fork of the BSD hack game

# Differences from Standard hack
* No hunger
* No cursed items
* All items automatically identified
* Items removed related to hunger, curses, and identification

# Build Instructions
On FreeBSD with clang, execute the following in the 'src' directory:

```
env NO_WERROR=1 make
```

