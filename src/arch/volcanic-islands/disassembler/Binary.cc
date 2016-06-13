/*
 *  Multi2Sim
 *  Copyright (C) 2016 Harrison Barclay (barclay.h@husky.neu.edu)
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

#include <cassert>
#include <cstring>
#include <lib/cpp/String.h>
#include <lib/cpp/Error.h>
#include <lib/cpp/Misc.h>

#include "Disassembler.h"
#include "Binary.h"


namespace VI
{

misc::Debug Binary::debug;


#define SI_BIN_FILE_NOT_SUPPORTED(__var) \
	throw misc::Panic(misc::fmt("Value 0x%x not supported for parameter '" \
			#__var "'", (__var)))
#define SI_BIN_FILE_NOT_SUPPORTED_NEQ(__var, __val) \
	if ((__var) != (__val)) \
		throw misc::Panic(misc::fmt("Parameter '" #__var \
				"' was expected to be 0x%x", (__val)))


struct BinaryNoteHeader
{
	Elf32_Word namesz;  // Size of the name field. Must be 8
	Elf32_Word descsz;  // Size of the data payload
	Elf32_Word type;  // Type of the payload
	char name[8];  // Note header string. Must be "ATI CAL"
};


misc::StringMap binary_user_data_map =
{
	{ "IMM_RESOURCE",                      BinaryUserDataResource },
	{ "IMM_SAMPLER",                       BinaryUserDataSampler},
	{ "IMM_CONST_BUFFER",                  BinaryUserDataConstBuffer },
	{ "IMM_VERTEX_BUFFER",                 BinaryUserDataVertexBuffer },
	{ "IMM_UAV",                           BinaryUserDataUAV },
	{ "IMM_ALU_FLOAT_CONST",               BinaryUserDataALUFloatConst},
	{ "IMM_ALU_BOOL32_CONST",              BinaryUserDataALUBool32Const },
	{ "IMM_GDS_COUNTER_RANGE",             BinaryUserDataGDSCounterRange },
	{ "IMM_GDS_MEMORY_RANGE",              BinaryUserDataGDSMemoryRange },
	{ "IMM_GWS_BASE",                      BinaryUserDataGWSBase },
	{ "IMM_WORK_ITEM_RANGE",               BinaryUserDataWorkItemRange },
	{ "IMM_WORK_GROUP_RANGE",              BinaryUserDataWorkGroupRange },
	{ "IMM_DISPATCH_ID",                   BinaryUserDataDispatchId },
	{ "IMM_SCRATCH_BUFFER",                BinaryUserDataScratchBuffer },
	{ "IMM_HEAP_BUFFER",                   BinaryUserDataHeapBuffer },
	{ "IMM_KERNEL_ARG",                    BinaryUserDataKernelArg },
	{ "IMM_CONTEXT_BASE",                  BinaryUserDataContextBase},
	{ "IMM_LDS_ESGS_SIZE",                 BinaryUserDataLDSEsgsSize },
	{ "SUB_PTR_FETCH_SHADER",              BinaryUserDataPtrFetchShader },
	{ "PTR_RESOURCE_TABLE",                BinaryUserDataPtrResourceTable },
	{ "PTR_CONST_BUFFER_TABLE",            BinaryUserDataConstBufferTable },
	{ "PTR_INTERNAL_RESOURCE_TABLE",       BinaryUserDataInteralResourceTable },
	{ "PTR_SAMPLER_TABLE",                 BinaryUserDataSamplerTable },
	{ "PTR_UAV_TABLE",                     BinaryUserDataUAVTable },
	{ "PTR_INTERNAL_GLOBAL_TABLE",         BinaryUserDataInternalGlobalTable },
	{ "PTR_VERTEX_BUFFER_TABLE",           BinaryUserDataVertexBufferTable },
	{ "PTR_SO_BUFFER_TABLE",               BinaryUserDataSoBufferTable },
	{ "PTR_EXTENDED_USER_DATA",            BinaryUserDataExtendedUserData },
	{ "PTR_INDIRECT_RESOURCE",             BinaryUserDataIndirectResource },
	{ "PTR_INDIRECT_INTERNAL_RESOURCE",    BinaryUserDataInternalResource },
	{ "PTR_INDIRECT_UAV",                  BinaryUserDataPtrIndirectUAV },
	{ "E_SC_USER_DATA_CLASS_LAST",         BinaryUserDataLast }
};


misc::StringMap binary_note_map = {
	{ "ELF_NOTE_ATI_PROGINFO", 1 },
	{ "ELF_NOTE_ATI_INPUTS", 2 },
	{ "ELF_NOTE_ATI_OUTPUTS", 3 },
	{ "ELF_NOTE_ATI_CONDOUT", 4 },
	{ "ELF_NOTE_ATI_FLOAT32CONSTS", 5 },
	{ "ELF_NOTE_ATI_INT32CONSTS", 6 },
	{ "ELF_NOTE_ATI_BOOL32CONSTS", 7 },
	{ "ELF_NOTE_ATI_EARLYEXIT", 8 },
	{ "ELF_NOTE_ATI_GLOBAL_BUFFERS", 9 },
	{ "ELF_NOTE_ATI_CONSTANT_BUFFERS", 10 },
	{ "ELF_NOTE_ATI_INPUT_SAMPLERS", 11 },
	{ "ELF_NOTE_ATI_PERSISTENT_BUFFERS", 12 },
	{ "ELF_NOTE_ATI_SCRATCH_BUFFERS", 13 },
	{ "ELF_NOTE_ATI_SUB_CONSTANT_BUFFERS", 14 },
	{ "ELF_NOTE_ATI_UAV_MAILBOX_SIZE", 15 },
	{ "ELF_NOTE_ATI_UAV", 16 },
	{ "ELF_NOTE_ATI_UAV_OP_MASK", 17 }
};

misc::StringMap binary_prog_info_map = {
	{ "mmSQ_DYN_GPR_CNTL_PS_FLUSH_REQ",          0x00002363 },
	{ "mmSQ_GPR_RESOURCE_MGMT_1",                0x00002301 },
	{ "mmSQ_GPR_RESOURCE_MGMT_3__EG",            0x00002303 },
	{ "mmSQ_THREAD_RESOURCE_MGMT__EG",           0x00002306 },
	{ "mmSQ_THREAD_RESOURCE_MGMT_2__EG",         0x00002307 },
	{ "COMPUTE_PGM_RSRC2",                       0x00002e13 },
	{ "mmSPI_THREAD_GROUPING",                   0x0000a1b2 },
	{ "mmSPI_COMPUTE_INPUT_CNTL",                0x0000a1ba },
	{ "mmSPI_GPR_MGMT",                          0x0000a1be },
	{ "mmSPI_WAVE_MGMT_1",                       0x0000a1c1 },
	{ "mmSPI_WAVE_MGMT_2",                       0x0000a1c2 },
	{ "mmSQ_LDS_ALLOC",                          0x0000a23a },
	{ "mmSQ_PGM_START_LS",                       0x0000a234 },
	{ "mmSQ_PGM_RESOURCES_LS",                   0x0000a235 },
	{ "mmSQ_PGM_RESOURCES_2_LS",                 0x0000a236 },
	{ "AMU_ABI_PS_INPUT_COUNT",                  (int) 0x80000000 },
	{ "AMU_ABI_PS_INPUT_SPARSE",                 (int) 0x80000001 },
	{ "AMU_ABI_CS_MAX_SCRATCH_REGS",             (int) 0x80000002 },
	{ "AMU_ABI_CS_NUM_SHARED_GPR_USER",          (int) 0x80000003 },
	{ "AMU_ABI_CS_NUM_SHARED_GPR_TOTAL",         (int) 0x80000004 },
   	{ "AMU_ABI_ECS_SETUP_MODE",                  (int) 0x80000005 },
	{ "AMU_ABI_NUM_THREAD_PER_GROUP",            (int) 0x80000006 },
	{ "AMU_ABI_TOTAL_NUM_THREAD_GROUP",          (int) 0x80000007 },
	{ "AMU_ABI_MAX_WAVEFRONT_PER_SIMD",          (int) 0x80000009 },
	{ "AMU_ABI_NUM_WAVEFRONT_PER_SIMD",          (int) 0x8000000a },
	{ "AMU_ABI_IS_MAX_NUM_WAVE_PER_SIMD",        (int) 0x8000000b },
	{ "AMU_ABI_SET_BUFFER_FOR_NUM_GROUP",        (int) 0x8000000c },
	{ "AMU_ABI_VS_OUTPUT_COUNT",                 (int) 0x8000000d },
	{ "AMU_ABI_VS_OUTPUT_SPARSE",                (int) 0x8000000e },
	{ "AMU_ABI_VS_INPUT_COUNT",                  (int) 0x8000000f },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER0",  (int) 0x80000010 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER1",  (int) 0x80000011 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER2",  (int) 0x80000012 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER3",  (int) 0x80000013 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER4",  (int) 0x80000014 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER5",  (int) 0x80000015 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER6",  (int) 0x80000016 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER7",  (int) 0x80000017 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER8",  (int) 0x80000018 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER9",  (int) 0x80000019 },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER10", (int) 0x8000001a },
	{ "AMU_ABI_SET_BUFFER_FOR_UAV_RET_BUFFER11", (int) 0x8000001b },
	{ "AMU_ABI_NUM_THREAD_PER_GROUP_X",          (int) 0x8000001c },
	{ "AMU_ABI_NUM_THREAD_PER_GROUP_Y",          (int) 0x8000001d },
	{ "AMU_ABI_NUM_THREAD_PER_GROUP_Z",          (int) 0x8000001e },
	{ "AMU_ABI_NUM_THREAD_PER_GROUP_X",          (int) 0x8000001c },
	{ "AMU_ABI_NUM_THREAD_PER_GROUP_Y",          (int) 0x8000001d },
	{ "AMU_ABI_NUM_THREAD_PER_GROUP_Z",          (int) 0x8000001e },
	{ "AMU_ABI_TOTAL_NUM_THREAD_GROUP",          (int) 0x80000007 },
	{ "AMU_ABI_NUM_WAVEFRONT_PER_SIMD",          (int) 0x8000000a },
	{ "AMU_ABI_IS_MAX_NUM_WAVE_PER_SIMD",        (int) 0x8000000b },
	{ "AMU_ABI_SET_BUFFER_FOR_NUM_GROUP",        (int) 0x8000000c },
	{ "AMU_ABI_RAT_OP_IS_USED",                  (int) 0x8000001f },
	{ "AMU_ABI_RAT_ATOMIC_OP_IS_USED",           (int) 0x80000020 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER",            (int) 0x80000021 },
	{ "AMU_ABI_NUM_GLOBAL_UAV",                  (int) 0x80000022 },
	{ "AMU_ABI_NUM_GLOBAL_RETURN_BUFFER",        (int) 0x80000023 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_SIZE",       (int) 0x80000024 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_SIZE_SHORT", (int) 0x80000025 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_SIZE_BYTE",  (int) 0x80000026 },
	{ "AMU_ABI_EXTENDED_CACHING",                (int) 0x80000027 },
	{ "AMU_ABI_OFFSET_TO_UAV0",                  (int) 0x80000030 },
	{ "AMU_ABI_OFFSET_TO_UAV1",                  (int) 0x80000031 },
	{ "AMU_ABI_OFFSET_TO_UAV2",                  (int) 0x80000032 },
	{ "AMU_ABI_OFFSET_TO_UAV3",                  (int) 0x80000033 },
	{ "AMU_ABI_OFFSET_TO_UAV4",                  (int) 0x80000034 },
	{ "AMU_ABI_OFFSET_TO_UAV5",                  (int) 0x80000035 },
	{ "AMU_ABI_OFFSET_TO_UAV6",                  (int) 0x80000036 },
	{ "AMU_ABI_OFFSET_TO_UAV7",                  (int) 0x80000037 },
	{ "AMU_ABI_OFFSET_TO_UAV8",                  (int) 0x80000038 },
	{ "AMU_ABI_OFFSET_TO_UAV9",                  (int) 0x80000039 },
	{ "AMU_ABI_OFFSET_TO_UAV10",                 (int) 0x8000003a },
	{ "AMU_ABI_OFFSET_TO_UAV11",                 (int) 0x8000003b },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV0",       (int) 0x80000040 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV1",       (int) 0x80000041 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV2",       (int) 0x80000042 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV3",       (int) 0x80000043 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV4",       (int) 0x80000044 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV5",       (int) 0x80000045 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV6",       (int) 0x80000046 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV7",       (int) 0x80000047 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV8",       (int) 0x80000048 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV9",       (int) 0x80000049 },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV10",      (int) 0x8000004a },
	{ "AMU_ABI_UNCACHED_FETCH_CONST_UAV11",      (int) 0x8000004b },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV0",         (int) 0x80000050 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV1",         (int) 0x80000051 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV2",         (int) 0x80000052 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV3",         (int) 0x80000053 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV4",         (int) 0x80000054 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV5",         (int) 0x80000055 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV6",         (int) 0x80000056 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV7",         (int) 0x80000057 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV8",         (int) 0x80000058 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV9",         (int) 0x80000059 },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV10",        (int) 0x8000005a },
	{ "AMU_ABI_CACHED_FETCH_CONST_UAV11",        (int) 0x8000005b },
	{ "AMU_ABI_GLOBAL_RETURN_FETCH_CONST0",      (int) 0x80000060 },
	{ "AMU_ABI_GLOBAL_RETURN_FETCH_CONST1",      (int) 0x80000061 },
	{ "AMU_ABI_GLOBAL_RETURN_FETCH_CONST2",      (int) 0x80000062 },
	{ "AMU_ABI_GLOBAL_RETURN_FETCH_CONST3",      (int) 0x80000063 },
	{ "AMU_ABI_GLOBAL_RETURN_FETCH_CONST4",      (int) 0x80000064 },
	{ "AMU_ABI_GLOBAL_RETURN_FETCH_CONST5",      (int) 0x80000065 },
	{ "AMU_ABI_GLOBAL_RETURN_FETCH_CONST6",      (int) 0x80000066 },
	{ "AMU_ABI_GLOBAL_RETURN_FETCH_CONST7",      (int) 0x80000067 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_TYPE0",      (int) 0x80000070 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_TYPE1",      (int) 0x80000071 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_TYPE2",      (int) 0x80000072 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_TYPE3",      (int) 0x80000073 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_TYPE4",      (int) 0x80000074 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_TYPE5",      (int) 0x80000075 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_TYPE6",      (int) 0x80000076 },
	{ "AMU_ABI_GLOBAL_RETURN_BUFFER_TYPE7",      (int) 0x80000077 },
	{ "AMU_ABI_WAVEFRONT_SIZE",                  (int) 0x80000078 },
	{ "AMU_ABI_NUM_GPR_AVAIL",                   (int) 0x80000079 },
	{ "AMU_ABI_NUM_GPR_USED",                    (int) 0x80000080 },
	{ "AMU_ABI_LDS_SIZE_AVAIL",                  (int) 0x80000081 },
	{ "AMU_ABI_LDS_SIZE_USED",                   (int) 0x80000082 },
	{ "AMU_ABI_STACK_SIZE_AVAIL",                (int) 0x80000083 },
	{ "AMU_ABI_STACK_SIZE_USED",                 (int) 0x80000084 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV0",       (int) 0x80000090 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV1",       (int) 0x80000091 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV2",       (int) 0x80000092 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV3",       (int) 0x80000093 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV4",       (int) 0x80000094 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV5",       (int) 0x80000095 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV6",       (int) 0x80000096 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV7",       (int) 0x80000097 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV8",       (int) 0x80000098 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV9",       (int) 0x80000099 },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV10",      (int) 0x8000009a },
	{ "AMU_ABI_CACHED_FETCH_CONST64_UAV11",      (int) 0x8000009b },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV0",      (int) 0x800000a0 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV1",      (int) 0x800000a1 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV2",      (int) 0x800000a2 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV3",      (int) 0x800000a3 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV4",      (int) 0x800000a4 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV5",      (int) 0x800000a5 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV6",      (int) 0x800000a6 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV7",      (int) 0x800000a7 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV8",      (int) 0x800000a8 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV9",      (int) 0x800000a9 },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV10",     (int) 0x800000aa },
	{ "AMU_ABI_CACHED_FETCH_CONST128_UAV11",     (int) 0x800000ab }
};


/* Read note at the current position of the stream. Argument 'buffer' takes a
 * base pointer to the beginning of the string buffer content. */
