# Post mortem: Fiesta 2 crashing on modern linux
Date of writing: 17th November 2019

This is a port mortem about an issue that was puzzling me for months. In the following sections, I am documenting the
whole story as detailed as possible hoping this is going to be useful for anyone else (or even myself) in the future.

## What happened?
I have been running a multidisk setup with all pump games on it for a few years now and have been maintaining and
improving pumptools in the process. I always kept things quite up to date because once you need some more tooling
even on something like a dedicated machine, updating the system after it has not been updated for months or years
becomes very difficult. Furthermore, things might stop working that have worked before because of one or even multiple
issues that got introduced with the update.

Anyway, my cabinet with the multidisk setup yields the following on `uname -a`:
`Linux piumd 4.15.11-1-ARCH #1 SMP PREEMPT Mon Mar 19 18:21:03 UTC 2018 x86_64 GNU/Linux`

My workstation is a bit more up-to-date with (though, I am behind a few months...):
`Linux ambient 5.2.3-arch1-1-ARCH #1 SMP PREEMPT Fri Jul 26 08:13:47 UTC 2019 x86_64 GNU/Linux`

In the past, like more than a year ago, my workstation was also running a newer kernel, version of runtime libraries
etc. but running Fiesta 2 and newer with pumptools wasn't an issue. However, Fiesta 2 and newer stopped working when
I was continuing with my development efforts and just crashed with a segmentation fault. All old versions prior
Fiesta 2 were still working perfectly fine.

## Error analysis: first information and debugging steps
First, I enabled f2hooks built in logging to yield more output `util.log.level=4` and enabled halting on crashes to
be able to attach a debugger `patch.sigsegv.halt_on_segv=1`. This is the output I got:
```
[I][f2hook][main.c:155]: Hooking finished
[M][f2hook][main.c:168]: Calling orig_init
[E][patch-sigsegv][sigsegv.c:18]: ===================
[E][patch-sigsegv][sigsegv.c:19]: !!!!! SIGSEGV !!!!!
[E][patch-sigsegv][sigsegv.c:28]: Backtrace (3 frames):
[E][patch-sigsegv][sigsegv.c:31]: ./f2hook.so(+0x6a02) [0xf7e20a02]
[E][patch-sigsegv][sigsegv.c:31]: ./f2hook.so(+0xf827) [0xf7e29827]
[E][patch-sigsegv][sigsegv.c:31]: linux-gate.so.1(__kernel_sigreturn+0) [0xf7f07940]
[E][patch-sigsegv][sigsegv.c:37]: Halting for debugger...
```

Ok, so it seems like something crashed in f2hook.so. However, hooking up the debugger did not yield anything useful
and this was just pointing to the sigsegv handler (thought it worked with debugging other issues and actually pointed
to the location where it crashed...strange).

Next, I started removing stuff from f2hook to get a minimal version which still boots up to that point, no changes.

Then, I hooked up gdb, set a breakpoint before the crash happened and stepped my way through the whole libc
runtime setup. I figured out, that the crash happens when the libc runtime is calling handler functions of the
target application which setup various parts of the runtime environment, like static initialization.

And this is the place where the crash happened. There is one function in the piu executable which setups up all static
variables, for example constructing static strings or other objects. Once it tried to construct the first static string
of that static setup function, it crashed. To be more precise, it crashed in the string constructor when allocating
memory for the string.

Great, we found the root cause for that. Let's figure out why that happens and what's different now that it crashes
there.

