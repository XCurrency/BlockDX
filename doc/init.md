Sample init scripts and service configuration for xc3dxd
==========================================================

Sample scripts and configuration files for systemd, Upstart and OpenRC
can be found in the contrib/init folder.

    contrib/init/xc3dxd.service:    systemd service unit configuration
    contrib/init/xc3dxd.openrc:     OpenRC compatible SysV style init script
    contrib/init/xc3dxd.openrcconf: OpenRC conf.d file
    contrib/init/xc3dxd.conf:       Upstart service configuration file
    contrib/init/xc3dxd.init:       CentOS compatible SysV style init script

1. Service User
---------------------------------

All three startup configurations assume the existence of a "xc3dx" user
and group.  They must be created before attempting to use these scripts.

2. Configuration
---------------------------------

At a bare minimum, xc3dxd requires that the rpcpassword setting be set
when running as a daemon.  If the configuration file does not exist or this
setting is not set, xc3dxd will shutdown promptly after startup.

This password does not have to be remembered or typed as it is mostly used
as a fixed token that xc3dxd and client programs read from the configuration
file, however it is recommended that a strong and secure password be used
as this password is security critical to securing the wallet should the
wallet be enabled.

If xc3dxd is run with "-daemon" flag, and no rpcpassword is set, it will
print a randomly generated suitable password to stderr.  You can also
generate one from the shell yourself like this:

bash -c 'tr -dc a-zA-Z0-9 < /dev/urandom | head -c32 && echo'

Once you have a password in hand, set rpcpassword= in /etc/xc3dx/xc3dx.conf

For an example configuration file that describes the configuration settings,
see contrib/debian/examples/xc3dx.conf.

3. Paths
---------------------------------

All three configurations assume several paths that might need to be adjusted.

Binary:              /usr/bin/xc3dxd
Configuration file:  /etc/xc3dx/xc3dx.conf
Data directory:      /var/lib/xc3dxd
PID file:            /var/run/xc3dxd/xc3dxd.pid (OpenRC and Upstart)
                     /var/lib/xc3dxd/xc3dxd.pid (systemd)

The configuration file, PID directory (if applicable) and data directory
should all be owned by the xc3dx user and group.  It is advised for security
reasons to make the configuration file and data directory only readable by the
xc3dx user and group.  Access to xc3dx-cli and other xc3dxd rpc clients
can then be controlled by group membership.

4. Installing Service Configuration
-----------------------------------

4a) systemd

Installing this .service file consists on just copying it to
/usr/lib/systemd/system directory, followed by the command
"systemctl daemon-reload" in order to update running systemd configuration.

To test, run "systemctl start xc3dxd" and to enable for system startup run
"systemctl enable xc3dxd"

4b) OpenRC

Rename xc3dxd.openrc to xc3dxd and drop it in /etc/init.d.  Double
check ownership and permissions and make it executable.  Test it with
"/etc/init.d/xc3dxd start" and configure it to run on startup with
"rc-update add xc3dxd"

4c) Upstart (for Debian/Ubuntu based distributions)

Drop xc3dxd.conf in /etc/init.  Test by running "service xc3dxd start"
it will automatically start on reboot.

NOTE: This script is incompatible with CentOS 5 and Amazon Linux 2014 as they
use old versions of Upstart and do not supply the start-stop-daemon uitility.

4d) CentOS

Copy xc3dxd.init to /etc/init.d/xc3dxd. Test by running "service xc3dxd start".

Using this script, you can adjust the path and flags to the xc3dxd program by
setting the xc3D and FLAGS environment variables in the file
/etc/sysconfig/xc3dxd. You can also use the DAEMONOPTS environment variable here.

5. Auto-respawn
-----------------------------------

Auto respawning is currently only configured for Upstart and systemd.
Reasonable defaults have been chosen but YMMV.