void Binary::ReadNote(BinaryDictEntry *dict_entry, std::istringstream& ss,
		const char *buffer)
{
	BinaryNoteHeader *header = (BinaryNoteHeader *) (buffer + ss.tellg());
	ss.seekg(sizeof(BinaryNoteHeader), std::ios_base::cur);
	if (!ss)
		throw Disassembler::Error(name + ": Cannot decode note header");
	
	// Read note description (payload)
	const char *desc = buffer + ss.tellg();
	ss.seekg(header->descsz, std::ios_base::cur);
	if (!ss)
		throw Disassembler::Error(name + ": Cannot decode note description");

	// Debug
	const char *note_str = binary_note_map.MapValue(header->type);
	debug << "  note: type = " << header->type << " ("
			<< note_str << "), descsz = "
			<< header->descsz << '\n';
		
	// Analyze note

	if(header->type == 1)  // ELF_NOTE_ATI_PROGINFO
	{
		int prog_info_count;
		BinaryNoteProgInfoEntry *prog_info_entry;
		int i;

		// Get number of entries
		assert(header->descsz % sizeof(BinaryNoteProgInfoEntry) == 0);
		prog_info_count = header->descsz / sizeof(BinaryNoteProgInfoEntry);
		debug << "\tnote with device configuration unique to the"
				<< " program (" << prog_info_count << " entries)\n";

		// Decode entries
		for (i = 0; i < prog_info_count; i++)
		{
			prog_info_entry = (BinaryNoteProgInfoEntry *)
					(desc + i * sizeof(BinaryNoteProgInfoEntry));
			debug << "\tprog_info_entry: addr = 0x" << std::hex <<
					prog_info_entry->address << std::dec << " (" <<
					binary_prog_info_map.MapValue(prog_info_entry->address)
					<< "), value = " << prog_info_entry->value << '\n';

			// Analyze entry
			switch (prog_info_entry->address)
			{

			case 0x00002e13:  // COMPUTE_PGM_RSRC2
			dict_entry->compute_pgm_rsrc2 = 
				(BinaryComputePgmRsrc2*) &prog_info_entry->value;
			break;

			case 0x80000082:  // AMU_ABI_LDS_SIZE_USED
				dict_entry->lds_size = prog_info_entry->value;
				break;

			case 0x80000084:  // AMU_ABI_STACK_SIZE_USED
				dict_entry->stack_size = prog_info_entry->value;
				break;

			case 0x80001000:  // AMU_ABI_USER_ELEMENT_COUNT
				dict_entry->num_user_elements = prog_info_entry->value;
				i++;

				// Analyze user elements
				for(unsigned int j = 0; j < 4 * dict_entry->num_user_elements; j++)
				{
					prog_info_entry = (BinaryNoteProgInfoEntry *)
							(desc + i * sizeof(BinaryNoteProgInfoEntry));

					debug << "\tprog_info_entry: addr = 0x" << std::hex <<
							prog_info_entry->address << std::dec << " ("
							<< binary_prog_info_map.MapValue(prog_info_entry->address)
							<< "), value = " << prog_info_entry->value << '\n';
					switch(j % 4)
					{
					case 0:
						dict_entry->user_elements[j / 4].dataClass = prog_info_entry->value;
						break;
					case 1:
						dict_entry->user_elements[j / 4].apiSlot = prog_info_entry->value;
						break;
					case 2:
						dict_entry->user_elements[j / 4].startUserReg = prog_info_entry->value;
						break;
					case 3:
						dict_entry->user_elements[j / 4].userRegCount = prog_info_entry->value;
						break;
					}

					i++;
				}
				break;

			case 0x80001041:  // NUM VGPRS
				dict_entry->num_vgpr = prog_info_entry->value;
				break;

			case 0x80001042:  // NUM SGPRS
				dict_entry->num_sgpr = prog_info_entry->value;
				break;

			}


		}
	}
}


