%define     main_version  4.0
%define     minor_version 4
%define     module_dir    src/modules

Name:       zabbix-agent-sockstat
Version:    %{main_version}.%{minor_version}
Release:    1%{?dist}
Summary:    Agent module for zabbix for network sockets status

Group:      Applications/Internet
License:    GPLv2+
Source1:    zbx_sockstat.c
Source2:    Makefile

Requires:   zabbix-agent >= 4.0.0, zabbix-agent < 4.1.0

%if 0%{?rhel} >= 7
BuildRequires:   systemd
Requires(post):  systemd
Requires(preun): systemd
%else
Requires(preun): /sbin/service
Requires(post): /sbin/service
%endif


%description
Agent module to parse the /proc/net/sockstat info for Zabbix > 2.2.x agent.
Info: https://github.com/vicendominguez/sockstat-zabbix-module

%prep
curl -o /tmp/zbx.rpm https://repo.zabbix.com/zabbix/%{main_version}/rhel/7/SRPMS/zabbix-%{version}-1.el7.src.rpm
rpm -i /tmp/zbx.rpm
%setup -qTcn zabbix-%{version}
tar --strip-components=1 -xf %{_sourcedir}/zabbix-%{version}.tar.gz
echo 'LoadModule=zbx_sockstat.so' > %{_sourcedir}/zabbix-agent-sockstat.conf

%build
%configure
mkdir -p %{module_dir}/%{name}
cp %{SOURCE1} %{SOURCE2} %{module_dir}/%{name}/
cd %{module_dir}/%{name}/
make

%install
install --directory %{buildroot}%{_libdir}/zabbix/modules/
install --directory %{buildroot}/etc/zabbix/zabbix_agentd.d

install -m 0755 %{_builddir}/zabbix-%{version}/%{module_dir}/%{name}/zbx_sockstat.so %{buildroot}%{_libdir}/zabbix/modules/
install -m 0644 %{_sourcedir}/zabbix-agent-sockstat.conf                             %{buildroot}/etc/zabbix/zabbix_agentd.d/

%clean
rm -rf %{buildroot}

%files
/%{_libdir}/zabbix/modules/
/etc/zabbix/zabbix_agentd.d/zabbix-agent-sockstat.conf

%post
%if 0%{?rhel} >= 7
/usr/bin/systemctl try-restart zabbix-agent.service >/dev/null 2>&1 || :
%else
/sbin/service zabbix-agent restart >/dev/null 2>&1 || :
%endif

%postun
%if 0%{?rhel} >= 7
/usr/bin/systemctl try-restart zabbix-agent.service >/dev/null 2>&1 || :
%else
/sbin/service zabbix-agent restart >/dev/null 2>&1 || :
%endif

%changelog
* Fri Feb 8  2019 Anatolii Vorona <vorona.tolik@gmail.com>    - 3.4
- Updated spec for EPEL7 and systemd

* Tue Jan 28 2014 Vicente Dominguez <twitter:@vicendominguez> - 2.00
- No malloc, random value solved and first production version

* Fri Jan 24 2014 Vicente Dominguez <twitter:@vicendominguez> - 0.99
- rpm for rhel6 (fast-way) 
