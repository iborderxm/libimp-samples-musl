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

# Function to check for undefined references in error output
check_undefined_references() {
	local output="$1"
	if echo "$output" | grep -q "undefined reference"; then
		echo "========================================"
		echo "ERROR: Undefined references detected!"
		echo "========================================"
		echo "$output" | grep "undefined reference" | sed 's/^/  /'
		echo "========================================"
		echo "Possible causes:"
		echo "  1. Missing library implementation in libsysutils.so"
		echo "  2. Library functions not available in current build"
		echo "  3. Need to define SU_CIPHER_AVAILABLE macro to enable cipher functions"
		echo "========================================"
		return 1
	fi
	return 0
}

# Function to run make with error checking
run_make_with_check() {
	local dir="$1"
	local output
	local ret
	
	echo "Running make in $dir..."
	output=$($MAKE_CMD 2>&1)
	ret=$?
	
	if [ $ret -ne 0 ]; then
		echo "========================================"
		echo "ERROR: Compilation failed in $dir"
		echo "========================================"
		echo "$output"
		echo "========================================"
		
		# Check for undefined references
		if ! check_undefined_references "$output"; then
			echo ""
			echo "Suggested fix:"
			echo "  Define SU_CIPHER_AVAILABLE in Makefile or compile with:"
			echo "  make CFLAGS=\"-DSU_CIPHER_AVAILABLE\""
			echo ""
		fi
		
		return $ret
	fi
	
	# Check for warnings even if compilation succeeded
	if echo "$output" | grep -q "warning:"; then
		echo "========================================"
		echo "WARNINGS detected in $dir:"
		echo "========================================"
		echo "$output" | grep "warning:" | sed 's/^/  /'
		echo "========================================"
	fi
	
	return 0
}

# Define list of base directories
folders=("T31")

# Track overall compilation status
overall_status=0

# Iterate over each folder
for folder in "${folders[@]}"; do
	echo "Processing folder: $folder"

	# Change to folder and remember to return later
	pushd "$folder" > /dev/null

	# libsysutils
	if [ -d "libsysutils" ]; then
		pushd "libsysutils" > /dev/null
		
		# Check if SU_CIPHER functions exist in libsysutils.so before compiling
		if [ -f "../lib/uclibc/libsysutils.so" ]; then
			echo "Checking for SU_CIPHER functions in libsysutils.so..."
			if command -v mipsel-linux-objdump > /dev/null; then
				SU_CIPHER_COUNT=$(mipsel-linux-objdump -T ../lib/uclibc/libsysutils.so 2>/dev/null | grep -c "SU_CIPHER_" || echo "0")
			elif command -v mipsel-linux-nm > /dev/null; then
				SU_CIPHER_COUNT=$(mipsel-linux-nm -D ../lib/uclibc/libsysutils.so 2>/dev/null | grep -c "SU_CIPHER_" || echo "0")
			else
				SU_CIPHER_COUNT="unknown"
			fi
			
			if [ "$SU_CIPHER_COUNT" = "0" ]; then
				echo "========================================"
				echo "WARNING: No SU_CIPHER functions found in libsysutils.so"
				echo "========================================"
				echo "The sample-chipid.c file uses SU_CIPHER functions but they are not"
				echo "available in the libsysutils.so library."
				echo ""
				echo "Options:"
				echo "  1. Compile with SU_CIPHER_AVAILABLE macro disabled (current sample-chipid.c supports this)"
				echo "  2. Ensure libsysutils.so is built with SU_CIPHER support"
				echo "  3. Check if you need a different version of libsysutils.so"
				echo "========================================"
				echo ""
			elif [ "$SU_CIPHER_COUNT" != "unknown" ]; then
				echo "Found $SU_CIPHER_COUNT SU_CIPHER functions in libsysutils.so"
			else
				echo "Cannot check for SU_CIPHER functions (objdump/nm not available)"
			fi
		else
			echo "Warning: libsysutils.so not found, skipping SU_CIPHER check"
		fi
		
		if ! run_make_with_check "libsysutils/$folder"; then
			overall_status=1
			popd > /dev/null
			popd > /dev/null
			break
		fi
		popd > /dev/null
	else
		echo "libsysutils directory does not exist in $folder"
	fi

	popd > /dev/null
done

# Report final status
if [ $overall_status -eq 0 ]; then
	echo "========================================"
	echo "All done. Compilation successful!"
	echo "========================================"
else
	echo "========================================"
	echo "Compilation failed with errors!"
	echo "========================================"
	exit 1
fi
