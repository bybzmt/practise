//go:build !linux

package serve

func PrivilegeDropping(group, user string) error {
	return nil
}
