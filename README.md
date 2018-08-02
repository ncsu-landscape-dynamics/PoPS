# PoPSS

PoPSS library

recoding the model to create a c++ version of the SOD-model base on https://github.com/f-tonini/SOD-modeling.
This repository contains the c++ version scripts used to develop a stochastic landscape spread model of forest pathogen *P. ramorum*.

The reference paper: Ross K. Meentemeyer, Nik J. Cunniffe, Alex R. Cook, Joao A. N. Filipe, Richard D. Hunter, David M. Rizzo, and Christopher A. Gilligan 2011. Epidemiological modeling of invasion in heterogeneous landscapes: spread of sudden oak death in California (1990–2030). *Ecosphere* 2:art17. [http://dx.doi.org/10.1890/ES10-00192.1] (http://www.esajournals.org/doi/abs/10.1890/ES10-00192.1) 

PoPSS is a header-only C++ library. It is using templates to be
universal and it makes use of C++11 features, so C++11 is the minimal
required version.

## Using the model

The PoPSS library can be used directly in a C++ program or through other
programs. It is used in an experimental version of a GRASS GIS module
called r.spread.sod.

* https://github.com/ncsu-landscape-dynamics/r.spread.pest

## Integrating the library into your own project

### As a Git submodule

This is a convenient way, if you are using Git and you can use the C++
header files directly.

Git supports inclusion of other repositories into your own code using
a mechanism called submodules. In your repository, run:

```
git submodule add https://github.com/ncsu-landscape-dynamics/PoPSS popss
```

The will create a directory called `popss` in your repository which will
now contain all the files from this repository. You can use the two
following commands to see the changes to your repository:

```
git status
git diff --cached
```

Git added a file called `.gitmodules` with the link to this repository
and linked a specific commit in this repository. The commit linked is
the currently latest commit to PoPSS library.

You can now commit and push changes to your repository.

When someone else clones our project, they need to run the two following
commands to get the content of the `popss` directory:

```
git submodule init
git submodule update
```

Alternatively, the `popss` directory can be populated during cloning
when `git clone` is used with the `--recurse-submodules` parameter.

If you want to update the specific PoPSS commit your repository is using
to the latest one, you the following command:

```
git submodule update --remote
```

## Testing the model in this repository

Here we are assuming that you use Linux command line or equivalent,
i.e., you have `make` (e.g. GNU make) and GNU GCC with `g++`
(or something compatible with the same command line interface).
If you don't have it, you may need to modify the `Makefile` or configure
your system.

First download the source code (as a ZIP file and unpack it or use Git
to get it from the Git repository).

Then compile the code:

    make

And finally run the tests:

    make test

## Authors

* Francesco Tonini (original R version)
* Zexi Chen (initial C++ version)
* Vaclav Petras (raster handling, critical temperature, library, ...)
* Anna Petrasova (single species simulation)

## License

Permission to use, copy, modify, and distribute this software and
its documentation under the terms of the GNU General Public License
is hereby granted. No representations are made about the suitability
of this software for any purpose. It is provided "as is" without express
or implied warranty. See the
[GNU General Public License](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
for more details.
