/* upstart
 *
 * test_util_check_env.c - meta test to ensure environment sane for
 * running tests.
 *
 * Copyright © 2013 Canonical Ltd.
 * Author: James Hunt <james.hunt@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <mntent.h>
#include <sys/vfs.h>

#include <nih/string.h>
#include <nih/test.h>
#include <nih/logging.h>

#include "test_util_common.h"

#ifndef OVERLAYFS_SUPER_MAGIC
#define OVERLAYFS_SUPER_MAGIC 0x794c764f
#endif

/**
 * check_for_overlayfs:
 *
 * Determine if the mount point used by the tests for creating temporary
 * files is using overlayfs.
 *
 * Returns: TRUE if temporary work area is on overlayfs, else FALSE.
 **/
int
check_for_overlayfs (void)
{
	struct statfs  statbuf;
	char           path[PATH_MAX];
	int            found = FALSE;

	/* Create a file in the temporary work area */
	TEST_FILENAME (path);
	fclose (fopen (path, "w"));

	/* Check it exits */
	assert0 (statfs (path, &statbuf));

	if (statbuf.f_type == OVERLAYFS_SUPER_MAGIC) {
		nih_warn ("Mountpoint for '%s' (needed by the Upstart tests) is an overlayfs "
				"filesystem, which does not support inotify.",
				path);
		found = TRUE;
	}

	assert0 (unlink (path));

	return found;
}

/**
 * test_checks:
 *
 * Perform any checks necessary before real tests are run.
 **/
void
test_checks (void)
{
	TEST_GROUP ("test environment");

	/*
	 * Warn (*) if overlayfs detected.
	 *
	 * (*) - Don't fail in the hope that one day someone might fix
	 * overlayfs.
	 */
	TEST_FEATURE ("checking for overlayfs");
	if (check_for_overlayfs ()) {
		nih_warn ("Found overlayfs mounts");
		nih_warn ("This environment will probably cause tests to fail mysteriously!!");
		nih_warn ("See bug LP:#882147 for further details.");
	}
}

int
main (int   argc,
      char *argv[])
{
	test_checks ();

	return 0;
}
