#include <assert.h>
#include <renamer.h>
#include <bits/stdc++.h>

/////////////////////////////////////////////////////////////////////
// This is the constructor function.
// When a renamer object is instantiated, the caller indicates:
// 1. The number of logical registers (e.g., 32).
// 2. The number of physical registers (e.g., 128).
// 3. The maximum number of unresolved branches.
//    Requirement: 1 <= n_branches <= 64.
// 4. The maximum number of active instructions (Active List size).
//
// Tips:
//
// Assert the number of physical registers > number logical registers.
// Assert 1 <= n_branches <= 64.
// Assert n_active > 0.
// Then, allocate space for the primary data structures.
// Then, initialize the data structures based on the knowledge
// that the pipeline is intially empty (no in-flight instructions yet).
/////////////////////////////////////////////////////////////////////
renamer::renamer(uint64_t n_log_regs,uint64_t n_phys_regs,uint64_t n_branches,uint64_t n_active){
    // Save the number of branches
    this->n_log_regs=n_log_regs;
	this->n_phys_regs=n_phys_regs;
	this->n_branches=n_branches;
	this->n_active=n_active;
    // Check assertions
    assert(1 <= n_branches <= 64);
    assert(n_active > 0);
    assert(n_phys_regs > n_log_regs);
    //  Initialzie the GB
    GBM = 0;
    RMT = new uint64_t[n_log_regs];
    // Initialize RMT
    for (uint64_t idx =  0; idx<n_log_regs;idx++){
        RMT[idx] = idx;
    }
    AMT = new uint64_t[n_log_regs];
    for (uint64_t idx =  0; idx<n_log_regs;idx++){
        AMT[idx] = idx;
    }
    // Initialize AMT
    FL = new free_list(n_phys_regs-n_log_regs);
    for (uint64_t i =  n_log_regs; i<n_phys_regs;i++){
        FL->push(i);
    }
    // Initialzie free list
    AL = new active_list(n_active);
    // init the active list
    PRF = new uint64_t[n_phys_regs];
    branch_checkpoints = new checkpoint_entry[n_branches];
    prf_ready_bit_array = new bool[n_phys_regs];
    for (uint64_t i = 0; i < n_phys_regs;i++){
        prf_ready_bit_array[i] = true;
    }
}
    
/////////////////////////////////////////////////////////////////////
// This is the destructor, used to clean up memory space and
// other things when simulation is done.
// I typically don't use a destructor; you have the option to keep
// this function empty.
/////////////////////////////////////////////////////////////////////
renamer::~renamer(){
    delete [] RMT;
    delete [] AMT;
    delete FL;
    delete AL;
}

//////////////////////////////////////////
// Functions related to Rename Stage.   //
//////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// The Rename Stage must stall if there aren't enough free physical
// registers available for renaming all logical destination registers
// in the current rename bundle.
//
// Inputs:
// 1. bundle_dst: number of logical destination registers in
//    current rename bundle
//
// Return value:
// Return "true" (stall) if there aren't enough free physical
// registers to allocate to all of the logical destination registers
// in the current rename bundle.
/////////////////////////////////////////////////////////////////////
bool renamer::stall_reg(uint64_t bundle_dst){
    printf("FL SIZE: %d | bundle: %d\n", FL->size(),bundle_dst);
    printf("Tail: %d | Head : %d\n",FL->tail, FL->head);
    if ( bundle_dst > FL->size()) return true;
    else return false;
}

/////////////////////////////////////////////////////////////////////
// The Rename Stage must stall if there aren't enough free
// checkpoints for all branches in the current rename bundle.
//
// Inputs:
// 1. bundle_branch: number of branches in current rename bundle
//
// Return value:
// Return "true" (stall) if there aren't enough free checkpoints
// for all branches in the current rename bundle.
/////////////////////////////////////////////////////////////////////
bool renamer::stall_branch(uint64_t bundle_branch){
    printf("GBM : %x\n", GBM);
    if ( (__builtin_popcount(GBM) + bundle_branch ) > n_branches ) return true;
    else return false;
}

