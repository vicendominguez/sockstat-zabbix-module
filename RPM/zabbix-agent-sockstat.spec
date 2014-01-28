Name:       zabbix-agent-sockstat
Version:    2.00 
Release:    1%{?dist}
Summary:    Agent module for zabbix for network sockets status

Group:      Applications/Internet
License:    GPLv2+
Source0:    %{name}-%{version}.tar.gz
Requires:   zabbix-agent => 2.2
Requires(post) : /sbin/service
Requires(preun) : /sbin/service


%description
Agent module to parse the /proc/net/sockstat info for Zabbix > 2.2.x agent.
Info: https://github.com/vicendominguez/sockstat-zabbix-module

%prep
%setup -q


%build


%install
install --directory $RPM_BUILD_ROOT/usr/lib/zabbix/agentmodules
install --directory $RPM_BUILD_ROOT/etc/zabbix/zabbix_agentd.d

install -m 0755 zbx_sockstat.so $RPM_BUILD_ROOT/usr/lib/zabbix/agentmodules/
install -m 0644 zabbix-agent-sockstat.conf $RPM_BUILD_ROOT/etc/zabbix/zabbix_agentd.d/

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/lib/zabbix/agentmodules
/etc/zabbix/zabbix_agentd.d/zabbix-agent-sockstat.conf

%post
/sbin/service zabbix-agent restart >/dev/null 2>&1

%postun
/sbin/service zabbix-agent restart >/dev/null 2>&1

%changelog
* Tue Jan 28 2014 Vicente Dominguez <twitter:@vicendominguez> - 2.00
- No malloc, random value solved and first production version

* Thu Jan 24 2014 Vicente Dominguez <twitter:@vicendominguez> - 0.99
- rpm for rhel6 (fast-way) 
