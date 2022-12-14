/*
 * YaoEvaluator.cpp
 *
 */

#include "YaoEvaluator.h"

#include "GC/Instruction.hpp"
#include "GC/Machine.hpp"
#include "GC/Program.hpp"
#include "GC/Processor.hpp"
#include "GC/Secret.hpp"
#include "GC/Thread.hpp"
#include "GC/ThreadMaster.hpp"

thread_local YaoEvaluator* YaoEvaluator::singleton = 0;

YaoEvaluator::YaoEvaluator(int thread_num, YaoEvalMaster& master) :
		Thread<GC::Secret<YaoEvalWire>>(thread_num, master),
		master(master),
		player(N, 0, thread_num << 24),
		ot_ext(OTExtensionWithMatrix::setup(player, {}, RECEIVER, true))
{
	set_n_program_threads(master.machine.nthreads);
}

void YaoEvaluator::pre_run()
{
	if (not continuous())
		receive_to_store(*P);
}

void YaoEvaluator::run(GC::Program<GC::Secret<YaoEvalWire>>& program)
{
	singleton = this;

	if (continuous())
		run(program, *P);
	else
	{
		run_from_store(program);
	}
}

void YaoEvaluator::run(GC::Program<GC::Secret<YaoEvalWire>>& program, Player& P)
{
	auto next = GC::TIME_BREAK;
	do
	{
		receive(P);
		try
		{
			next = program.execute(processor, master.memory, -1);
		}
		catch (needs_cleaning& e)
		{
		}
	}
	while(GC::DONE_BREAK != next);
}

void YaoEvaluator::run_from_store(GC::Program<GC::Secret<YaoEvalWire>>& program)
{
	machine.reset_timer();
	do
	{
		gates_store.pop(gates);
		output_masks_store.pop(output_masks);
	}
	while(GC::DONE_BREAK != program.execute(processor, master.memory, -1));
}

bool YaoEvaluator::receive(Player& P)
{
	if (P.receive_long(0) == YaoCommon::DONE)
		return false;
	P.receive_player(0, gates);
	P.receive_player(0, output_masks);
	return true;
}

void YaoEvaluator::receive_to_store(Player& P)
{
	while (receive(P))
	{
		gates_store.push(gates);
		output_masks_store.push(output_masks);
	}
}