/////////////////////////////////////////////////////////////////////
// This function is used to get the branch mask for an instruction.
/////////////////////////////////////////////////////////////////////
uint64_t renamer::get_branch_mask(){
    return GBM;
}

/////////////////////////////////////////////////////////////////////
// This function is used to rename a single source register.
//
// Inputs:
// 1. log_reg: the logical register to rename
//
// Return value: physical register name
/////////////////////////////////////////////////////////////////////
uint64_t renamer::rename_rsrc(uint64_t log_reg){
    return RMT[log_reg];
}

/////////////////////////////////////////////////////////////////////
// This function is used to rename a single destination register.
//
// Inputs:
// 1. log_reg: the logical register to rename
//
// Return value: physical register name
/////////////////////////////////////////////////////////////////////
uint64_t renamer::rename_rdst(uint64_t log_reg){
    RMT[log_reg] = FL->pop();
    return RMT[log_reg];
}

/////////////////////////////////////////////////////////////////////
// This function creates a new branch checkpoint.
//
// Inputs: none.
//
// Output:
// 1. The function returns the branch's ID. When the branch resolves,
//    its ID is passed back to the renamer via "resolve()" below.
//
// Tips:
//
// Allocating resources for the branch (a GBM bit and a checkpoint):
// * Find a free bit -- i.e., a '0' bit -- in the GBM. Assert that
//   a free bit exists: it is the user's responsibility to avoid
//   a structural hazard by calling stall_branch() in advance.
// * Set the bit to '1' since it is now in use by the new branch.
// * The position of this bit in the GBM is the branch's ID.
// * Use the branch checkpoint that corresponds to this bit.
// 
// The branch checkpoint should contain the following:
// 1. Shadow Map Table (checkpointed Rename Map Table)
// 2. checkpointed Free List head pointer and its phase bit
// 3. checkpointed GBM
/////////////////////////////////////////////////////////////////////
uint64_t renamer::checkpoint(){
    //printf("Starting checkpoint\n");
    //assert(GBM != 0xffffffffffffffff);   *****************************NEED TO REPLACE THIS GOOFY***********************LOLOLOOLOLOL
    // Find the open bit
    uint64_t mask = 0x1;
    uint64_t index = 0;  
    while (GBM & mask){
        index++;
        mask = mask << 1;
    }
    GBM |= mask;
    // Back everything up you dummy :))))))
    checkpoint_entry & dummy = branch_checkpoints[index];
    dummy.shadow_map_table = RMT;
    assert(&dummy.shadow_map_table != &RMT);
    dummy.head = FL->head;
    dummy.tail_phase = FL->tail_phase;
    dummy.head_phase = FL->head_phase;
    dummy.GBM = GBM;
   //printf("Starting checkpoint\n");
    return index;
}

//////////////////////////////////////////
// Functions related to Dispatch Stage. //
//////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// The Dispatch Stage must stall if there are not enough free
// entries in the Active List for all instructions in the current
// dispatch bundle.
//
// Inputs:
// 1. bundle_inst: number of instructions in current dispatch bundle
//
// Return value:
// Return "true" (stall) if the Active List does not have enough
// space for all instructions in the dispatch bundle.
/////////////////////////////////////////////////////////////////////
bool renamer::stall_dispatch(uint64_t bundle_inst){
    printf("AL Full: %d\n", AL->size());
    if (AL->size() + bundle_inst > n_active) return true;
    else return false;
}

