## SmallFry ##

SmallFry is a small proof-of-concept JPEG "optimizer" with a permissive license. What it does is try and find the most you can compress a JPEG without suffering obvious perceived quality loss. It works similarly to how [JPEGmini](http://www.jpegmini.com) works, with some borrowed metrics from their SPIE papers. If you want a nice GUI and easier batch automation, take a look at JPEGmini.

**Background**

I wrote this hack-of-a-program in early 2013, and largely ignored it since. Now that [mozjpeg](https://github.com/mozilla/mozjpeg) has gain some small mindshared, I thought it might be useful to others, since now it can take advantage of trellis quantization and scan optimization provided by it.

**Usability**

A small CLI util is provided, along with a (crappy) API and library. The code is just C89, and can be compiled with pretty much any compiler. Binaries are provided for Windows in the releases section.

The code is very sub-optimal and slow. Pull requests are welcome. I am lazy.

Currently only JPEG input is supported, but it is absolutely trivial to add other input via [ImageMagick](http://www.imagemagick.org) or [libavcodec](http://ffmpeg.org) (maybe via [FFMS2](https://github.com/FFMS/ffms2)). The current code is just what I had hacked together previously; it is not optimal.

**Disclaimer**

This code is entirely mine, and unrelated to my employer in any way, and is not used by my employer.

Users of this code may want to look into any patents they may require to deploy it. I am not a lawyer.
