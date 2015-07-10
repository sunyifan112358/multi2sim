/*
 *  Multi2Sim
 *  Copyright (C) 2012  Rafael Ubal (ubal@ece.neu.edu)
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

#ifndef ARCH_X86_TIMING_THREAD_H
#define ARCH_X86_TIMING_THREAD_H

#include <deque>
#include <string>

#include <memory/Module.h>
#include <arch/x86/emulator/Uinst.h>
#include <arch/x86/emulator/Context.h>

#include "Uop.h"
#include "BranchPredictor.h"
#include "RegisterFile.h"
#include "TraceCache.h"


namespace x86
{

// Forward declarations
class Core;
class Cpu;
class Timing;

/// X86 Thread
class Thread
{
private:

	// Name, assigned in constructor
	std::string name;

	// Cpu the thread belongs to, assigned in constructor
	Cpu *cpu;

	// Core the thread belongs to, assigned in constructor
	Core *core;

	// Thread index in the core, assigned in constructor
	int id_in_core;

	// Global thread ID in the CPU, assigned in constructor
	int id_in_cpu;




	//
	// Number of uops in private structures
	//

	// Uop count in instruction queue
	int uop_count_in_instruction_queue = 0;

	// Uop count in load/store queue
	int uop_count_in_load_store_queue = 0;

	// Number of occupied integer registers
	int num_integer_registers_occupied = 0;

	// Number of occupied float point registers
	int num_float_point_registers_occupied = 0;

	// Number of XMM registers
	int num_xmm_registers_occupied = 0;




	//
	// Queues
	//

	// Insert a uop into the tail of the fetch queue
	void InsertInFetchQueue(std::shared_ptr<Uop> uop);

	// Extract a uop from the head of the fetch queue
	std::shared_ptr<Uop> ExtractFromFetchQueue();

	// Fetch queue
	std::deque<std::shared_ptr<Uop>> fetch_queue;

	// Insert a uop into the tail of the uop queue
	void InsertInUopQueue(std::shared_ptr<Uop> uop);

	// Extract a uop from the head of the uop queue
	std::shared_ptr<Uop> ExtractFromUopQueue();

	// Uop queue
	std::deque<std::shared_ptr<Uop>> uop_queue;

	// Instruction queue
	std::list<std::shared_ptr<Uop>> instruction_queue;

	// Load queue
	std::list<std::shared_ptr<Uop>> load_queue;

	// Store queue
	std::list<std::shared_ptr<Uop>> store_queue;




	//
	// Component pointer
	//

	// Branch predictor
	std::unique_ptr<BranchPredictor> branch_predictor;

	// Trace cache
	std::unique_ptr<TraceCache> trace_cache;

	// Physical register file
	std::unique_ptr<RegisterFile> reg_file;




	//
	// Fetch stage
	//

	// Current occupancy of the fetch queue in bytes
	int fetch_queue_occupancy = 0;

	// Current occupancy of the trace cache queue in bytes
	int trace_cache_queue_occupancy = 0;

	// Current instruction pointer
	unsigned int fetch_eip = 0;
	
	// Next instruction pointer
	unsigned int fetch_neip = 0;

	// Number of bytes occupied in the fetch queue
	int num_bytes_in_fetch_queue  = 0;

	// Number of uops occupied in the trace cache queue
	int num_uop_in_trace_cache_queue  = 0;

	// Virtual base address of the first byte in the cache block that was
	// fetched last
	unsigned int fetch_block_address = -1;

	// Physical address of last instruction fetch
	unsigned int fetch_address = 0;

	// Access identifier for of last instruction fetch
	long long fetch_access = 0;

	// Cycle until which fetching is stalled (inclusive)
	long long fetch_stall_until = 0;

	// Cycle in which last micro-instruction committed
	long long last_commit_cycle = 0;




	//
	// Statistics
	//

	// Number of fectched micro-instructions
	long long num_fetched_uinsts = 0;

	// Number of dispatched micro-instructions for every opcode
	long long num_dispatched_uinst_array[Uinst::OpcodeCount] = { };

	// Number of issued micro-instructions for every opcode
	long long num_issued_uinst_array[Uinst::OpcodeCount] = { };

	// Number of committed micro-instructions for every opcode
	long long num_committed_uinst_array[Uinst::OpcodeCount] = { };

	// Number of squashed micro-instructions
	long long num_squashed_uinst = 0;

	// Number of branch micro-instructions
	long long num_branch_uinst = 0;

	// Number of mis-predicted branch micro-instructions
	long long num_mispred_branch_uinst = 0;




	//
	// Statistics for structures
	//

	long long rob_occupancy = 0;
	long long rob_full = 0;
	long long rob_reads = 0;
	long long rob_writes = 0;

	long long iq_occupancy = 0;
	long long iq_full = 0;
	long long iq_reads = 0;
	long long iq_writes = 0;
	long long iq_wakeup_accesses = 0;

	long long lsq_occupancy = 0;
	long long lsq_full = 0;
	long long lsq_reads = 0;
	long long lsq_writes = 0;
	long long lsq_wakeup_accesses = 0;

	long long reg_file_int_occupancy = 0;
	long long reg_file_int_full = 0;
	long long reg_file_int_reads = 0;
	long long reg_file_int_writes = 0;

	long long reg_file_fp_occupancy = 0;
	long long reg_file_fp_full = 0;
	long long reg_file_fp_reads = 0;
	long long reg_file_fp_writes = 0;

	long long reg_file_xmm_occupancy = 0;
	long long reg_file_xmm_full = 0;
	long long reg_file_xmm_reads = 0;
	long long reg_file_xmm_writes = 0;

	long long rat_int_reads = 0;
	long long rat_int_writes = 0;
	long long rat_fp_reads = 0;
	long long rat_fp_writes = 0;
	long long rat_xmm_reads = 0;
	long long rat_xmm_writes = 0;

	long long btb_reads = 0;
	long long btb_writes = 0;

public:

	/// Constructor
	Thread(Core *core, int id_in_core);

	/// Return the core that this thread belongs to
	Core *getCore() const { return core; }

	/// Get thread index within the core
	int getIdInCore() const { return id_in_core; }

	/// Get the thread index within the CPU
	int getIdInCpu() const { return id_in_cpu; }




	//
	// Increment counters
	//

	/// Increment the number of occupied physical integer registers
	void incNumIntegerRegistersOccupied() { num_integer_registers_occupied++; }

	/// Increment the number of occupied physical float point registers
	void incNumFloatPointRegistersOccupied() { num_float_point_registers_occupied++; }

	/// Increment the number of occupied physical xmm registers
	void incNumXmmRegistersOccupied() { num_xmm_registers_occupied++; }

	/// Increment the read count of Register Aliasing Table for INT registers
	void incRatIntReads() { rat_int_reads++; }

	/// Increment the read count of Register Aliasing Table for FP registers
	void incRatFpReads() { rat_fp_reads++; }

	/// Increment the read count of Register Aliasing Table for XMM registers
	void incRatXmmReads() { rat_xmm_reads++; }

	/// Increment the write count of Register Aliasing Table for INT registers
	void incRatIntWrites() { rat_int_writes++; }

	/// Increment the write count of Register Aliasing Table for FP registers
	void incRatFpWrites() { rat_fp_writes++; }

	/// Increment the write count of Register Aliasing Table for XMM registers
	void incRatXmmWrites() { rat_xmm_writes++; }





	//
	// Decrement counters
	//

	/// Decrement the number of occupied physical integer registers
	void decNumIntegerRegistersOccupied() { num_integer_registers_occupied--; }

	/// Decrement the number of occupied physical float point registers
	void decNumFloatPointRegistersOccupied() { num_float_point_registers_occupied--; }

	/// Decrement the number of occupied physical xmm registers
	void decNumXmmRegistersOccupied() { num_xmm_registers_occupied--; }

	/// Decrement the read count of Register Aliasing Table for INT registers
	void decRatIntReads() { rat_int_reads--; }

	/// Decrement the read count of Register Aliasing Table for FP registers
	void decRatFpReads() { rat_fp_reads--; }

	/// Decrement the read count of Register Aliasing Table for XMM registers
	void decRatXmmReads() { rat_xmm_reads--; }

	/// Increment the write count of Register Aliasing Table for INT registers
	void decRatIntWrites() { rat_int_writes--; }

	/// Increment the write count of Register Aliasing Table for INT registers
	void decRatFpWrites() { rat_fp_writes--; }

	/// Increment the write count of Register Aliasing Table for INT registers
	void decRatXmmWrites() { rat_xmm_writes--; }




	//
	// Getters
	//

	/// Get the number of occupied physical integer registers
	int getNumIntegerRegistersOccupied() { return num_integer_registers_occupied; }

	/// Get the number of occupied physical float point registers
	int getNumFloatPointRegistersOccupied() { return num_float_point_registers_occupied; }

	/// Get the number of occupied physical xmm registers
	int getNumXmmRegistersOccupied() { return num_xmm_registers_occupied; }

	/// Get the read count of Register Aliasing Table for INT registers
	int getRatIntReads() { return rat_int_reads; }

	/// Get the read count of Register Aliasing Table for INT registers
	int getRatFpReads() { return rat_fp_reads; }

	/// Get the read count of Register Aliasing Table for INT registers
	int getRatXmmReads() { return rat_xmm_reads; }

	/// Get the write count of Register Aliasing Table for INT registers
	int getRatIntWrites() { return rat_int_writes; }

	/// Get the write count of Register Aliasing Table for INT registers
	int getRatFpWrites() { return rat_fp_writes; }

	/// Get the write count of Register Aliasing Table for INT registers
	int getRatXmmWrites() { return rat_xmm_writes; }

	/// Get cycle until which fetch is stalled
	long long getFetchStallUntil() { return fetch_stall_until; }

	/// Check whether the pipeline is empty
	bool IsPipelineEmpty();




	//
	// Fetch stage
	//

	/// Check whether or not the fecth is allowed
	bool canFetch();

	/// Fetch one x86 macro-instruction, run emulation for it, and create
	/// its corresponding set of uops, which are stored at the end of the
	/// fetch queue.
	///
	/// \param fetch_from_trace_cache
	///	Flag indicating whether the uops for the fetched macro-
	///	instruction are considered to come from the trace cache (true)
	///	or from instruction memory (false).
	///
	/// \return
	///	If any of the uops is a branch, the function returns that uop.
	///	Otherwise, it returns the first uop created, or nullptr if no
	///	uop was created.
	///
	Uop *FetchInstruction(bool fetch_from_trace_cache);

	/// Try to fetch instruction from trace cache.
	/// Return true if there was a hit and fetching succeeded.
	bool FetchFromTraceCache();

	/// Fetch stage function
	void Fetch();




	//
	// Decode stage
	//

	/// Run the decode stage for the thread
	void Decode();




	//
	// Public fields
	//

	/// Software context currently mapped to this hardware thread
	Context *context = nullptr;

	/// Memory module used as an entry in the memory hierarchy for data
	/// accesses
	mem::Module *data_module = nullptr;

	/// Memory module used as an entry in the memory hierarchy for
	/// instruction accesses
	mem::Module *instruction_module = nullptr;
};

}

#endif