/////////////////////////////////////////////////////////////////////
// This function dispatches a single instruction into the Active
// List.
//
// Inputs:
// 1. dest_valid: If 'true', the instr. has a destination register,
//    otherwise it does not. If it does not, then the log_reg and
//    phys_reg inputs should be ignored.
// 2. log_reg: Logical register number of the instruction's
//    destination.
// 3. phys_reg: Physical register number of the instruction's
//    destination.
// 4. load: If 'true', the instr. is a load, otherwise it isn't.
// 5. store: If 'true', the instr. is a store, otherwise it isn't.
// 6. branch: If 'true', the instr. is a branch, otherwise it isn't.
// 7. amo: If 'true', this is an atomic memory operation.
// 8. csr: If 'true', this is a system instruction.
// 9. PC: Program counter of the instruction.
//
// Return value:
// Return the instruction's index in the Active List.
//
// Tips:
//
// Before dispatching the instruction into the Active List, assert
// that the Active List isn't full: it is the user's responsibility
// to avoid a structural hazard by calling stall_dispatch()
// in advance.
/////////////////////////////////////////////////////////////////////
uint64_t renamer::dispatch_inst(bool dest_valid,
                        uint64_t log_reg,
                        uint64_t phys_reg,
                        bool load,
                        bool store,
                        bool branch,
                        bool amo,
                        bool csr,
                        uint64_t PC){
    
    //printf("Starting dispatch\n");
    // Create new entry
    active_list_entry * entry = new active_list_entry(dest_valid,log_reg,phys_reg,load,store,branch,amo,csr,PC);
    assert(!AL->full());
    uint64_t index = AL->tail;
    AL->push(entry);
    //printf("Finishing dispatch\n");
    //printf("Index: %d", index);
    return index;
}

/////////////////////////////////////////////////////////////////////
// Test the ready bit of the indicated physical register.
// Returns 'true' if ready.
/////////////////////////////////////////////////////////////////////
bool renamer::is_ready(uint64_t phys_reg){
    return (prf_ready_bit_array[phys_reg]);
}

/////////////////////////////////////////////////////////////////////
// Clear the ready bit of the indicated physical register.
/////////////////////////////////////////////////////////////////////
void renamer::clear_ready(uint64_t phys_reg){
    prf_ready_bit_array[phys_reg] = false;
}


//////////////////////////////////////////
// Functions related to the Reg. Read   //
// and Execute Stages.                  //
//////////////////////////////////////////
uint64_t renamer::read(uint64_t phys_reg){
    return PRF[phys_reg];
}

/////////////////////////////////////////////////////////////////////
// Set the ready bit of the indicated physical register.
/////////////////////////////////////////////////////////////////////
void renamer::set_ready(uint64_t phys_reg){
    prf_ready_bit_array[phys_reg] = true;
}


//////////////////////////////////////////
// Functions related to Writeback Stage.//
//////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Write a value into the indicated physical register.
/////////////////////////////////////////////////////////////////////
void renamer::write(uint64_t phys_reg, uint64_t value){
    PRF[phys_reg] = value;
}

/////////////////////////////////////////////////////////////////////
// Set the completed bit of the indicated entry in the Active List.
/////////////////////////////////////////////////////////////////////
void renamer::set_complete(uint64_t AL_index){
    //printf("AL Index: %d %d\n",AL_index,n_active);
    active_list_entry* entry = AL->at(AL_index);
    entry->completed = true;
}

