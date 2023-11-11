# Wallpaper Manager
## Description
Small project in C++ using wxWidgets to better manage collections of wallpapers on Windows.

## Installation
Tested for Windows 10 and 11.
Download the release .zip [here](https://github.com/YoshiLeLama/wp_manager/releases/tag/stable), extract it and run WP_Manager.exe in the extracted folder.

## Usage
You can add or remove an empty collection of wallpaper images using the buttons next to the category choice.

You can add images to the collection using File > Add Files (or CTRL+O).

You can set the wallpaper image by double clicking on an image.

You can select an image by clicking on it, or multiple images by holding CTRL and clicking on them.

You can delete selected images from the collection by using Select > Delete selected files (or Del).

## Technical notes
The added images are copied into a folder called 'data' where the application is running.

This folder contains one directory per collection containing the images, each one corresponding to an id (unique across all the collections), and an 'id' file containing the id of the current background image.
