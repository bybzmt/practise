//go:build !linux

package server

func PrivilegeDropping(group, user string) error {
	return nil
}
