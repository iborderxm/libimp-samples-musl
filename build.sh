#!/bin/bash

# Check for ccache
if command -v ccache > /dev/null; then
	export CROSS_COMPILE="ccache mipsel-linux-"
else
	export CROSS_COMPILE="mipsel-linux-"
fi

# Check if the -clean flag is set
if [[ "$1" == "" ]]; then
	echo "run with --make or --clean"
	exit 0
elif [[ "$1" == "--clean" ]]; then
	MAKE_CMD="make distclean"
	rm -f ingenic_musl/musl_shim.o
elif [[ "$1" == "--make" ]]; then
	MAKE_CMD="make"
else
	echo "Invalid command"
	exit 1
fi

# Define the list of base directories
folders=("T31")

# Iterate over each folder
for folder in "${folders[@]}"; do
	echo "Processing folder: $folder"

	# Change to the folder and remember to return later
	pushd "$folder"

	# libsysutils
	if [ -d "libsysutils" ]; then
		pushd "libsysutils"
		$MAKE_CMD
		popd # Return from libsysutils
	else
		echo "libsysutils directory does not exist in $folder"
	fi

	popd # Return to the starting directory
done

echo "All done."
