/*
 * YaoGarbler.cpp
 *
 */

#include "YaoGarbler.h"
#include "YaoGate.h"

#include "GC/ThreadMaster.hpp"
#include "GC/Instruction.hpp"
#include "GC/Processor.hpp"
#include "GC/Program.hpp"
#include "GC/Machine.hpp"
#include "GC/Secret.hpp"
#include "GC/Thread.hpp"

thread_local YaoGarbler* YaoGarbler::singleton = 0;

YaoGarbler::YaoGarbler(int thread_num, YaoGarbleMaster& master) :
		GC::Thread<GC::Secret<YaoGarbleWire>>(thread_num, master),
		master(master),
		and_proc_timer(CLOCK_PROCESS_CPUTIME_ID),
		and_main_thread_timer(CLOCK_THREAD_CPUTIME_ID),
		player(master.N, 1, thread_num << 24),
		ot_ext(OTExtensionWithMatrix::setup(player,
				master.delta.get<__m128i>(), SENDER, true))
{
	prng.ReSeed();
	set_n_program_threads(master.machine.nthreads);

	and_jobs.resize(get_n_worker_threads());
	for (auto& job : and_jobs)
		job = new YaoAndJob(*this);
}

YaoGarbler::~YaoGarbler()
{
	for (auto& job : and_jobs)
		delete job;
#ifdef VERBOSE
	cerr << "Number of AND gates: " << counter << endl;
#endif
#ifdef YAO_TIMINGS
	cout << "AND time: " << and_timer.elapsed() << endl;
	cout << "AND process timer: " << and_proc_timer.elapsed() << endl;
	cout << "AND main thread timer: " << and_main_thread_timer.elapsed() << endl;
	cout << "AND prepare timer: " << and_prepare_timer.elapsed() << endl;
	cout << "AND wait timer: " << and_wait_timer.elapsed() << endl;
	for (auto& x : timers)
		cout << x.first << " time:" << x.second.elapsed() << endl;
#endif
}

void YaoGarbler::run(GC::Program<GC::Secret<YaoGarbleWire>>& program)
{
	singleton = this;

	GC::BreakType b = GC::TIME_BREAK;
	while(GC::DONE_BREAK != b)
	{
		try
		{
			b = program.execute(processor, master.memory, -1);
		}
		catch (needs_cleaning& e)
		{
			if (not continuous())
				throw runtime_error("run-time branching impossible with garbling at once");
			processor.PC--;
		}
		send(*P);
		gates.clear();
		output_masks.clear();
		if (continuous())
			process_receiver_inputs();
	}
}

void YaoGarbler::post_run()
{
	if (not continuous())
	{
		P->send_long(1, YaoCommon::DONE);
		process_receiver_inputs();
	}
}

void YaoGarbler::send(Player& P)
{
	P.send_long(1, YaoCommon::MORE);
	P.send_to(1, gates, true);
	P.send_to(1, output_masks, true);
}

void YaoGarbler::process_receiver_inputs()
{
	while (not receiver_input_keys.empty())
	{
		vector<Key>& inputs = receiver_input_keys.front();
		BitVector _;
		ot_ext.extend_correlated(inputs.size(), _);

		octetStream os;
		for (size_t i = 0; i < inputs.size(); i++)
			os.serialize(inputs[i] ^ ot_ext.senderOutputMatrices[0][i]);
		player.send(os);

		receiver_input_keys.pop_front();
	}
}