## Tracing the bug down to libstdc++ and being totally mislead by various things
Because there have been issues with incompatible libc/libcstd++ versions previously, I quickly came to the idea of
comparing the version that worked on my cabinet and the version that did not work on my workstation. Indeed, these
were different versions (considering the age gap of the two OSes, that wasn't very surprising).

I started digging into the libstdc++ source code, especially looking at what is going on when strings are allocated.
Navigating the libstdc++ source is not a nice task. The naming is very confusing and, imo, non intuitive. With
templating all over the place, this makes it even worse.

Finally, I found something that sounded very related to what I have discovered so far:
In the file include/bits/basic_string.h, search for
```
_GLIBCXX_RESOLVE_LIB_DEFECTS
3076. basic_string CTAD ambiguity
```

and you will end up at a string constructor call. From that, I concluded that something was fixed related to
constructor calls. I compared the string allocation call of both versions, the non working and working ones. The
allocation for strings has changed.

Going back to the location of the crash in the piu executable, I saw that the string allocation was even inlined and
not going to a library call of...boost filesystem. The first static object created is the path "../" which is used by
a path object by boost filesystem. Because boost filesystem is a header only library, I assume that's why this got
inlined.

However, I was using the libstdc++ version of my local machine where the string allocation scheme was different. This
still did not truly explain why it crashed there with a jump to address 0, but things aligned pretty well so far.

So, the first idea for a solution was to use the old libstdc++ version from the machine where everything was fine
and working. However, using it did not change anything. I went on and grabbed more libraries from the machine until
I had all of them on my workstation and hooked them up with a `LD_LIBRARY_PATH`. But now, the application did crash
very early and did not even get to setting up the runtime at all. I did some poking but could not figure out what
was happening there. It just crashed with another jump to 0 very early in launching the elf.

Debugging continued for a bunch of weeks, on and off. I got more focused on digging deeper into the string allocation
thing as this was giving me a direction and was aligning with my findings so far. I came up with a "solution" to hook
and overwrite the string constructor calls of the newer libstdc++ version.

I started replacing them one by one to re-implement the old allocation scheme. However, this was not enough as many
function calls that are working with string objects rely on the used allocation scheme, too. Therefore, I also started
patching these until I realized that I have to re-implement, at least, everything basic_string related.

At that point, I was quite sure that this is the wrong way to go and stopped with this approach.

A few more months passed, I got back at this multiple times but couldn't come up with another solution or even
direction.

## AppImage
In a talk with a buddy, I had the idea of using some sort of sandboxing or environment boxing to pack things up nicely
into a single blob to make it easier to handle, even distributable and maybe even more platform independent.

After some internet research, I found a solution that looked like it fits the pump game use-case very well which was
AppImage. AppImage packs all your stuff into a single binary blob which is basically just a sqash fs image with a
special runtime bootstrapping the whole image and running a bash script once the image is mounted. The provided features
are very minimalistic but solve all our needs for packing pump games nicely.

The reference documentation also pointed out a few things about packing runtime libraries and how to set them up using
`LD_LIBRARY_PATH` which I was already doing but just using a naked bash script. However, this reading was starting to
point me into the right direction.

## Providing a platform/distro independent environment for your application down to the linker
I started playing around with the libraries I used for Fiesta 2. First, I grabbed _all_ libraries from my cabinet which
are required to run the game (using `ldd`). Then I started trying different combinations of including them using
`LD_LIBRARY_PATH` and some used by my native system. Nope, either I was running into the static init crashing issue
or things crashed even earlier on.

Wait, have I ever looked into why the application crashed so early, even before spinning up the runtime environment?
After working on this on and off for months and totally losing track by getting to deep into this libstdc++ thing, I
realized I didn't.

I set up the application to use all libraries from my cabinet and started debugging. First, I tried my luck with `gdb`
but didn't get very far as it just showed me a jump to 0, again. Next, I got pointed out to using `valgrind`. This was
a great hint as valgrind hooks into the bootstrapping process of the application and even displays a lot of information
about library loading and setting up the environment even way before the runtime is set up. Recommendation: Use
`valgrind -v -v -v` to get a lot of output.

However, in the end, valgrind also just pointed out the jump to 0 being the reason for the crash but not why this
happened. The additional information was good to figure out the steps happening in the application setup process but
did not yield any additional insights.

After giving this some more thought and making sure I used all runtime libraries from the cabinet, I did some
internet research again and tried go get more information on how ELFs are bootstrapped and what happens. It turned out
that information on that topic very sparse and difficult to find. However, this brought me to the following thread
from the gnu mailing list:
```
Hi all;

I have a need to use a different runtime linker (ld-linux.so) for some
of my applications.  I _don't_ want to change the executables themselves
(changing the path to the runtime linker in the ELF image for example).

I know that I can just run my application as an argument to ld-linux.so,
and that actually does work very well.  But, I am left with two
problems:

  * Some of my applications fork/exec other programs, and those other
    programs also need to use the other runtime linker.  Just as I don't
    want to change the ELF image, I certainly don't want to have to
    recompile the programs to exec the runtime linker!

  * Debugging: I can't debug because I can't find a way to convince GDB
    to invoke the program-to-be-debugged using an alternative runtime
    linker.

I was wondering if anyone has any ideas about how to do this?  I checked
the man pages and didn't see anything (I was looking for something like
an environment variable that ld-linux.so might obey to re-exec a
different version, or some such thing--not likely, I know).

If there's nothing like that available another idea is to use LD_PRELOAD
to install a private version of the execve() system call, which would
set the alternative ld-linux.so as the first argument.  Any thoughts on
this approach?  Do you think it would solve the GDB problem as well?
I'm going to proceed to work on this idea but I wanted to see if anyone
had other thoughts about it.



PS. In case anyone's curious, I'm trying to run some programs natively
    on my desktop system which were actually compiled for a different
    system, and the native and target systems have different versions of
    glibc installed.  I can't change either of the versions of glibc,
    but I do have the complete root filesystem of the target box in a
    subdirectory of my native box and I can use LD_LIBRARY_PATH, etc. to
    use them... I just need a way to get the runtime environment to
    initialize properly.

--
-------------------------------------------------------------------------------
 Paul D. Smith <address@hidden>   HASMAT--HA Software Mthds & Tools
 "Please remain calm...I may be mad, but I am a professional." --Mad Scientist
-------------------------------------------------------------------------------
   These are my opinions---Nortel Networks takes no responsibility for them.
```

Source: https://lists.gnu.org/archive/html/help-gnu-utils/2004-06/msg00020.html

Oh, wow. I was amazed that someone else was having a very similar use-case of running applications from legacy systems
on a newer machine with different library versions and conflicts.

Reading on, this answer was finally revealing the last piece of the puzzle to understand the issue I was facing with
Fiesta 2:
```
If you have an old a.elf main executable program that was linked
to run with libc.so.6, and you want to run that a.elf on a machine
whose libc.so.6 differs from the one which a.elf was linked against,
and you want a reasonable guarantee that it will work, then it may
be necessary to use something like rtldi to choose the matching
ld-linux.so.2 and libc.so.6 and other libraries as of the time of
static binding of a.elf.  In theory, libc.so.6 is backwards compatibile
(a given a.elf should run the same under any subsequent libc.so.6),
but in practice there have been too many bugs in {ld-linux.so.2,
libc.so.6, ...}.  For example, many programs linked for libc.so.6
under RedHat Linux 7.2 and earlier won't run under glibc-2.3.2.
The most effective way to run such a program may be to install the
old glibc-x.y.z[.rpm] in a different location, binary edit the PT_INTERP
string of the a.elf, and use rtldi.  This makes the old [edited] a.elf
simultaneously interoperable with other programs from different
libc.so.6 generations, and only the "oddball" program has to know.

...

ld-linux.so.2, libc.so.6, libdl.so, libnss*.so, and _many_ other shared
libraries of a given released version, are a matched set.  They must be
used together, all from the same release, or not at all.  Mixing and
matching pieces from different glibc-x.y.z need not work.  In order to
use a consistent set that differs from the default set, you must invoke
the ld-linux.so.2 using "--library-path PATH" to specify the location
of the rest of the set.  Using --library-path supersedes LD_LIBRARY_PATH
for one execve() only, and does not interfere with chilren, execve()
with no fork(), or other processes.
```

Oh, I was not aware about ld-linux.so being THAT important to this set of libc.sp, libdl.so etc. Therefore, I grabbed
the ld-linux.so file from my cabinet and ran the application with all libraries (except the GPU driver specific ones)
on my workstation like this:
```
LD_PRELOAD="./f2hook.so" ./ld-linux.so.2 --library-path ./lib ./piu ./game/ --options ./hook.conf
```

And the application starts up, passes runtime initializing, hits main and the game works.

I don't think there is anything else to add to the copy-pasted explanations above. Make sure to read them carefully to
understand the whole issue. Once you know what is going on, the whole thing is not even super complex.

## Conclusions
I would consider this being one of the major issues I have resolved so far for pumptools as this was so misleading,
difficult to debug and find any general information on. The reason behind that and the solution are super simple, once
you know them.

As we have already discussed various times in the past, this issue further points out how important proper solutions
for sandboxing, containerizing or how you want to call it for legacy applications like pump games are. The naked
bash script setting up everything is already quite good and doing the job. AppImage further enhances this by making
the whole application more portable.

However, understanding the whole process behind ELF loading, library linking, setting up the process environment and
setting up the runtime environment is very important. Otherwise, rather simple incompatibilities are the result which
have a major impact on running the application.