// Decode notes in the PT_NOTE segment of the given encoding dictionary entry
void Binary::ReadNotes(BinaryDictEntry *dict_entry)
{
	// Get buffer and set position
	ELFReader::ProgramHeader *ph = dict_entry->pt_note_segment;
	std::istringstream ss;
	ph->getStream(ss);
	const char *buffer = ph->getBuffer();

	// Decode notes
	debug << "\nReading notes in PT_NOTE segment (enc. dict. for machine = 0x"
			<< std::hex << dict_entry->header->d_machine << std::dec << ")\n";
	while (ss.tellg() < ph->getSize())
		ReadNote(dict_entry, ss, buffer);
}


void Binary::ReadDictionary()
{
	BinaryDictEntry *dict_entry;
	int num_dict_entries;

	// ELF header
	debug << "\n**\n"
			<< "** Parsing AMD Binary (Internal ELF file)\n"
			<< "** " << name << "\n"
			<< "**\n\n";
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(getIdent()[EI_CLASS], ELFCLASS32);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(getIdent()[EI_DATA], ELFDATA2LSB);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(getIdent()[EI_OSABI], 0x64);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(getIdent()[EI_ABIVERSION], 1);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(getType(), ET_EXEC);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(getMachine(), 0x7d);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(getEntry(), 0);
	
	// Look for encoding dictionary (program header with type 'PT_LOPROC+2')
	ELFReader::ProgramHeader *ph = NULL;
	for (int i = 0; i < getNumProgramHeaders(); i++)
	{
		ph = getProgramHeader(i);
		if (ph->getType() == PT_LOPROC + 2)
			break;
		ph = NULL;
	}
	if (!ph)
		throw Disassembler::Error(name + ": No encoding dictionary");
	debug << "Encoding dictionary found in program header "
			<< ph->getIndex() << "\n";
	
	// Parse encoding dictionary
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(ph->getVaddr(), 0);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(ph->getPaddr(), 0);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(ph->getMemsz(), 0);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(ph->getFlags(), 0);
	SI_BIN_FILE_NOT_SUPPORTED_NEQ(ph->getAlign(), 0);
	assert(ph->getFilesz() % sizeof(BinaryDictHeader) == 0);
	num_dict_entries = ph->getFilesz() / sizeof(BinaryDictHeader);
	debug << "  -> " << num_dict_entries << " entries\n\n";

	// Read encoding dictionary entries
	std::istringstream ss;
	ph->getStream(ss);
	for (int i = 0; i < num_dict_entries; i++)
	{
		dict_entry = new BinaryDictEntry();
		dict.push_back(dict_entry);

		// Read header
		dict_entry->header = (BinaryDictHeader *) (ph->getBuffer() + ss.tellg());
		ss.seekg(sizeof(BinaryDictHeader), std::ios_base::cur);
		
		// FIXME
		// Store encoding dictionary entry
		if (dict_entry->header->d_machine == 0)
		{
			debug << "machine = " << dict_entry->header->d_machine
					<< " (always 0?)\n";
			vi_dict_entry = dict_entry;
		}
		vi_dict_entry = dict_entry;
		
	}

	// Debug
	debug << "Encoding dictionaries:\n";
	for (unsigned i = 0; i < dict.size(); i++)
	{
		BinaryDictEntry *entry = dict[i];
		BinaryDictHeader *header = entry->header;
		debug << "[" << i << "] "
				<< "machine = " << header->d_machine
				<< "type = " << header->d_type << ", "
				<< "offset = " << std::hex << header->d_offset
				<< std::dec << ", "
				<< "size = " << header->d_size << ", "
				<< "flags = " << std::hex << header->d_flags
				<< std::dec << '\n';
	}
}