/////////////////////////////////////////////////////////////////////
// This function is for handling branch resolution.
//
// Inputs:
// 1. AL_index: Index of the branch in the Active List.
// 2. branch_ID: This uniquely identifies the branch and the
//    checkpoint in question.  It was originally provided
//    by the checkpoint function.
// 3. correct: 'true' indicates the branch was correctly
//    predicted, 'false' indicates it was mispredicted
//    and recovery is required.
//
// Outputs: none.
//
// Tips:
//
// While recovery is not needed in the case of a correct branch,
// some actions are still required with respect to the GBM and
// all checkpointed GBMs:
// * Remember to clear the branch's bit in the GBM.
// * Remember to clear the branch's bit in all checkpointed GBMs.
//
// In the case of a misprediction:
// * Restore the GBM from the branch's checkpoint. Also make sure the
//   mispredicted branch's bit is cleared in the restored GBM,
//   since it is now resolved and its bit and checkpoint are freed.
// * You don't have to worry about explicitly freeing the GBM bits
//   and checkpoints of branches that are after the mispredicted
//   branch in program order. The mere act of restoring the GBM
//   from the checkpoint achieves this feat.
// * Restore the RMT using the branch's checkpoint.
// * Restore the Free List head pointer and its phase bit,
//   using the branch's checkpoint.
// * Restore the Active List tail pointer and its phase bit
//   corresponding to the entry after the branch's entry.
//   Hints:
//   You can infer the restored tail pointer from the branch's
//   AL_index. You can infer the restored phase bit, using
//   the phase bit of the Active List head pointer, where
//   the restored Active List tail pointer is with respect to
//   the Active List head pointer, and the knowledge that the
//   Active List can't be empty at this moment (because the
//   mispredicted branch is still in the Active List).
// * Do NOT set the branch misprediction bit in the Active List.
//   (Doing so would cause a second, full squash when the branch
//   reaches the head of the Active List. We don’t want or need
//   that because we immediately recover within this function.)
/////////////////////////////////////////////////////////////////////
void renamer::resolve(uint64_t AL_index, uint64_t branch_ID, bool correct){
    uint64_t idx = 0;
    uint64_t mask = 1;
    uint64_t branch_bit = (0x1 << branch_ID);
    //printf("Starting resolve\n");
    if (correct){
        // Clear branch bit
        GBM &= ~branch_bit;
        // clear in all checkpointed GBM
        while (idx != n_branches){
            if (GBM & mask){
                branch_checkpoints[idx].GBM &= ~branch_bit;
            }
            mask <<= 1;
            idx++;
        }   
    }
    // Restoration case
    else {
        GBM = branch_checkpoints[branch_ID].GBM;
        RMT = branch_checkpoints[branch_ID].shadow_map_table;
        FL->head = branch_checkpoints[branch_ID].head;
        FL->head_phase = branch_checkpoints[branch_ID].head_phase;
        FL->tail_phase = branch_checkpoints[branch_ID].tail_phase; 

        // Clear the GBM
        GBM &= ~branch_bit;
        // Roll back the active list tail
        AL->setTail(AL_index+1);
    }
    //printf("Finishing resolve\n");
}

//////////////////////////////////////////
// Functions related to Retire Stage.   //
//////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// This function allows the caller to examine the instruction at the head
// of the Active List.
//
// Input arguments: none.
//
// Return value:
// * Return "true" if the Active List is NOT empty, i.e., there
//   is an instruction at the head of the Active List.
// * Return "false" if the Active List is empty, i.e., there is
//   no instruction at the head of the Active List.
//
// Output arguments:
// Simply return the following contents of the head entry of
// the Active List.  These are don't-cares if the Active List
// is empty (you may either return the contents of the head
// entry anyway, or not set these at all).
// * completed bit
// * exception bit
// * load violation bit
// * branch misprediction bit
// * value misprediction bit
// * load flag (indicates whether or not the instr. is a load)
// * store flag (indicates whether or not the instr. is a store)
// * branch flag (indicates whether or not the instr. is a branch)
// * amo flag (whether or not instr. is an atomic memory operation)
// * csr flag (whether or not instr. is a system instruction)
// * program counter of the instruction
/////////////////////////////////////////////////////////////////////
bool renamer::precommit(bool &completed,
                    bool &exception, bool &load_viol, bool &br_misp, bool &val_misp,
                bool &load, bool &store, bool &branch, bool &amo, bool &csr,
            uint64_t &PC){
    //printf("Starting precommit\n");
    if (!AL->empty()){
        active_list_entry * entry = AL->at(AL->head);
        //printf("Completed %x\n",entry->load_violation);
        completed = entry->completed;
        exception = entry->exception;
        load_viol = entry->load_violation;

        br_misp = entry->branch_mispred;
        val_misp = entry->val_mispred;
        load = entry->load_flag;

        store = entry->store_flag;
        branch = entry->branch_flag;
        amo = entry->amo_flag;
        csr = entry->csr_flag;
        PC = entry->pc;
        //printf("Finishing precommit\n");
        return true;
    }
    else {
       // printf("Finishing precommit\n");
        return false;
    }    
}

