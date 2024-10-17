###
 # @Autor: Witten Yeh
 # @Date: 2024-10-17 11:39:19
 # @LastEditors: Witten Yeh
 # @LastEditTime: 2024-10-17 13:13:06
 # @Description: 
### 

if [ $# -lt 1 ]; then
    echo "Usage: $0 <source_file.c> [--size <size>] [--level <level>] <input_image> <output_image>"
    exit 1
fi

SOURCE_FILE=$1

gcc -O3 -fopenmp $SOURCE_FILE ../deps/lodepng.c -o denoise

./denoise --size 3 --level 3 ../data/image-noise.png ../data/image-denoise.png