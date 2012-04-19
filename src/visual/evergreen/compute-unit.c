/*
 *  Multi2Sim Tools
 *  Copyright (C) 2011  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <visual-evergreen.h>


struct vi_evg_compute_unit_t *vi_evg_compute_unit_create(char *name)
{
	struct vi_evg_compute_unit_t *compute_unit;

	/* Allocate */
	compute_unit = calloc(1, sizeof(struct vi_evg_compute_unit_t));
	if (!compute_unit)
		fatal("%s: out of memory", __FUNCTION__);

	/* Name */
	compute_unit->name = strdup(name);
	if (!name)
		fatal("%s: out of memory", __FUNCTION__);

	/* Initialize */
	compute_unit->work_group_table = hash_table_create(0, FALSE);

	/* Return */
	return compute_unit;
}


void vi_evg_compute_unit_free(struct vi_evg_compute_unit_t *compute_unit)
{
	struct vi_evg_work_group_t *work_group;

	char *work_group_name;

	/* Free work-groups */
	HASH_TABLE_FOR_EACH(compute_unit->work_group_table, work_group_name, work_group)
		vi_evg_work_group_free(work_group);
	hash_table_free(compute_unit->work_group_table);

	/* Free compute unit */
	free(compute_unit->name);
	free(compute_unit);
}


void vi_evg_compute_unit_read_checkpoint(struct vi_evg_compute_unit_t *compute_unit, FILE *f)
{
	char *work_group_name;

	struct vi_evg_work_group_t *work_group;

	/* Empty work-group list */
	HASH_TABLE_FOR_EACH(compute_unit->work_group_table, work_group_name, work_group)
		vi_evg_work_group_free(work_group);
	hash_table_clear(compute_unit->work_group_table);
}


void vi_evg_compute_unit_write_checkpoint(struct vi_evg_compute_unit_t *compute_unit, FILE *f)
{
}
