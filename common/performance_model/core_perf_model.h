#ifndef CORE_PERF_MODEL_H
#define CORE_PERF_MODEL_H
// This class represents the actual performance model for a given core

#include <queue>
#include <iostream>
#include <string>

// Forward Decls
class Core;
class BranchPredictor;
class Core;

#include "instruction.h"
#include "basic_block.h"
#include "fixed_types.h"
#include "lock.h"
#include "dynamic_instruction_info.h"

class CorePerfModel
{
public:

   CorePerfModel(Core* core, float frequency);
   virtual ~CorePerfModel();

   void queueDynamicInstruction(Instruction *i);
   void queueBasicBlock(BasicBlock *basic_block);
   void iterate();

   volatile float getFrequency() { return m_frequency; }
   void updateInternalVariablesOnFrequencyChange(volatile float frequency);
   void recomputeAverageFrequency(); 

   UInt64 getCycleCount() { return m_cycle_count; }
   void setCycleCount(UInt64 cycle_count);

   void pushDynamicInstructionInfo(DynamicInstructionInfo &i);
   void popDynamicInstructionInfo();
   DynamicInstructionInfo& getDynamicInstructionInfo();

   static CorePerfModel *createMainPerfModel(Core* core);
   static CorePerfModel *createPepPerfModel(Core* core);
   //static CorePerfModel *create(Core *core, Core::core_type_t core_type); 

   BranchPredictor *getBranchPredictor() { return m_bp; }

   void disable();
   void enable();
   bool isEnabled() { return m_enabled; }

   virtual void outputSummary(std::ostream &os) = 0;

   class AbortInstructionException { };


protected:
   friend class SpawnInstruction;

   typedef std::queue<DynamicInstructionInfo> DynamicInstructionInfoQueue;
   typedef std::queue<BasicBlock *> BasicBlockQueue;

   Core* getCore() { return m_core; }
   void frequencySummary(std::ostream &os);

   UInt64 m_cycle_count;

private:

   class DynamicInstructionInfoNotAvailableException { };

   virtual void handleInstruction(Instruction *instruction) = 0;

   Core* m_core;

   volatile float m_frequency;

   volatile float m_average_frequency;
   UInt64 m_total_time;
   UInt64 m_checkpointed_cycle_count;

   bool m_enabled;

   BasicBlockQueue m_basic_block_queue;
   Lock m_basic_block_queue_lock;

   DynamicInstructionInfoQueue m_dynamic_info_queue;
   Lock m_dynamic_info_queue_lock;

   UInt32 m_current_ins_index;

   BranchPredictor *m_bp;
};

#endif
