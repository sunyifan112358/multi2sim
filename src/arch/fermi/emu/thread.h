/*
 *  Multi2Sim
 *  Copyright (C) 2012  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This module is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This module is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this module; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef FERMI_EMU_THREAD_H
#define FERMI_EMU_THREAD_H

#include <arch/fermi/asm/asm.h>


/*
 * Class 'FrmThread'
 */

#define FRM_MAX_LOCAL_MEM_ACCESSES_PER_INST  2

typedef union
{
	unsigned int u32;
	int s32;
	float f;
} FrmThreadReg;


struct frm_mem_access_t
{
	int type;  /* 0-none, 1-read, 2-write */
	unsigned int addr;
	int size;
};


CLASS_BEGIN(FrmThread, Object)

	/* IDs */
	int id;
	int id_in_warp;
	int id_in_thread_block;

	/* 3-dimensional IDs */
	int id_3d[3];  /* global 3D IDs */
	int id_in_thread_block_3d[3];  /* local 3D IDs */

	/* Warp, thread_block, and grid where it belongs */
	FrmWarp *warp;
	FrmThreadBlock *thread_block;
	FrmGrid *grid;

	/* Thread state */
	FrmThreadReg gpr[128];  /* General purpose registers */
	FrmThreadReg sr[FrmInstSRegCount];  /* Special registers */
	unsigned int pr[8];  /* Predicate registers */

	/* Linked list of write tasks. They are enqueued by machine instructions
	 * and executed as a burst at the end of an ALU group. */
	struct linked_list_t *write_task_list;

	/* FIXME: LDS (Local Data Share) OQs (Output Queues) */
	struct list_t *lds_oqa;
	struct list_t *lds_oqb;

	/* This is a digest of the active mask updates for this thread. Every time
	 * an instruction updates the active mask of a warp, this digest is updated
	 * for active threads by XORing a random number common for the warp.
	 * At the end, threads with different 'branch_digest' numbers can be considered
	 * divergent threads. */
	unsigned int branch_digest;

	/* Last global memory access */
	unsigned int global_mem_access_addr;
	unsigned int global_mem_access_size;

	/* Last local memory access */
	int lds_access_count;  /* Number of local memory access performed by last instruction */
	unsigned int lds_access_addr[FRM_MAX_LOCAL_MEM_ACCESSES_PER_INST];
	unsigned int lds_access_size[FRM_MAX_LOCAL_MEM_ACCESSES_PER_INST];
	int lds_access_type[FRM_MAX_LOCAL_MEM_ACCESSES_PER_INST];  /* 0-none, 1-read, 2-write */

CLASS_END(FrmThread)


#define FRM_FOREACH_THREAD_IN_GRID(GRID, THREAD_ID) \
	for ((THREAD_ID) = (GRID)->thread_id_first; \
		(THREAD_ID) <= (GRID)->thread_id_last; \
		(THREAD_ID)++)
#define FRM_FOREACH_THREAD_IN_THREADBLOCK(THREADBLOCK, THREAD_ID) \
	for ((THREAD_ID) = (THREADBLOCK)->thread_id_first; \
		(THREAD_ID) <= (THREADBLOCK)->thread_id_last; \
		(THREAD_ID)++)
#define FRM_FOREACH_THREAD_IN_WARP(WARP, THREAD_ID) \
	for ((THREAD_ID) = (WARP)->thread_id_first; \
		(THREAD_ID) <= (WARP)->thread_id_last; \
		(THREAD_ID)++)


void FrmThreadCreate(FrmThread *self);
void FrmThreadDestroy(FrmThread *self);

int FrmThreadGetActive(FrmThread *self);
void FrmThreadSetActive(FrmThread *self, int active);

int FrmThreadGetPred(FrmThread *self);
void FrmThreadSetPred(FrmThread *self, int pred);

void FrmThreadUpdateBranchDigest(FrmThread *self,
	long long inst_count, unsigned int inst_addr);


#endif

