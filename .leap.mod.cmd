savedcmd_leap.mod := printf '%s\n'   leap.o | awk '!x[$$0]++ { print("./"$$0) }' > leap.mod
