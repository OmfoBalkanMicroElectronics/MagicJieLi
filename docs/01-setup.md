# Setup

## What you need

- This repository. It is the complete MagicJL SDK tree, with no separate stock SDK, patches or overlays needed.
- **Code::Blocks 16.01** (`codeblocks-16.01mingw-setup.exe`)
- A **JieLi PI32V2 LTO toolchain** (`jl_toolchain_pi32v2_lto`) compatible with the AC692x
- Windows (the vendor toolchain and IDE integration are Windows-only)

## Install the IDE and toolchain

1. Install Code::Blocks, then **open and close it once**. The toolchain installer registers itself in the config file Code::Blocks creates on first launch.
2. Install the PI32V2 LTO toolchain. JieLi's 2018 documentation uses version `2.1.7` as its example. Use the version that came with your SDK or that JieLi supplied.
3. Some toolchain distributions are licence-locked to the machine. If yours is, generate a `*.key` with the JL toolchain utility and import the `*.lic` you receive. MagicJL has also been built with a toolchain that needs no licence. If you see no licence warning, skip this step.

## Open the project

Clone the repository, or download it as a ZIP, to a short path without spaces, for example:

```text
C:\Jieli\MagicJieLi\
```

Then open:

```text
BT_201_AC692x_SDK_release_V2.6.3\apps\sdk.cbp
```

If Code::Blocks shows no compiler for the project, right-click the project and choose **Build options → Compiler settings**, then select the installed JieLi PI32V2 compiler profile.

Next: [Building & flashing](03-build-and-flash.md) · Details: [SDK map](07-sdk-map.md)
