#!/bin/bash
set -e
PACKAGE=DeathKnell

if [[ $# -ne 1 ]] ; then
    echo 'Usage:  sh buildRpm <BUILD_TYPE>'
    echo '        BUILD_TYPE is PRODUCTION or COVERAGE'
    exit 0
fi
 
VERSION="$1"
BUILDSTRING=
if [ "$1" = "PRODUCTION" ] ; then
   BUILD_TYPE="-DUSE_DEBUG_COVERAGE=OFF"
   BUILDSTRING="production"
elif  [ "$1" = "COVERAGE" ] ; then
   BUILD_TYPE="-DUSE_DEBUG_COVERAGE=ON"
   BUILDSTRING="debug"
else
   echo "<BUILD_TYPE> must be one of: PRODUCTION or COVERAGE"
   exit 0
fi


# As version number we use the commit number on HEAD 
# we do not bother with other branches for now
GIT_VERSION=`git rev-list HEAD --count`
VERSION="1.$GIT_VERSION"

echo "Building $PACKAGE, type: $BUILD_TYPE, version: $VERSION"

PWD=`pwd`
CWD=$PWD/$PACKAGE
DISTDIR=$CWD/dist/$PACKAGE
PATH=$PATH:/usr/local/probe/bin:$PATH

rm -rf ~/rpmbuild
rpmdev-setuptree
cp packaging/$PACKAGE.spec ~/rpmbuild/SPECS
rm -f $PACKAGE-$VERSION.tar.gz
tar czf $PACKAGE-$VERSION.tar.gz ./*
mkdir -p ~/rpmbuild/SOURCES
cp $PACKAGE-$VERSION.tar.gz ~/rpmbuild/SOURCES
cd ~/rpmbuild


rpmbuild -v -bb  --define="version ${VERSION}" --define="buildtype {$BUILD_TYPE}" --define="buildstring {$BUILDSTRING}" --target=x86_64 ~/rpmbuild/SPECS/$PACKAGE.spec

# Copy the artifacts to the local distribution directory
rm -rf $DISTDIR
mkdir -p $DISTDIR/include/
cp -r ~/rpmbuild/BUILD/$PACKAGE/src/*.h $DISTDIR/include
mkdir -p $DISTDIR/lib/
cp -r ~/rpmbuild/BUILD/$PACKAGE/*.so $DISTDIR/lib

