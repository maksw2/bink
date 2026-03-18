# bink video 2 running under uefi

pretty cool right?

<table>
  <tr>
    <td align="center"><img src="qemu.png" width="200"></td>
    <td align="center"><img src="lenovo.png" width="200"></td>
  </tr>
  <tr>
    <td align="center"><img src="msi.png" width="200"></td>
    <td align="center"><img src="hyperv.png" width="200"></td>
  </tr>
  <tr>
    <td colspan="2" align="center"><img src="fish.gif" width="200"></td>
  </tr>
</table>

## what is it?

a ~~microkernel in denial~~ simple bink player.. running under uefi!

## how?

from the outside it looks like wine but less running under uefi instead of linux.  
i respect the firmware, but i also make it dance.

i shim a subset of win32 calls up to nt 6 or so and load the proprietary blob (dll) so it feels at home.

## legal?

nobody noticed this yet.  
either a job offer or a C&D 🙏 (rad don't kill me)

## how 2 build

open x64 vs dev prompt
- `build.bat`
- `run.bat`
- enjoy

the video to play is fatroot/video.bk2

## how hard was it?

> Bink file handlers are slightly more complicated to write on generic x86, because Bink doesn't handle the threading for you. Drop us an email, and we can send you the default file IO system, and so that you can modify it for your platform.

except i'm using the windows dll which expects threading to just be there. by some miracle it just works.

> We can send you many different source examples for various sound systems (waveOut, SDL, CoreAudio, etc), so that you can get up and running quickly. E-mail us for details.

except i'm a student with no access to the sdk.

> 1. Include bink.h in your source files. You may want to add an directory include to your compiler line in your make file.
> 2. Add the #define __RADX32__ to your compiler command line.
> 3. Add the BinkX32Imp.c and RADDLL.c files to your project. These are the files that load the BinkX32.dll binary module.

except i only had the windows dll (bink2w64.dll) and loads of time.

## q&a

Q: Why does it exist?  
A: Science isn't about 'why', it's about 'why not'.

Q: Why not raw frames?  
A: Too simple.

Q: Why not linux?
A: Where's the fun in that?

Q: Why don't you name this project properly?
A: Booze - When Wine isn't strong enough.

Q: How did you implement file I/O?
A: I didn't. I just preload the entire video into memory.

Q: Is this finished?  
A: Mostly.

Q: Can I use this in production?  
A: Define production.

Q: Will you add audio?  
A: I tried. I failed.
