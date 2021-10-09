<!-- PROJECT SHIELDS -->
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/gmagno/igl">
    <img src="./igl.svg" alt="Logo" width="256" height="256">
  </a>

  <h3 align="center">IGL - Image GL</h3>

  <p align="center">
    An OpenGL based image viewer!
    <br />
    <a href="https://github.com/gmagno/igl"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/gmagno/igl">View Demo</a> :construction:
    ·
    <a href="https://github.com/gmagno/igl/issues">Report Bug</a>
    ·
    <a href="https://github.com/gmagno/igl/issues">Request Feature</a>
  </p>
</p>



<!-- TABLE OF CONTENTS -->
### Table of Contents

* [About the Project](#framed_picture-igl---image-gl)
* [Dependencies](#hammer_and_pick-dependencies)
* [Getting Started](#baby_bottle-getting-started)
* [Installation](#package-installation)
* [Usage](#feet-usage)
* [Shortcut Keys](#keyboard-shortcut-keys)
* [License](#copyright-license)
* [Contact](#envelope-contact)



<!-- ABOUT THE PROJECT -->
### :framed_picture: IGL - Image GL 

[![igl][igl-demo]](https://github.com/gmagno/igl)

`igl` is a [bitmap](https://en.wikipedia.org/wiki/Raster_graphics) image viewer aiming at being:
 - :rocket: fast!
 - :mage_man: smart/ahead image loading
 - :construction_worker_woman: simple and intuitive functionality
 - :keyboard: keyboard friendly
 - :dark_sunglasses: awesome

And it's pronounced *eagle*.

:construction: Note: this is still work in progress, so please expect some uncaught exceptions and crashes.

### :hammer_and_pick: Dependencies

`igl` is built on top of amazing software:
* [Dear ImGui](https://github.com/ocornut/imgui)
* [GLFW](https://github.com/glfw/glfw)
* [stb_image](https://github.com/nothings/stb)
* [spdlog](https://github.com/gabime/spdlog)
* [CLI11](https://github.com/CLIUtils/CLI11)
* [fmt](https://github.com/fmtlib/fmt)
* [glad](https://github.com/Dav1dde/glad) 

In order to build the app also make sure you have:
* :toolbox: `cmake 3.10`
* :snake: `python` (optional: to install `glad` and fetch the latest loader source files)

<!-- GETTING STARTED -->
### :baby_bottle: Getting Started

For my fellow linux users, you may go straight to the [releases page](https://github.com/gmagno/img/releases), fetch the latest binary (AppImage) and conveniently add it to your system without having to go through the building process:

```bash
$ wget https://github.com/gmagno/igl/releases/latest/download/igl-???????-x86_64.AppImage
$ chmod +x igl-*-x86_64.AppImage
$ sudo ln -s /path/to/igl-*-x86_64.AppImage /usr/local/bin/igl  # optional
```

### :package: Installation

Apart from `glad` dependencies are managed as git submodules, don't forget to clone the repo *recursively* !

1. Get the source:
```bash
$ git clone --recursive https://github.com/gmagno/igl && cd igl
```

You may, optionally, download the latest OpenGL Loader files with `glad`:

```bash
$ python3 -m venv .venv        # creates a python virtual environment
$ source .venv/bin/activate    # activate the virtual environment
$ python3 -m pip install glad  # installs glad with which we fetch the source
$ glad --generator c --out-path ext/glad  # overrides whatever is in ext/glad
```

2. Build
```bash
$ mkdir build && cd build
$ cmake ..
$ make -j $(nproc)
```

3. Install

```bash
$ sudo cp igl /usr/local/bin  # or wherever pleases you
```

4. Enjoy! :partying_face:


<!-- USAGE EXAMPLES -->
### :feet: Usage

The app accepts a single positional argument, a path to either an image file or a directory. Easy peasy lemon squeezy :lemon:

```
$ igl path/to/awesome-image.png  # or igl path/to/dir/
```

The `--help` flag won't provide more info than that, but here it goes for completness anyway :nerd_face:

```
$ igl -h
IGL - Image OpenGL
Usage: igl [OPTIONS] [path]

Positionals:
  path TEXT                   Path to image or directory

Options:
  -h,--help                   Request help
```

Once the app is launched you may press the `h` key on your keyboard to see the available shortcut keys:

[![igl][igl-shot]](https://github.com/gmagno/igl)

### :keyboard: Shortcut Keys

|                                            | Function                             | Key                                             |
| ------------------------------------------ | ------------------------------------ | -------------------------------------------     |
| :woman_teacher:                            | Show the help window                 | `h`                                             |
| :arrow_up_down::left_right_arrow:          | Image panning                        | `drag` while pressing the `middle mouse button` |
|                                            |                                      | `w`, `a`, `s`, `d`                              |
|                                            |                                      | [`Shift` +] `mouse wheel`                       |
| :mag:                                      | Image zoom in/out                    | `Ctrl` + `mouse wheel`                          |
|                                            |                                      | `Ctrl` + `=`/`-`                                |
| :dart:                                     | Image recenter                       | `c`                                             |
| :previous_track_button::next_track_button: | Image select next/prev               | `Alt` + `mouse wheel`                           |
|                                            |                                      | `j`, `k`                                        |
| :open_file_folder:                         | Show files list window               | `e`                                             |
| :broom:                                    | Reset windows layout                 | `r`                                             |
| :sunny::new_moon:                          | Toggle theme {dark, light, classic}  | `t`                                             |
| :door:                                     | Quit                                 | `q`                                             |


### :black_nib: Font

If you're wondering about the font used in the app, it's called [Hermit](https://github.com/pcaro90/hermit) and it is gorgeous!
Kudos to [@pcaro90](https://github.com/pcaro90) for the lovely work.


<!-- LICENSE -->
### :copyright: License

Distributed under the MIT License. See `LICENSE` for more information.


<!-- CONTACT -->
### :envelope: Contact

Gonçalo Magno - [@gmagno6](https://twitter.com/gmagno6) - goncalo.magno@gmail.com

Project Link: [https://github.com/gmagno/igl](https://github.com/gmagno/igl)



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[stars-shield]: https://img.shields.io/github/stars/gmagno/igl.svg?style=flat-square
[stars-url]: https://github.com/gmagno/igl/stargazers
[issues-shield]: https://img.shields.io/github/issues/gmagno/igl?style=flat-square
[issues-url]: https://github.com/gmagno/igl/issues
[license-shield]: https://img.shields.io/github/license/gmagno/igl.svg?style=flat-square
[license-url]: https://github.com/gmagno/igl/blob/master/LICENSE
[igl-demo]: ./igl-demo.gif
[igl-shot]: ./igl-shot.png