/////////////////////////////////////////////////////////////////////
// This function commits the instruction at the head of the Active List.
//
// Tip (optional but helps catch bugs):
// Before committing the head instruction, assert that it is valid to
// do so (use assert() from standard library). Specifically, assert
// that all of the following are true:
// - there is a head instruction (the active list isn't empty)
// - the head instruction is completed
// - the head instruction is not marked as an exception
// - the head instruction is not marked as a load violation
// It is the caller's (pipeline's) duty to ensure that it is valid
// to commit the head instruction BEFORE calling this function
// (by examining the flags returned by "precommit()" above).
// This is why you should assert() that it is valid to commit the
// head instruction and otherwise cause the simulator to exit.
/////////////////////////////////////////////////////////////////////
void renamer::commit(){
    //printf("Starting commit\n");
    assert(!AL->empty());
    active_list_entry * head = AL->at(AL->head);
    assert(head->completed);
    assert(!head->exception);
    assert(!head->load_violation);
    head = AL->pop();
    if (head->dest_flag) FL->push(head->logical_reg_num);
    printf("log: %d phys: %d\n is dest? %x\n",head->logical_reg_num,head->physical_reg_num,head->dest_flag);
    AMT[head->logical_reg_num] = head->physical_reg_num;
    //printf("Finished commit\n");
}

//////////////////////////////////////////////////////////////////////
// Squash the renamer class.
//
// Squash all instructions in the Active List and think about which
// sructures in your renamer class need to be restored, and how.
//
// After this function is called, the renamer should be rolled-back
// to the committed state of the machine and all renamer state
// should be consistent with an empty pipeline.
/////////////////////////////////////////////////////////////////////
void renamer::squash(){
   // printf("Starting commit\n");
    // roll back tail pointer to empty AL
    AL->flush();
    // roll back head pointer of FL to tail (full)
    FL->flush();
    // clear GBM
    GBM = 0;
    // copy AMT to RMT
    RMT = AMT;
   // printf("Finished commit\n");
}

//////////////////////////////////////////
// Functions not tied to specific stage.//
//////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Functions for individually setting the exception bit,
// load violation bit, branch misprediction bit, and
// value misprediction bit, of the indicated entry in the Active List.
/////////////////////////////////////////////////////////////////////
void renamer::set_exception(uint64_t AL_index){
    assert(0 < AL_index < n_active);
    active_list_entry * entry = AL->at(AL_index);
    entry->exception = true;
}
void renamer::set_load_violation(uint64_t AL_index){
    assert(0 < AL_index < n_active);
    active_list_entry * entry = AL->at(AL_index);
    entry->load_violation = true;

}
void renamer::set_branch_misprediction(uint64_t AL_index){
    assert(0 < AL_index < n_active);
    active_list_entry * entry = AL->at(AL_index);
    entry->branch_mispred = true;

}
void renamer::set_value_misprediction(uint64_t AL_index){
    assert(0 < AL_index < n_active);
    active_list_entry * entry = AL->at(AL_index);
    entry->val_mispred = true;

}

/////////////////////////////////////////////////////////////////////
// Query the exception bit of the indicated entry in the Active List.
/////////////////////////////////////////////////////////////////////
bool renamer::get_exception(uint64_t AL_index){
    assert(0 < AL_index < n_active);
    active_list_entry * entry = AL->at(AL_index);
    return entry->exception;
}