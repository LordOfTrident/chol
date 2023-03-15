<p align="center">
	<img width="300px" src="./res/logo.png">
</p>
<p align="center">
	<a href="./LICENSE">
		<img alt="License" src="https://img.shields.io/badge/license-MIT-26c374?style=for-the-badge">
	</a>
	<a href="https://github.com/LordOfTrident/chol/issues">
		<img alt="Issues" src="https://img.shields.io/github/issues/LordOfTrident/chol?style=for-the-badge&color=4f79e4">
	</a>
	<a href="https://github.com/LordOfTrident/chol/pulls">
		<img alt="GitHub pull requests" src="https://img.shields.io/github/issues-pr/LordOfTrident/chol?style=for-the-badge&color=4f79e4">
	</a>
	<br><br><br>
</p>

A collection of header-only [STB-style](https://github.com/nothings/stb) C99 libraries.

**NOTE:** This library is still actively developed and might recieve breaking changes.

## Table of contents
* [Documentation](#documentation)
* [Examples](#examples)
* [Quickstart](#quickstart)
  * [Git submodules](#git-submodules)
  * [YPM](#ypm)
* [Platform support](#platform-support)
* [Bugs](#bugs)

## Documentation
Each header file contains its documentation in the form of comments.

If you need to include `windows.h` or `unistd.h` in your project while youre using a library from
here, include [`sys.h`](./sys.h) instead, which defines version macros to make certain
functions used by the libraries here available. If those macros arent defined and those headers
are included before the libraries, the compiler might throw errors.

## Examples
Examples for each library can be found in the [examples](./examples) folder.
To build them, first bootstrap the builder with
```sh
$ cc build.c -o build
```

and then build
```sh
$ ./build
```

The binaries will be outputted into [bin](./bin).

## Quickstart
Either copy this repository into your project, or you can use one of the following

### Git submodules
```sh
$ git submodule add https://github.com/LordOfTrident/chol
```

### YPM
```sh
$ ypm add https://github.com/LordOfTrident/chol
$ ypm update
```

You can find ypm [here](https://github.com/yeti0904/ypm)

## Platform support
Table of what platforms the given library was tested on and worked

| Library                   | Linux support | Windows support | Unix support | MacOS support |
|---------------------------|---------------|-----------------|--------------|---------------|
| [builder.h](./builder.h)  | Yes           | Yes             | Not tested   | Not tested    |
| [fs.h](./fs.h)            | Yes           | Yes             | Not tested   | Not tested    |
| [log.h](./log.h)          | Yes           | Yes             | Not tested   | Not tested    |
| [sv.h](./sv.h)            | Yes           | Yes             | Yes          | Yes           |
| [args.h](./args.h)        | Yes           | Yes             | Yes          | Yes           |
| [common.h](./common.h)    | Yes           | Yes             | Yes          | Yes           |
| [colorer.h](./colorer.h)  | Yes           | Yes             | Not tested   | Not tested    |
| [darray.h](./darray.h)    | Yes           | Yes             | Yes          | Yes           |

## Bugs
If you find any bugs, please create an issue and report them.
