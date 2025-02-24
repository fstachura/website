#!/bin/sh

userresources=$HOME/.Xresources
usermodmap=$HOME/.Xmodmap
sysresources=/etc/X11/xinit/.Xresources
sysmodmap=/etc/X11/xinit/.Xmodmap

# merge in defaults and keymaps

if [ -f $sysresources ]; then







    xrdb -merge $sysresources

fi

if [ -f $sysmodmap ]; then
    xmodmap $sysmodmap
fi

if [ -f "$userresources" ]; then







    xrdb -merge "$userresources"

fi

if [ -f "$usermodmap" ]; then
    xmodmap "$usermodmap"
fi

# start some nice programs

if [ -d /etc/X11/xinit/xinitrc.d ] ; then
 for f in /etc/X11/xinit/xinitrc.d/?*.sh ; do
  [ -x "$f" ] && . "$f"
 done
 unset f
fi

width=`cat /sys/class/graphics/fb0/virtual_size | cut -d, -f1`
height=`cat /sys/class/graphics/fb0/virtual_size | cut -d, -f2`

feh --bg-center /usr/local/share/wallpaper.jpeg &
twm &
xterm -geometry $(((width-8)/6))x$(((height-30)/13))+0+0 -ti 340 -e 'sh -l' &

sleep infinity
