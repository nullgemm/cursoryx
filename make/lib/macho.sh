#!/bin/bash

# get into the script's folder
cd "$(dirname "$0")" || exit
cd ../..

# params
build=$1
toolchain=$2

echo "syntax reminder: $0 <build type> <target toolchain type>"
echo "build types: development, release, sanitized"
echo "target toolchain types: osxcross, native"

# utilitary variables
tag=$(git tag --sort v:refname | tail -n 1)
output="make/output"

# ninja file variables
folder_ninja="build"
folder_objects="\$builddir/obj"
folder_cursoryx="cursoryx_bin_$tag"
folder_library="\$folder_cursoryx/lib/cursoryx"
folder_include="\$folder_cursoryx/include"
name="cursoryx_macho"

# compiler flags
flags+=("-std=c99" "-pedantic")
flags+=("-Wall" "-Wextra" "-Werror=vla" "-Werror")
flags+=("-Wformat")
flags+=("-Wformat-security")
flags+=("-Wno-address-of-packed-member")
flags+=("-Wno-unused-parameter")
flags+=("-Isrc")
flags+=("-Isrc/include")
flags+=("-fPIC")
ldflags+=("-framework AppKit")
ldflags+=("-framework QuartzCore")

#defines+=("-DCURSORYX_ERROR_ABORT")
#defines+=("-DCURSORYX_ERROR_SKIP")
defines+=("-DCURSORYX_ERROR_LOG_DEBUG")

# customize depending on the chosen build type
if [ -z "$build" ]; then
	build=development
fi

case $build in
	development)
flags+=("-g")
defines+=("-DCURSORYX_ERROR_LOG_THROW")
	;;

	release)
flags+=("-D_FORTIFY_SOURCE=2")
flags+=("-fstack-protector-strong")
flags+=("-fPIE")
flags+=("-fPIC")
flags+=("-O2")
defines+=("-DCURSORYX_ERROR_LOG_MANUAL")
	;;

	sanitized_memory)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=leak")
flags+=("-fsanitize-recover=all")
defines+=("-DCURSORYX_ERROR_LOG_THROW")
	;;

	sanitized_undefined)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=undefined")
flags+=("-fsanitize-recover=all")
defines+=("-DCURSORYX_ERROR_LOG_THROW")
	;;

	sanitized_address)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=address")
flags+=("-fsanitize-address-use-after-scope")
flags+=("-fsanitize-recover=all")
defines+=("-DCURSORYX_ERROR_LOG_THROW")
	;;

	sanitized_thread)
flags+=("-g")
flags+=("-O1")
flags+=("-fno-omit-frame-pointer")
flags+=("-fno-optimize-sibling-calls")

flags+=("-fsanitize=thread")
flags+=("-fsanitize-recover=all")
	;;

	*)
echo "invalid build type"
exit 1
	;;
esac

# target toolchain type
if [ -z "$toolchain" ]; then
	toolchain=osxcross
fi

case $toolchain in
	osxcross)
name+="_osxcross"
cc="o64-clang"
ar="x86_64-apple-darwin21.4-ar"
	;;

	native)
name+="_native"
cc="clang"
ar="ar"
	;;

	*)
echo "invalid target toolchain type"
exit 1
	;;
esac

# common cursoryx lib for elf executables
ninja_file=lib_macho.ninja
src+=("src/common/cursoryx.c")
src+=("src/common/cursoryx_error.c")

# default target
default+=("\$folder_library/\$name.a")

# ninja start
mkdir -p "$output"

{ \
echo "# vars"; \
echo "builddir = $folder_ninja"; \
echo "folder_objects = $folder_objects"; \
echo "folder_cursoryx = $folder_cursoryx"; \
echo "folder_library = $folder_library"; \
echo "folder_include = $folder_include"; \
echo "name = $name"; \
echo "cc = $cc"; \
echo "ar = $ar"; \
echo ""; \
} > "$output/$ninja_file"

# ninja flags
echo "# flags" >> "$output/$ninja_file"

echo -n "flags =" >> "$output/$ninja_file"
for flag in "${flags[@]}"; do
	echo -ne " \$\n$flag" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

echo -ne "defines =" >> "$output/$ninja_file"
for define in "${defines[@]}"; do
	echo -ne " \$\n$define" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

# ninja rules
{ \
echo "# rules"; \
echo "rule ar"; \
echo "    command = \$ar rcs \$out \$in"; \
echo "    description = ar \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule cc"; \
echo "    deps = gcc"; \
echo "    depfile = \$out.d"; \
echo "    command = \$cc \$flags \$defines -MMD -MF \$out.d -c \$in -o \$out"; \
echo "    description = cc \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule cp"; \
echo "    command = cp \$in \$out"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule clean"; \
echo "    command = make/scripts/clean.sh"; \
echo "    description = cleaning repo"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "rule generator"; \
echo "    command = make/lib/elf.sh $build"; \
echo "    description = re-generating the ninja build file"; \
echo ""; \
} >> "$output/$ninja_file"

# ninja targets
## copy headers
{ \
echo "# copy headers"; \
echo "build \$folder_include/cursoryx.h: \$"; \
echo "cp src/include/cursoryx.h"; \
echo "build \$folder_include/cursoryx_appkit.h: \$"; \
echo "cp src/include/cursoryx_appkit.h"; \
echo ""; \
} >> "$output/$ninja_file"

{ \
echo "build headers: phony \$"; \
echo "\$folder_include/cursoryx.h \$"; \
echo "\$folder_include/cursoryx_appkit.h"; \
echo ""; \
} >> "$output/$ninja_file"

## compile sources
echo "# compile sources" >> "$output/$ninja_file"
for file in "${src[@]}"; do
	folder=$(dirname "$file")
	filename=$(basename "$file" .c)
	obj+=("\$folder_objects/$folder/$filename.o")
	{ \
	echo "build \$folder_objects/$folder/$filename.o: \$"; \
	echo "cc $file"; \
	echo ""; \
	} >> "$output/$ninja_file"
done

## archive object
echo "# archive objects" >> "$output/$ninja_file"
echo -ne "build \$folder_library/\$name.a: ar" >> "$output/$ninja_file"
for file in "${obj[@]}"; do
	echo -ne " \$\n$file" >> "$output/$ninja_file"
done
echo -e "\n" >> "$output/$ninja_file"

## special targets
{ \
echo "# run special targets"; \
echo "build regen: generator"; \
echo "build clean: clean"; \
echo "default" "${default[@]}"; \
} >> "$output/$ninja_file"
