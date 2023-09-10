#!/bin/bash

# Save the first argument as the img to be used
img=$1
# Save the snd argument in a variable called "mode"
if [ -n "$2" ]; then
    if [ "$2" == "new" ]; then
        echo "Deleting modified image and creating a new one"
        rm -f ../modified_imgs/$img.img
        if [ -n "$3" ]; then
            params=-$3
        fi
    else
        params=-$2
    fi
fi

# create directory for modifed images if it doesn't exist
mkdir -p ../modified_imgs

modified_img_exist=$(ls ../modified_imgs | grep -c $img.img)

if [ $modified_img_exist -eq 0 ] ; then
    echo "No modified image ${img}.img found. Creating a new one..."
    cp ../original_imgs/$img.img ../modified_imgs/$img.img
fi

mkdir mnt 2> /dev/null
# Mount the image
make
./fat-fuse $params ../modified_imgs/$img.img mnt
if [ -z "$params" ]; then
    sleep 1
    clear
    echo "${img}.img mounted. Turn into mnt/ to start editing."
    sleep 2
    clear
else
    sleep 1
    clear
    echo "${img}.img unmounted. Cleaning up..."
    sleep 2
    make clean
fi