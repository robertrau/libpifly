#!/bin/bash

function check_yn()
{
	read ans
	if [ "$ans" = "n" ] || [ "$ans" = "N" ]; then\
		echo "User chose n, exiting"
		exit 1
	elif [ "$ans" != "y" ] && [ "$ans" != "Y" ] && [ "$ans" != "" ]; then
		echo "Un-recognized option, exiting"
		exit 2
	fi
}


function get_install_dir()
{
	installDir=${HOME}/raspberrypi
	echo -n "Install toolchain and rootfs to [$installDir]: "
	read ans

	if [ "$ans" != "" ]; then
		installDir=$ans
	fi
}

function download_and_extract_noobs()
{
	if [ -z $downloadDir ]; then
		exit 1
	fi
	echo -n "This script will go out and download NOOBS 2.3.0 from http://director.downloads.raspberrypi.org/NOOBS/images/NOOBS-2017-03-03/NOOBS_v2_3_0.zip and extract the rootfs for building against, continue? [Y/n]: "
	check_yn
	echo "Installing to $installDir"
	cd $downloadDir
	echo "Downloading NOOBS"
	curl -o NOOBS_v2_3_0.zip http://director.downloads.raspberrypi.org/NOOBS/images/NOOBS-2017-03-03/NOOBS_v2_3_0.zip
	unzip NOOBS_v2_3_0.zip
}

function clean_up_downloadsDir()
{
	if [ -z $downloadDir ]; then
		exit 1
	fi
	cd $downloadDir
	rm -rf *
}

function extract_rootfs()
{
	if [ -z $downloadDir ]; then
		exit 1
	fi
	if [ -z $rootfsDir ]; then
		exit 1
	fi
	cd $downloadDir
	cp os/Raspbian/root.tar.xz $rootfsDir
	cd $rootfsDir
	echo "Extracting root filesystem..."
	tar xf root.tar.xz > /dev/null
	rm root.tar.xz
}

function clone_toolchain()
{
	if [ "$installDir" = "" ]; then
		exit 1
	fi
	cd $installDir
	echo -n "Setup will now clone raspberry pi toolchain from https://github.com/raspberrypi/tools.git, proceed? [Y/n]: "
	check_yn
	
	git clone https://github.com/raspberrypi/tools.git
}

function clone_kernel()
{
	if [ "$installDir" = "" ]; then
		exit 1
	fi
	cd $installDir
	echo -n "Setup will now clone raspberry pi kernel from https://github.com/raspberrypi/linux.git, proceed? [Y/n]: "
	check_yn
	
	git clone --depth=1 https://github.com/raspberrypi/linux.git
}

function install_kernel_headers()
{
	if [ "$kernelDir" = "" ]; then
		exit 1
	fi
	cd $kernelDir
	git checkout rpi-4.9.y

	KERNEL=kernel7
	make ARCH=arm CROSS_COMPILE=$toolchainDir/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf- bcm2709_defconfig
	make headers_install ARCH=arm INSTALL_HDR_PATH=$rootfsDir/usr
}

function setup_install_fs()
{
	if [ "$installDir" = "" ]; then
		exit 1
	fi
	downloadDir=$installDir/tmp
	rootfsDir=$installDir/rootfs
	toolchainDir=$installDir/tools
	kernelDir=$installDir/linux
	mkdir -p $downloadDir
	mkdir -p $rootfsDir
	mkdir -p $toolchainDir
	mkdir -p $kernelDir
}

function download_bcm2835()
{
	if [ -z $downloadDir ]; then
		exit 1
	fi
	cd $downloadDir
	echo -n "This script will go out and download, build, and install libbcm2835 from http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz, continue? [Y/n]: "
	check_yn
	
	curl -o bcm2835-1.52.tar.gz http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz
	tar xf bcm2835-1.52.tar.gz
	mv bcm2835-1.52 ../
	rm bcm2835-1.52.tar.gz
	bcmlibDir=$installDir/bcm2835-1.52
}

function build_install_bcmLib()
{
	if [ -z $bcmlibDir ]; then
		exit 1
	fi
	cd $bcmlibDir
	echo "Building libbcm2835"
	export PATH="$toolchainDir/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/:$PATH"
	./configure --prefix=$rootfsDir/usr/local CFLAGS="--sysroot=$rootfsDir" --host arm-linux-gnueabihf

	make
	make install
}

function build_toolchain_file()
{
	if [ -z $rootfsDir ]; then
		exit 1
	fi
	if [ -z $toolchainDir ]; then
		exit 1
	fi
	if [ -z $startDir ]; then
		exit 1
	fi
	cd $startDir

	res=$(uname -p)
	if [ "$res" = "x86_64" ]; then
		echo "Selecting 64bit build platform"
		binDir=$toolchainDir/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
	elif [ "$res" = "x86" ]; then
		echo "Selecting 32bit build platform"
		binDir=$toolchainDir/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin
	else
		echo "Unsupported build platform, exiting"
		exit 1
	fi

	sed "s|<placeholder_rootfs>|$rootfsDir|g" ./scripts/pi.cmake.template | sed "s|<placeholder_bin>|$binDir|g" > ./scripts/pi.cmake
}

function check_for()
{
	program=$1
	echo "Checking to make sure $program is installed"
	searchRes=$(whereis $program | sed "s/$program://g")
	if [ -n "$searchRes" ]; then
		echo "Found $program"
	else
		echo "Could not find $program, exiting"
		exit 1
	fi
}

if [ "$0" != "scripts/dev_setup.sh" ] && [ "$0" != "./scripts/dev_setup.sh" ]; then
	echo "This script must be run from the root of the repository. Exiting"
	exit 1
fi

startDir=$(pwd)

check_for "git"
check_for "cmake"
check_for "unzip"
check_for "tar"
check_for "curl"
check_for "sed"

get_install_dir
setup_install_fs
download_and_extract_noobs
extract_rootfs
clean_up_downloadsDir
clone_toolchain
clone_kernel
install_kernel_headers
download_bcm2835
build_install_bcmLib
build_toolchain_file