void Binary::ReadSegments()
{
	debug << "\nReading PT_NOTE and PT_LOAD segments:\n";
	for (unsigned i = 0; i < dict.size(); i++)
	{
		// Get encoding dictionary entry
		BinaryDictEntry *dict_entry = dict[i];
		for (int j = 0; j < getNumProgramHeaders(); j++)
		{
			// Get program header. If not in encoding dictionary segment, skip.
			ELFReader::ProgramHeader *ph = getProgramHeader(j);
			if (ph->getOffset() < dict_entry->header->d_offset ||
					ph->getOffset() >= dict_entry->header->d_offset +
					dict_entry->header->d_size)
				continue;
			assert(ph->getOffset() + ph->getFilesz() <=
					dict_entry->header->d_offset +
					dict_entry->header->d_size);

			// Segment PT_NOTE
			if (ph->getType() == PT_NOTE)
			{
				if (dict_entry->pt_note_segment)
					throw Disassembler::Error("More than one "
							"PT_NOTE segment");
				dict_entry->pt_note_segment = ph;
			}

			// Segment PT_LOAD
			if (ph->getType() == PT_LOAD)
			{
				if (dict_entry->pt_load_segment)
					throw Disassembler::Error("More than one "
							"PT_LOAD segment");
				dict_entry->pt_load_segment = ph;
			}
		}

		// Check that both PT_NOTE and PT_LOAD segments were found
		if (!dict_entry->pt_note_segment)
			throw Disassembler::Error("No PT_NOTE segment");
		if (!dict_entry->pt_load_segment)
			throw Disassembler::Error("No PT_LOAD segment");
		debug << "  Dict. entry " << i
				<< ": PT_NOTE segment: "
				<< "offset = 0x" << std::hex
				<< dict_entry->pt_note_segment->getOffset()
				<< std::dec << ", "
				<< "size = "
				<< dict_entry->pt_note_segment->getSize()
				<< '\n';
		debug << "  Dict. entry " << i
				<< ": PT_LOAD segment: "
				<< "offset = 0x" << std::hex <<
				dict_entry->pt_load_segment->getOffset()
				<< std::dec << ", "
				<< "size = "
				<< dict_entry->pt_load_segment->getSize()
				<< '\n';
	}
}


