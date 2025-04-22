//go:build !linux

package client

func PrivilegeDropping(group, user string) error {
	return nil
}
