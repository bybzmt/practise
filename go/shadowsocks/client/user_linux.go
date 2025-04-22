//go:build linux

package client

import (
	"os/user"
	"strconv"
	"syscall"
)

func PrivilegeDropping(gid, uid string) error {
	if uid == "" && gid == "" {
		return nil
	}

	if gid != "" {
		if _, err := strconv.Atoi(gid); err != nil {
			u, err := user.LookupGroup(gid)
			if err != nil {
				return err
			}

			gid = u.Gid
		}
	}

	if _, err := strconv.Atoi(uid); err != nil {
		u, err := user.Lookup(uid)
		if err != nil {
			return err
		}

		uid = u.Uid
		if gid == "" {
			gid = u.Gid
		}
	}

	_gid, err := strconv.Atoi(gid)
	if err != nil {
		return err
	}

	_uid, err := strconv.Atoi(uid)
	if err != nil {
		return err
	}

	if err := syscall.Setgid(_gid); err != nil {
		return err
	}

	if err := syscall.Setuid(_uid); err != nil {
		return err
	}

	return nil
}