void Binary::ReadSections()
{
	for (auto &dict_entry : dict)
	{
		// Get encoding dictionary entry
		ELFReader::ProgramHeader *load_segment = dict_entry->pt_load_segment;
		ELFReader::ProgramHeader *note_segment = dict_entry->pt_note_segment;
		assert(load_segment);
		assert(note_segment);

		for (int i = 0; i < getNumSections(); i++)
		{
			// Get section. If not in PT_LOAD segment, skip.
			ELFReader::Section *section = getSection(i);
			if (section->getOffset() < load_segment->getOffset() ||
					section->getOffset() >= load_segment->getOffset()
					+ load_segment->getSize())
				continue;
			assert(section->getOffset() + section->getSize() <=
					load_segment->getOffset() +
					load_segment->getSize());

			// Process section
			if (section->getName() == ".text")
			{
				if (dict_entry->text_section)
					throw Disassembler::Error("Duplicated .text "
							"section");
				dict_entry->text_section = section;
			}
			else if (section->getName() == ".data")
			{
				if (dict_entry->data_section)
					throw Disassembler::Error("Duplicated .data "
							"section");
				dict_entry->data_section = section;
				if (section->getSize() != 4736)
					throw Disassembler::Error("Section .data is not "
							"4736 bytes");
				dict_entry->consts = (BinaryDictConsts *) section->getBuffer();
			}
			else if (section->getName() == ".symtab")
			{
				if (dict_entry->symtab_section)
					throw Disassembler::Error("Duplicated .symtab "
							"section");
				dict_entry->symtab_section = section;
			}
			else if (section->getName() == ".strtab")
			{
				if (dict_entry->strtab_section)
					throw Disassembler::Error("Duplicated .strtab "
							"section");
				dict_entry->strtab_section = section;
			}
			else
			{
				throw Disassembler::Error("Unrecognized section: " +
						section->getName());
			}
		}

		// Check that all sections where read
		if (!dict_entry->text_section
				|| !dict_entry->data_section
				|| !dict_entry->symtab_section
				|| !dict_entry->strtab_section)
			throw Disassembler::Error("One of these sections not found: "
					".text .data .symtab .strtab");
	}
}


Binary::Binary(const char *buffer, unsigned int size, std::string name)
		: ELFReader::File(buffer, size)
{
	this->name = name;

	/* Read encoding dictionary.
	 * Check that a VI dictionary entry is present */
	vi_dict_entry = NULL;
	ReadDictionary();
	if (!vi_dict_entry)
		throw Disassembler::Error(name +
	": No encoding dictionary entry for Volcanic Islands.\n\n"
	"\tThe OpenCL kernel binary that your application is trying to load "
	"does not contain Volcanic Islands assembly code. Please make " 
	"sure that a Tonga device is selected when compiling the OpenCL "
	"kernel source. In some cases, even a proper selection of this "
	"architecture causes Volcanic Islands assembly not to be included "
	"if the APP SDK is not correctly installed when compiling your "
	"own kernel sources.");
	
	ReadSegments();
	ReadSections();

	ReadNotes(vi_dict_entry);
}


Binary::~Binary()
{
	for (auto &dict_entry : dict)
		delete dict_entry;
}


}  // namespace SI

