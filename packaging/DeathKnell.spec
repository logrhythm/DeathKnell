Name:          DeathKnell
Version:       %{version}
Release:       %{buildnumber}%{?dist}
Summary:       An implementation of an "on-death" callback structure built from g3log
Group:         Development/Tools
License:       MIT
URL:           https://github.com/logrhythm/deathknell
BuildRequires: cmake >= 2.8, gperftools-libs >= 2.7, g3logrotate, FileIO, gtest-devel >= 1.8.0, gtest >= 1.8.0
Requires:      g3log, dpiUser
ExclusiveArch: x86_64

%description

%prep
cd ~/rpmbuild/BUILD
rm -rf %{name}
mkdir -p %{name}
cd %{name}
tar xzf ~/rpmbuild/SOURCES/%{name}-%{version}.tar.gz
if [ $? -ne 0 ]; then
   exit $?
fi

%build
cd %{name}/
PATH=/usr/local/probe/bin:$PATH
rm -f  CMakeCache.txt


if [ "%{buildtype}" == "-DUSE_LR_DEBUG=OFF" ]; then
   cmake -DVERSION:STRING=%{version}.%{buildnumber} \
      -DCMAKE_CXX_COMPILER_ARG1:STRING=' -std=c++14 -Wall -fPIC -Ofast -m64 -isystem/usr/local/probe/include -Wl,-rpath -Wl,. -Wl,-rpath -Wl,/usr/local/probe/lib ' \
      -DCMAKE_BUILD_TYPE:STRING=Release -DBUILD_SHARED_LIBS:BOOL=ON
elif [ "%{buildtype}" == "-DUSE_LR_DEBUG=ON" ]; then
   cmake -DUSE_LR_DEBUG=ON -DVERSION:STRING=%{version}.%{buildnumber} \
      -DCMAKE_CXX_COMPILER_ARG1:STRING=' -std=c++14  -Wall -Werror -g -gdwarf-2 --coverage -O0 -fPIC -m64 -isystem/usr/local/probe/include -Wl,-rpath -Wl,. -Wl,-rpath -Wl,/usr/local/probe/lib '
else
   echo "Unknown buildtype:" "%{buildtype}"
   exit 1
fi

make -j6
./UnitTestRunner
if [ "%{buildtype}" == "-DUSE_LR_DEBUG=ON" ]; then
   /usr/local/probe/bin/CodeCoverage.py
   username=$(whoami)
   userhome=$HOME
   sudo chown -R ${username}:${username} ${userhome}/rpmbuild/BUILD/*
fi
mkdir -p $RPM_BUILD_ROOT/usr/local/probe/lib
cp -rfd lib%{name}.so* $RPM_BUILD_ROOT/usr/local/probe/lib
mkdir -p $RPM_BUILD_ROOT/usr/local/probe/include
cp src/*.h $RPM_BUILD_ROOT/usr/local/probe/include


%post

%preun

%postun

%files
%defattr(-,dpi,dpi,-)
/usr/local/probe/lib
/usr/local/probe/include
