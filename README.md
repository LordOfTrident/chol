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

A collection of header-only [STB-style](https://github.com/nothings/stb) C libraries.

## Table of contents
* [Documentation](#documentation)
* [Examples](#examples)
* [Quickstart](#quickstart)
  * [Git submodules](#git-submodules)
  * [YPM](#ypm)
* [Bugs](#bugs)

## Documentation
Each header file contains its documentation in the form of comments.

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

## Bugs
If you find any bugs, please create an issue and report them.
