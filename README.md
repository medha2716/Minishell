# Description

# Assumptions

commands are of length <= 4096 (5000) bytes
path <= PATH_MAX (1024 bytes)
if & and ; used together a space given bt=etween them for eg: sleep 5 & ; sleep 3 (not sleep 5 &; sleep 3)
for seek -e -l and there is only 1 file, only 1e6 bytes of text file displayed on terminal
for foreground processes, time of entire process including differents commands separated by ; is shown in the next prompt (if time taken > 2s) 


seek -e missing permissions wala error

if invalid position given pastevents execute not stored in pastvents
