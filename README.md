# `list.desktop`

Just a toy application to learn the ropes in the GTK
toolset.

# Design

Uses a GtkBox to separate the entry portion and the list
portion. The list is a GtkScrolledWindow wrapping a
GtkViewport which contains a GtkListBox. Items are
GtkListBoxRow with a horizontal box containing a label and
a GtkCheckButton.

# Building

``` shell
git clone https://github.com/AgentTroll/list.desktop
cd list.desktop
mkdir build && cd build
cmake ..
make
```

Alternatively you may want to head over to the releases
page to download the binary yourself

# Usage

Requires GTK3 to be installed on your system.

```
./list_desktop
```

It's a list application that sits on your desktop. Type
stuff in, click the `+` button to add. Use the checkbox to
mark stuff done and use the middle mouse button to remove
entries from the list. Pretty simple stuff.

# Credits

Built with [CLion](https://www.jetbrains.com/clion/).

Uses [GTK3](https://www.gtk.org/) and [cJSON](https://github.com/DaveGamble/cJSON].