# YtSubsTracker
An application for anonnymous 'subscribing' to youtube channel

## About
YtSubsTracker is a GTK3 application, probably going to work best on Gnome 3 on Linux. 
It's purpose is to do something similar to what youtube does when you log in and look at your subscriptions - 
display videos from channels you care about, in chronological order. However, this application manages your 
subscriptions list locally, and uses anonymous (in contrast to youtube APIs) xml feeds in order to get the latest videos.

The selected videos are played using VLC, but it would be nice to have the command editable in the future using some preferences.

## Compiling
http://mesonbuild.com/Running-Meson.html

## Contributing
**disclaimer:** I have not done this before

I have mostly used Gnome Builder IDE to make this. It's not necessary, but it's a great experience.  
I think I want this app to adhere to the good old Unix philosophy - do one tning and do it well.
It already does that thing quiet well, so the only additional feature I can think of is to show only videos of one channel, 
if you click on that channel.  
On the other hand, Unix philosophy promotes modularity. I tried to make the app quiet modular, and I hope new backends
(if there ever is something similar to youtube in this aspect) and new frontends (android, >NOT< Windows, better integration with other DEs, command line)
can be easily added with minimal architectural changes. Until GTK works on android or I manage to get Librem 5 without buying it,
I personally would definitely use the android port.

So if you want to contribute something bigger, feel free to open an issue or just send me an e-mail, and I'll try to behave professionally and responsibly.
